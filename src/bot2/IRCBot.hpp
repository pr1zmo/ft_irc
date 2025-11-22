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

#include "INIParser.hpp"

#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT "6667"
#define DEFAULT_PASS "pass"

// TODO
// #define DEFAULT_NICK "BOT_"

struct ChannelInfo
{
    bool autoJoin;
    std::string greeting;
};

struct IRCMessage
{
    std::string prefix;
    std::string command;
    std::vector<std::string> params;
    std::string trailing;
};

class IRCBot
{

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


void splitCommand(const std::string &input, std::string &cmd, std::string &args);

#endif