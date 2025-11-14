/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCbot.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 21:50:52 by zelbassa          #+#    #+#             */
/*   Updated: 2025/11/14 15:09:09 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCbot.hpp"
#include "BotConfig.hpp"
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>
#include <algorithm>
#include <fstream>

/*
 * An irc bot that connects to the server like any other client
 * and responds to specific commands or messages
 * Simple Usage: ./irc_bot <server_ip> <server_port> <bot_nick>
*/

static vector<string> ft_split(const string &s, char delimiter) {
	vector<string> parts;
	string part;
	istringstream ss(s);
	while (getline(ss, part, delimiter)) {
		parts.push_back(part);
	}
	return parts;
}

int ft_error(int err, const string &msg)
{
	cerr << "Error: " << msg << ": (" << strerror(err) << ")" << endl;
	return err;
}

Bot::Bot(BotConf &conf) {
	string server_ip = conf.get("server.host");
	int server_port = atoi(conf.get("server.port").c_str());
	_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_sockfd < 0) {
		perror("Socket creation failed");
		throw std::invalid_argument("Socket creation failed");
	}

	memset(&_server_addr, 0, sizeof(_server_addr));
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_port = htons(server_port);
	if (inet_pton(AF_INET, server_ip.c_str(), &_server_addr.sin_addr) <= 0) {
		perror("Invalid address/ Address not supported");
		close(_sockfd);
		throw std::invalid_argument("Invalid address/ Address not supported");
	}
}

Bot::~Bot() {
	if (_sockfd >= 0)
		close(_sockfd);
}

void Bot::quit() {
	if (_sockfd >= 0)
		close(_sockfd);
}

string Bot::getUpTime() const {
	std::time_t now = std::time(0);
	long elapsed = static_cast<long>(now - startTime);

	long hours = elapsed / 3600;
	long minutes = (elapsed % 3600) / 60;
	long seconds = elapsed % 60;

	std::ostringstream ss;
	ss << "Uptime: "
		<< setw(2) << setfill('0') << hours << "h "
		<< setw(2) << setfill('0') << minutes << "m "
		<< setw(2) << setfill('0') << seconds << "s";

	return ss.str();
}

size_t	get_time(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return ((time.tv_sec * 1000) + (time.tv_usec / 1000));
}

bool Bot::connectToServer(){
	serv_fd = connect(_sockfd, (sockaddr *)&_server_addr, sizeof(_server_addr));
	if (serv_fd < 0) {
		startTime = time(0);
		return (ft_error(errno, "connect"), false);
	}
	return true;
}

int Bot::parseParams(char *msg, std::vector<std::string>& params){
	char *token = strtok(msg, " \r\n");
	while (token != NULL) {
		params.push_back(std::string(token));
		token = strtok(NULL, " \r\n");
	}
	return params.size();
}

int Bot::sendMessage(const std::string& message){
	ssize_t bytes_sent;
	bytes_sent = send(this->_sockfd, message.c_str(), message.size(), 0);
	if (bytes_sent < 0) {
		ft_error(errno, "send failed");
		return -1;
	}
	usleep(200000); // Sleep for 0.2 seconds to allow server to respond
	return bytes_sent;
}

static std::string strip_invisible(const std::string& s) {
	if (s.empty()) return s;

	int end = s.size() - 1;

	// move backwards until you find the last printable character
	while (end >= 0 && !std::isprint(static_cast<unsigned char>(s[end]))) {
		--end;
	}

	// if you want to trim invisible chars also from the start:
	int start = 0;
	while (start <= end && !std::isprint(static_cast<unsigned char>(s[start]))) {
		++start;
	}

	return s.substr(start, end - start + 1);
}

