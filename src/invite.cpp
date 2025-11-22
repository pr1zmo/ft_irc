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
    std::istringstream iss(param);
    std::string channel_name;
    std::string target_nick;

    if (!cli.isRegistered()) {
        cli.response(":server 451 * :You have not registered\r\n");
        return;
    }
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

    if (!channel->isOp(cli.getNickname())) {
        cli.response(":server 482 " + cli.getNickname() + " " + channel_name + " :You're not channel operator\r\n");
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

    if (channel->contains(target_nick)) {
        cli.response(":server 443 " + cli.getNickname() + " " + target_nick + " " + channel_name + " :is already on channel\r\n");
        return;
    }

    if (channel->isInviteOnly() && !channel->isOp(cli.getNickname())) {
        cli.response(":server 473 " + cli.getNickname() + " " + channel_name + " :Cannot join channel (+i)\r\n");
        return;
    }

    channel->inviteUser(target_nick);

    // Feedback to the sender
    cli.response(":server 341 " + cli.getNickname() + " " + target_nick + " " + channel_name + "\r\n");

    // Notify the target user
    target_client->response(
    ":" + cli.getNickname() + "!" + cli.getUsername() + "@" + cli.getHostname()
    + " INVITE " + target_nick + " " + channel_name + "\r\n");
}