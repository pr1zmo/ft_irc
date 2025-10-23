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
  
    // Implementation of JOIN command execution

    //check if client is registered
    if (cli.isRegistered() == false) {
        cli.response(":server 451 * :You have not registered\r\n");
        return;
    }
    Channel* channel = server.getChannel(param);
    if (!channel) {
        // Create channel if it doesn't exist
        channel = new Channel(param);
        server.addChannel(param, channel);

    }
    if (!channel->addUser(cli.getNickname())) {
        cli.response("You are already in the channel");
        return;
    }
    cli.joinChannel(channel);
    // THE CREATOR OF THE CHANNEL BECOMES OP
    if (channel->userCount() == 1) {
        channel->addOp(cli.getNickname());
    }
    cli.response(":server 332 " + cli.getNickname() + " " + param + " :Welcome to " + param + "\r\n");
    channel->broadcast(cli.getNickname() + " has joined the channel");
}
