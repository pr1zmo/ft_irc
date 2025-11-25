#include "IRCBot.hpp"
#include <poll.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <signal.h>

int sig = 0;

void on_signal(int)
{
	sig = 1;
}

void handle_signals()
{
	struct sigaction sa;

	std::memset(&sa, 0, sizeof(sa));
	sa.sa_handler = on_signal;

	sigaction(SIGINT, &sa, 0);
	sigaction(SIGTERM, &sa, 0);
	sigaction(SIGQUIT, &sa, 0);

}

IRCBot::IRCBot(INIParser &parser) : _sock_fd(-1), _registered(false)
{
	_server_host = parser.get("server", "host", DEFAULT_HOST);
	_server_port = parser.get("server", "port", DEFAULT_PORT);
	_server_pass = parser.get("server", "password", DEFAULT_PASS);
	_nickname = parser.get("identity", "nickname", "BOT_1234");
	_username = parser.get("identity", "username", "botuser");
	_realname = parser.get("identity", "realname", "IRC Bot");
	_logging = (parser.get("logging", "enabled", "false") == "true");
	_log_file = parser.get("logging", "log_file", "bot.logs");
	loadChannels(parser);
}

IRCBot::~IRCBot()
{
	if (_sock_fd >= 0)
		close(_sock_fd);
}

int IRCBot::sendMsg(const std::string &msg)
{

	log("<<< " + msg);
	size_t bytes_sent = send(_sock_fd, msg.c_str(), msg.size(), 0);
	return bytes_sent;
}

bool IRCBot::connectToServer()
{
	struct addrinfo hints;
	struct addrinfo *res = NULL;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	int gai = getaddrinfo(_server_host.c_str(), _server_port.c_str(), &hints, &res);
	if (gai != 0)
	{
		std::cerr << "getaddrinfo failed: " << gai_strerror(gai) << std::endl;
		return false;
	}

	int sock = -1;
	for (struct addrinfo *rs = res; rs != NULL; rs = rs->ai_next)
	{
		sock = socket(rs->ai_family, rs->ai_socktype, rs->ai_protocol);
		if (sock < 0)
			continue;

		if (connect(sock, rs->ai_addr, rs->ai_addrlen) == 0)
		{
			_sock_fd = sock;
			freeaddrinfo(res);
			return true;
		}

		close(sock);
		sock = -1;
	}

	freeaddrinfo(res);
	return false;
}

void IRCBot::run()
{
	if (!connectToServer())
	{
		std::cerr << "Failed to connect to the server" << std::endl;
		return;
	}

	sendRegistration();

	struct pollfd pfd;
	pfd.fd = _sock_fd;
	pfd.events = POLLIN;

	int TIMEOUT_MS = 10000;
	unsigned long lastPing = time(NULL);

	char buf[4096];
	std::string _recvBuffer;

	while (!sig)
	{
		pfd.revents = 0;

		int ret = poll(&pfd, 1, TIMEOUT_MS);

		if (ret < 0)
		{
			std::cerr << "poll() failed\n";
			sig = 1;
			break;
		}

		if (ret == 0)
		{
			unsigned long now = time(NULL);

			if (now - lastPing >= 30)
			{
				sendMsg("PING :BotCheck\r\n");
				lastPing = now;
			}

			continue;
		}

		if (pfd.revents & POLLIN)
		{
			std::memset(buf, 0, sizeof(buf));
			int r = recv(_sock_fd, buf, sizeof(buf) - 1, 0);

			if (r <= 0)
			{
				std::cerr << "Disconnected from server\n";
				break;
			}

			_recvBuffer.append(buf, r);

			size_t pos;
			while ((pos = _recvBuffer.find("\r\n")) != std::string::npos)
			{
				std::string msg = _recvBuffer.substr(0, pos);
				_recvBuffer.erase(0, pos + 2);
				if (_registered)
					handleMsg(msg);
				else if (!(_registered = handleRegister(msg)))
				{
					if (msg.find(" 433 ") != std::string::npos)
					{
						std::cerr << "Nickname already in use!\n";
						close(_sock_fd);
						return;
					}
					if (msg.find(" 464 ") != std::string::npos)
					{
						std::cerr << "Password incorrect!\n";
						close(_sock_fd);
						return;
					}
				}
			}
		}
		if (pfd.revents & (POLLHUP | POLLERR | POLLNVAL))
		{
			std::cerr << "Socket error or hangup" << std::endl;
			break;
		}
	}
	sendMsg("QUIT\r\n");
}


