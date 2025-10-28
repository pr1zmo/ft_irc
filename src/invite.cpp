#include "Commands.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include "ft_irc.h"


Invite::Invite(/* args */)
: Command()
{
}
Invite::~Invite()
{
}
void Invite::execute(Client &cli, const std::string& param, const std::string& cmd, std::map<int, Client>& clients, Server& server) {
    // Implementation of INVITE command execution
    std::istringstream iss(param);
    std::string channel_name;
    std::string target_nick;

    if (!(iss >> target_nick >> channel_name)) {
        cli.response("Usage: " + cmd + " <nickname> <#channel>\r\n");
        return;
    }

    Channel* channel = server.getChannel(channel_name);
    if (!channel) {
        cli.response(":server 403 " + cli.getNickname() + " " + channel_name + " :No such channel\r\n");
        return;
    }

    if (!channel->contains(cli.getNickname())) {
        cli.response(":server 442 " + cli.getNickname() + " " + channel_name + " :You're not on that channel\r\n");
        return;
    }

    Client* target_client = NULL;
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->second.getNickname() == target_nick) {
            target_client = &(it->second);
            break;
        }
    }
    if (!target_client) {
        cli.response(":server 401 " + cli.getNickname() + " " + target_nick + " :No such nick/channel\r\n");
        return;
    }

    channel->inviteUser(target_nick);
    cli.response(":server 341 " + cli.getNickname() + " " + target_nick + " " + channel_name + "\r\n");
    target_client->response(":" + cli.getNickname() + " INVITE " + target_nick + " :" + channel_name + "\r\n");
}