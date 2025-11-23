#ifndef IRCBOT_HPP
#define IRCBOT_HPP

#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstring>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <map>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <sstream>
<<<<<<< HEAD
=======
#include <ostream>
>>>>>>> 546865a6eb5ab010618260e237c9889e34a29b11

#include "INIParser.hpp"

#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT "6667"
#define DEFAULT_PASS "pass"
<<<<<<< HEAD

// TODO
// #define DEFAULT_NICK "BOT_"

struct ChannelInfo
{
	bool autoJoin;
	std::string greeting;
=======
extern int sig;

struct ChannelInfo
{
    bool autoJoin;
    std::string greeting;
>>>>>>> 546865a6eb5ab010618260e237c9889e34a29b11
};

struct IRCMessage
{
<<<<<<< HEAD
	std::string prefix;
	std::string command;
	std::vector<std::string> params;
	std::string trailing;
=======
    std::string prefix;
    std::string command;
    std::vector<std::string> params;
    std::string trailing;
>>>>>>> 546865a6eb5ab010618260e237c9889e34a29b11
};

class IRCBot
{
<<<<<<< HEAD

	// struct sockaddr_in _server_addr;
	int _sock_fd;
	std::string _nickname;
	std::string _username;
	std::string _realname;
	std::string _server_host;
	std::string _server_port;
	std::string _server_pass;
	bool _logging;
	std::string _log_file;
	std::map<std::string, ChannelInfo> _channels;

public:
	IRCBot(INIParser &parser);
	~IRCBot();
	int sendMsg(const std::string &msg);
	void handleMsg(const std::string &msg);
	void loadChannels(INIParser &ini);
	std::string buildJoinList();
	void log(const std::string &msg);
	bool connectToServer();
	void sendRegistration();
	IRCMessage parseMsg(const std::string &raw);
	void run();

	void dispatchCommand(const std::string &nick, const std::string &target, const std::string &cmd, const std::string &args);
	void sendPriv(const std::string &target, const std::string &text);
	void cmdPing(const std::string &nick, const std::string &target);
	void cmdSay(const std::string &nick, const std::string &target, const std::string &args);
	void cmdJoin(const std::string &, const std::string &chan);
	void cmdPart(const std::string &, const std::string &chan);
	void cmdHelp(const std::string &, const std::string &target);
};


=======
    
    // struct sockaddr_in _server_addr;
    int _sock_fd;
    std::string _nickname;
    std::string _username;
    std::string _realname;
    std::string _server_host;
    std::string _server_port;
    std::string _server_pass;
    bool _registered;
    bool _logging;
    std::string _log_file;
    std::map<std::string, ChannelInfo> _channels;

public:
    IRCBot(INIParser &parser);
    ~IRCBot();
    int sendMsg(const std::string &msg);
    void handleMsg(const std::string &msg);
    void loadChannels(INIParser &ini);
    std::string buildJoinList();
    void log(const std::string &msg);
    bool connectToServer();
    void sendRegistration();
    bool handleRegister(const std::string &raw);
    IRCMessage parseMsg(const std::string &raw);
    void run();
    void dispatchCommand(const std::string &nick, const std::string &target, const std::string &cmd, const std::string &args);
    void sendPriv(const std::string &target, const std::string &text);
    void cmdPing(const std::string &nick, const std::string &target);
    void cmdSay(const std::string &target, const std::string &args);
    void cmdJoin(const std::string &, const std::string &chan);
    void cmdPart(const std::string &, const std::string &chan);
    void cmdHelp(const std::string &, const std::string &target);
};


void handle_signals();
>>>>>>> 546865a6eb5ab010618260e237c9889e34a29b11
void splitCommand(const std::string &input, std::string &cmd, std::string &args);

#endif