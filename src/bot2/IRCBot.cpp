#include "IRCBot.hpp"
#include <poll.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

IRCBot::IRCBot(INIParser &parser) : _sock_fd(-1)
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

    // long lport = std::strtol(server_port.c_str());
    // TODO: check for the port range
}

IRCBot::~IRCBot() {}

int IRCBot::sendMsg(const std::string &msg)
{
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
        close(_sock_fd);
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

    while (true)
    {
        pfd.revents = 0;

        int ret = poll(&pfd, 1, TIMEOUT_MS);

        if (ret < 0)
        {
            std::cerr << "poll() failed\n";
            break;
        }

        // Timeout
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

        // Incoming data
        if (pfd.revents & POLLIN)
        {
            std::memset(buf, 0, sizeof(buf));
            int r = recv(_sock_fd, buf, sizeof(buf) - 1, 0);

            // int r = recv(_sock_fd, buf, sizeof(buf), 0); // read full buffer now
            // std::cout << buf << std::endl;

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
                handleMsg(msg);
            }
        }

        if (pfd.revents & (POLLHUP | POLLERR | POLLNVAL))
        {
            std::cerr << "Socket error or hangup\n";
            break;
        }
    }

    close(_sock_fd);
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
        // std::cout << chan << std::endl;
        trim(chan);
        if (chan.empty())
            continue;

        ChannelInfo info;
        std::string sectionName = "channel." + chan;

        // Load optional values
        info.greeting = ini.get(sectionName, "greeting", "Hello!!!!!");
        info.autoJoin = (ini.get(sectionName, "auto_join", "true") == "true");

        _channels[chan] = info;
    }
}

void IRCBot::handleMsg(const std::string &raw)
{
    log("<<< " + raw + "\n");

    // if (raw.find(" 001 ") != std::string::npos)
    // {
    //     for (std::map<std::string, ChannelInfo>::iterator it = _channels.begin();
    //          it != _channels.end(); ++it)
    //     {
    //         if (it->second.autoJoin)
    //         {
    //             const std::string &chan = it->first;

    //             // Join the channel
    //             sendMsg("JOIN " + chan + "\r\n");

    //             // Send greeting message -- NOTE THE REQUIRED ':' BEFORE THE TEXT
    //             if (!it->second.greeting.empty())
    //             {
    //                 sendMsg("PRIVMSG " + chan + " :" + it->second.greeting + "\r\n");
    //             }
    //         }
    //     }
    //     return;
    // }
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

    // : nick!user@host PRIVMSG #chatroom :hello world
    // :asdasd PRIVMSG Bot42 :!help
    // Prefix
    if (i < len && raw[i] == ':')
    {
        size_t end = raw.find(' ', i);
        msg.prefix = raw.substr(i + 1, end - i - 1);
        i = end + 1;
    }

    // Command
    size_t end = raw.find(' ', i);
    if (end == std::string::npos)
    {
        msg.command = raw.substr(i);
        return msg;
    }

    msg.command = raw.substr(i, end - i);
    i = end + 1;

    // Params
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

    // Trailing text
    if (i < len && raw[i] == ':')
        msg.trailing = raw.substr(i + 1);

    return msg;
}

void IRCBot::log(const std::string &msg)
{
    if (_logging)
    {
        std::ofstream outfile(_log_file, std::ios::app);
        time_t now = time(NULL);
        // outfile << "[" << std::put_time(std::localtime(&now), "%F %T") << "] " << msg;
        char buf[64];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        outfile << "[" << buf << "] " << msg;
    }
}
