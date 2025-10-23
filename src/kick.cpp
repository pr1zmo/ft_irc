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

    if (!(iss >> target_nick)) {
        cli.response("Usage: " + cmd + " <nick> [reason]\r\n");
        return;
    }
    std::getline(iss, reason);
    if (!reason.empty() && reason[0] == ' ') reason = reason.substr(1);
    if (reason.empty()) {
        reason = "No reason specified";
    }
    
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
        target_client.response(":" + cli.getNickname() + " KICK " + target_nick + " :" + reason + "\r\n");
        target_client.sendPendingMessages();

        const std::map<std::string, Channel*>& channels = server.getChannels();
        std::map<std::string, Channel*>::const_iterator cit = channels.begin();
        for (; cit != channels.end(); ++cit) {
            Channel* channel = cit->second;
            if (!channel) continue;
            if (channel->contains(target_nick)) {
                if (channel->isOp(cli.getNickname()))
                {
                    channel->kickUser(target_nick, reason);
                    cli.response("You kicked " + target_nick + " from " + channel->getName() + " :" + reason + "\r\n");
                    channel->broadcast(cli.getNickname(), ":" + cli.getNickname() + " KICK " + target_nick + " FROM " + channel->getName() + " :" + reason, server);
                } else {
                    cli.response(":" + cli.getNickname() + " NOTICE * :You are not channel operator in " + channel->getName() + "\r\n");
                    continue;
                }
            }
        }
    } else {
        cli.response(":" + cli.getNickname() + " NOTICE * :User " + target_nick + " not found\r\n");
    }
}


