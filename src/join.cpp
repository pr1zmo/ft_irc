/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:14:02 by zelbassa          #+#    #+#             */
/*   Updated: 2025/09/21 20:14:03 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.h"
#include "Channel.hpp"

Join::Join(/* args */)
: Command()
{
}

Join::~Join()
{
}

void Join::execute(Client &cli, const std::string& param, const std::string& cmd, std::map<int, Client>& clients, Server& server) {
    // check registration
    if (!cli.isRegistered()) {
        cli.response(":server 451 * :You have not registered\r\n");
        return;
    }

    if (param.empty()) {
        cli.response("Error: JOIN command requires a channel name.\r\n");
        return;
    }
    if (param[0] != '#') {
        cli.response("Error: Invalid channel name. Channel names must start with '#'.\r\n");
        return;
    }

    // parse channel name and optional password without modifying param
    std::string channelName;
    std::string password;
    size_t pos = param.find(' ');
    if (pos == std::string::npos) {
        channelName = param;
    } else {
        channelName = param.substr(0, pos);
        password = param.substr(pos + 1);
    }

    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        // create and configure new channel
        channel = new Channel(channelName);
        channel->setInviteOnly(false);
        channel->setTopicRestricted(false);
        channel->setPasswordProtected(false);
        channel->setUserLimit(100);
        if (!password.empty()) {
            channel->setPassword(password);
            channel->setPasswordProtected(true);
        }
        server.addChannel(channelName, channel);
    }


    if (channel->isPasswordProtected()) {
        if (password.empty() || !channel->checkPassword(password)) {
            cli.response(":server 475 " + cli.getNickname() + " " + channelName + " :Cannot join (+k)\r\n");
            return;
        }
    }
    // if (channel->isInviteOnly() && !channel->isInvited(cli.getNickname()) && !channel->isOp(cli.getNickname())) {
    //     cli.response(":server 473 " + cli.getNickname() + " " + channelName + " :Cannot join (invite only)\r\n");
    //     return;
    // }

    // attempt to add user
    if (!channel->addUser(cli.getNickname(), &cli)) {
        cli.response(":server 443 " + cli.getNickname() + " " + channelName + " :is already on channel\r\n");
        return;
    }

    // mark as joined
    cli.joinChannel(channel);

    // first member becomes op
    if (channel->userCount() == 1) {
        channel->addOp(cli.getNickname());
    }

    // send join confirmations / topic / names as appropriate
    cli.response(":server 332 " + cli.getNickname() + " " + channelName + " :Welcome to " + channelName + "\r\n");

    // broadcast JOIN to channel members
    channel->broadcast(cli.getNickname(), ":" + cli.getNickname() + " JOIN " + channelName + "\r\n", server);
}

