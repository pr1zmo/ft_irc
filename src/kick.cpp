#include "Commands.hpp"
#include <cctype>
#include <algorithm>
#include <sstream>
#include <string>
#include <map>
#include "ft_irc.h"
#include "Server.hpp"
#include "Channel.hpp"


Kick::Kick(/* args */)
: Command()
{
}
Kick::~Kick()
{
}

void Kick::execute(Client &cli, const std::string& param, const std::string& cmd, std::map<int, Client>& clients, Server& server) {
    std::istringstream iss(param);
    std::string target_nick;
    std::string reason;
    std::string channelName;        

    if (!(iss >> channelName >> target_nick)) {
        cli.response("Usage: " + cmd + " #channel <nick> [reason]\r\n");
        return;
    }
    std::getline(iss, reason);
    if (!reason.empty() && reason[0] == ' ') reason = reason.substr(1);
    if (reason.empty()) {
        reason = "No reason specified";
    }
    // basic validation
    if (channelName.empty() || channelName[0] != '#') {
        cli.response(":" + cli.getNickname() + " NOTICE * :Invalid channel name\r\n");
        return;
    }

    cout << "KICK command: target_nick=" << target_nick << ", reason=" << reason << endl;

    std::map<int, Client>::iterator it = clients.begin();
    bool found = false;
    for (; it != clients.end(); ++it) {
        if (it->second.getNickname() == target_nick) {
            found = true;
            break;
        }
    }

    if (found) {
        Client& target_client = it->second;


        Channel* channel = server.getChannel(channelName);
        if (!channel) {
            cli.response(":" + cli.getNickname() + " NOTICE * :No such channel\r\n");
        } else {
            if (!channel->contains(target_nick)) {
            cli.response(":" + cli.getNickname() + " NOTICE * :User " + target_nick + " is not on channel " + channel->getName() + "\r\n");
            } else if (!channel->isOp(cli.getNickname())) {
            cli.response(":" + cli.getNickname() + " NOTICE * :You are not channel operator in " + channel->getName() + "\r\n");
            } else {
            channel->kickUser(target_nick, reason);
            target_client.response(":" + cli.getNickname() + " KICK " + target_nick + " :" + reason + "\r\n");
            target_client.sendPendingMessages();
            cli.response("You kicked " + target_nick + " from " + channel->getName() + " :" + reason + "\r\n");
            channel->broadcast(cli.getNickname(), ":" + cli.getNickname() + " KICK " + target_nick + " FROM " + channel->getName() + " :" + reason, server);
            }
        }
    } else {
        cli.response(":" + cli.getNickname() + " NOTICE * :User " + target_nick + " not found\r\n");
    }
}