bool isValidChannelName(const std::string &name)
{
    if (name.empty())
        return false;

    if (name[0] != '#' && name[0] != '&' && name[0] != '+' && name[0] != '!')
        return false;

    if (name.length() < 2 || name.length() > 50)
        return false;

    for (size_t i = 1; i < name.length(); i++)
    {
        char c = name[i];

        if (c == ' '  ||  
            c == ','  ||  
            c == '\a' ||  
            c == ':') 
        {
            return false;
        }
    }

    return true;
}
void IRCBot::loadChannels(INIParser &ini)
{
	std::string list = ini.get("global", "channels", "");
	if (list.empty())
		return;

	std::stringstream ss(list);
	std::string chan;

	while (std::getline(ss, chan, ','))
	{
		trim(chan);
		if (chan.empty())
			continue;

        if (!isValidChannelName(chan))
        {
            std::cerr << "Invalid channel name in config: " << chan << "\n";
            continue;
        }

		ChannelInfo info;
		std::string sectionName = "channel." + chan;

		info.greeting = ini.get(sectionName, "greeting", "Hello!!!!!");
		info.autoJoin = (ini.get(sectionName, "auto_join", "true") == "true");

		_channels[chan] = info;
	}
}

bool IRCBot::handleRegister(const std::string &raw)
{
	if (raw.find(" 001 ") != std::string::npos)
	{
		for (std::map<std::string, ChannelInfo>::iterator it = _channels.begin();
				it != _channels.end(); ++it)
		{
			if (it->second.autoJoin)
			{
				const std::string &chan = it->first;

				sendMsg("JOIN " + chan + "\r\n");

				if (!it->second.greeting.empty())
				{
					sendMsg("PRIVMSG " + chan + " :" + it->second.greeting + "\r\n");
				}
			}
		}
		return true;
	}
	return false;
}

void IRCBot::handleMsg(const std::string &raw)
{
	log(">>> " + raw + "\n");

	IRCMessage msg = parseMsg(raw);

	if (msg.command == "PRIVMSG" && !msg.params.empty())
	{
		const std::string &target = msg.params[0];
		const std::string &text = msg.trailing;

		if (!text.empty() && text[0] == '!')
		{
			std::string cmd, args;
			splitCommand(text.substr(1), cmd, args);
			dispatchCommand(_nickname, target, cmd, args);
		}
	}
}

IRCMessage IRCBot::parseMsg(const std::string &raw)
{
	IRCMessage msg;

	size_t i = 0;
	size_t len = raw.size();

	if (i < len && raw[i] == ':')
	{
		size_t end = raw.find(' ', i);
		msg.prefix = raw.substr(i + 1, end - i - 1);
		i = end + 1;
	}

	size_t end = raw.find(' ', i);
	if (end == std::string::npos)
	{
		msg.command = raw.substr(i);
		return msg;
	}

	msg.command = raw.substr(i, end - i);
	i = end + 1;

	while (i < len && raw[i] != ':')
	{
		end = raw.find(' ', i);
		if (end == std::string::npos)
		{
			msg.params.push_back(raw.substr(i));
			return msg;
		}
		msg.params.push_back(raw.substr(i, end - i));
		i = end + 1;
	}

	if (i < len && raw[i] == ':')
		msg.trailing = raw.substr(i + 1);

	return msg;
}

void IRCBot::log(const std::string &msg)
{
	if (_logging)
	{
		std::ofstream outfile(_log_file.c_str(), std::ios::app);
		time_t now = time(NULL);
		char buf[64];
		strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
		outfile << "[" << buf << "] " << msg;
	}
}