void Bot::handle_privmsg(const string &message, BotConf &conf, const string& sender){
	// Example PRIVMSG format: ":nickname!username@hostname PRIVMSG #channel :message text"
	// extract sender:
	size_t privmsg_pos = message.find("PRIVMSG ");
	if (privmsg_pos == string::npos) {
		return; // malformed message
	} else {
		size_t msg_start = message.find_last_of(':');
		if (msg_start != string::npos && msg_start + 1 < message.size()) {
			string msg_content = message.substr(msg_start + 1);
			msg_content = strip_invisible(msg_content);
			// Check for specific commands
			if (conf.has("response."+msg_content)){
				sendMessage("PRIVMSG " + sender + " :" + conf.get("response."+msg_content) + "\r\n");
			} else {
				sendMessage("PRIVMSG " + sender + " :I don't understand that command.\r\n");
			}
		}
	}
}

static string cmd_time(BotConf &conf){
	(void)conf;
	time_t now = time(0);
	tm *ltm = localtime(&now);
	ostringstream ss;
	ss << "Current Time: "
		<< setw(2) << setfill('0') << ltm->tm_hour << ":"
		<< setw(2) << setfill('0') << ltm->tm_min << ":"
		<< setw(2) << setfill('0') << ltm->tm_sec;
	return ss.str();
}

static string cmd_date(BotConf &conf){
	(void)conf;
	time_t now = time(0);
	tm *ltm = localtime(&now);
	ostringstream ss;
	ss << "Current Date: "
		<< setw(2) << setfill('0') << ltm->tm_mday << "/"
		<< setw(2) << setfill('0') << (ltm->tm_mon + 1) << "/"
		<< (1900 + ltm->tm_year);
	return ss.str();
}

static string cmd_uptime(BotConf &conf){
	time_t now = time(0);
	size_t startTime = static_cast<size_t>(atoi(conf.get("connection.start_time").c_str()));
	long elapsed = static_cast<long>(now - startTime);

	long hours = elapsed / 3600;
	long minutes = (elapsed % 3600) / 60;
	long seconds = elapsed % 60;

	ostringstream ss;
	ss << "Uptime: "
		<< setw(2) << setfill('0') << hours << "h "
		<< setw(2) << setfill('0') << minutes << "m "
		<< setw(2) << setfill('0') << seconds << "s";

	return ss.str();
}


void Bot::handle_system(const string &message, BotConf &conf, const string& sender){
	vector<string> cmd_list = ft_split(conf.get("commands.list"), ',');
	string trigger = message.substr(message.find("!") + 1, message.find(" ") - message.find("!") - 1);
	trigger = strip_invisible(trigger);

	typedef string (*CommandFunc)(BotConf &conf);
	map<string, CommandFunc> command_map;
	command_map["uptime"] = &cmd_uptime;
	command_map["date"] = &cmd_date;
	command_map["time"] = &cmd_time;
	for (size_t i = 0; i < cmd_list.size(); ++i){
		string cmd_key = "command." + trigger + ".trigger";
		if (conf.has(cmd_key) == false)
			continue;
		string cmd_trigger = conf.get(cmd_key);
		string cmd_response = command_map.find(trigger) != command_map.end() ? command_map[trigger](conf) : "Unknown command.";
		sendMessage("PRIVMSG " + sender + ":" + cmd_response + " " + "\r\n");
		return;
	}
}

void Bot::handle_response(const string &message, BotConf &conf){
	if (message.find("PING") != string::npos){
		string server = message.substr(message.find("PING") + 5);
		sendMessage("PONG " + server + "\r\n");
	}
	else if (message.find("PRIVMSG") != string::npos){
		string sender;
		size_t x = message.find(':');
		if (x != string::npos) {
			sender = message.substr(x + 1, message.find(" ")); // skip leading ':'
		} else {
			return; // malformed message
		}
		if (message.find("!") != string::npos)
			handle_system(message, conf, sender);
		else
			handle_privmsg(message, conf, sender);
	}
}

void Bot::join_channels(BotConf &conf){
	if (!conf.has("channels"))
		throw Bot::ConfigFileException();
	vector<string> channels = ft_split(conf.get("channels"), ',');
	for (u_long i = 0; i < channels.size(); i++){
		sendMessage("JOIN " + channels.at(i) + "\r\n");
	}
}

void Bot::run(BotConf &conf){
	char buffer[4096];
	memset(buffer, 0, sizeof(buffer));
	int bytes_received = recv(_sockfd, buffer, sizeof(buffer) - 1, 0);
	if (bytes_received < 0) {
		ft_error(errno, "recv failed");
		return;
	} else if (bytes_received == 0) {
		cout << "Connection closed by server." << endl;
		return;
	}
	buffer[bytes_received] = '\0';
	// cout << buffer;
	std::vector<std::string> params;

	_bot_nick = conf.get("identity.nickname");
	_server_pass = conf.get("server.password");
	sendMessage("PASS "+ _server_pass +"\r\n");
	sendMessage("NICK " + _bot_nick + "\r\n");
	sendMessage("USER " + _bot_nick + " 0 * :IRC Bot\r\n");

	join_channels(conf);
	memset(buffer, 0, sizeof(buffer));
	while (true){
		bytes_received = recv(_sockfd, buffer, sizeof(buffer) - 1, 0);
		if (bytes_received < 0) {
			ft_error(errno, "recv failed");
			break;
		} else if (bytes_received == 0) {
			cout << "Connection closed by server." << endl;
			break;
		}
		buffer[bytes_received] = '\0';
		if (conf.get("logging.enabled") == "true"){
			fstream logFile;
			logFile.open(conf.get("logging.log_file").c_str(), ios::out | ios::app);
			if (logFile.is_open()){
				logFile << buffer;
				logFile.close();
			}
		}
		handle_response(buffer, conf);
		if (parseParams(buffer, params) < 1){
			cout << "No parameters found in the message." << endl;
			return;
		}
	}
}

Bot *b_signal;

static string to_string98(int n) {
	ostringstream oss;
	oss << n;
	return oss.str();
}

bool running = true;

void on_signal(int x){
	if (x == SIGINT || x == SIGTERM)
		b_signal->quit();
}

void handle_signal(){
	struct sigaction sa;

	std::memset(&sa, 0, sizeof(sa));
	sa.sa_handler = on_signal;

	sigaction(SIGINT, &sa, 0);
	sigaction(SIGTERM, &sa, 0);
	sigaction(SIGQUIT, &sa, 0);
}

const char* Bot::ConfigFileException::what() const throw(){
	return "Wrong value in the config file\n";
}

int main(int ac, char **av) {
	(void)ac;

	try{
		BotConf conf;
		conf.load(av[1]);
		conf.get("server.host");
		Bot bot(conf);
		b_signal = &bot;
		handle_signal();
		if (bot.connectToServer()){
			cout << "Connected to server successfully.\n";
			if (conf.get("connection.start_time") == "0")
				conf.set("connection.start_time", to_string98(static_cast<size_t>(get_time() / 1000)));
			bot.run(conf);
		}
		else if (conf.get("connection.auto_reconnect") == "true"){
			cout << "Could not connect! attempting to reconnect...\n";
			int timeout = atoi(conf.get("connection.timeout").c_str());
			int reconnect_delay = atoi(conf.get("connection.reconnect_delay").c_str());
			size_t start = get_time();
			while ((get_time() - start) / 1000 < static_cast<size_t>(timeout)){
				cout << "Reconnecting...\n";
				if (bot.connectToServer()){
					cout << "Reconnected to server successfully.\n";
					if (conf.get("connection.start_time") == "0")
						conf.set("connection.start_time", to_string98(static_cast<size_t>(get_time() / 1000)));
					bot.run(conf);
					break;
				}
				sleep(reconnect_delay);
			}
		}
		else
			cout << "Could not connect\n";
	} catch (std::exception &e){
		cout << e.what() << endl;
	}

	return 0;
}