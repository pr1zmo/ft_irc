/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:13:35 by zelbassa          #+#    #+#             */
/*   Updated: 2025/11/23 21:40:32 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.h"

Quit::Quit(/* args */)
: Command()
{
}

Quit::~Quit()
{
}

void Quit::execute(Client &cli, const std::string& param, const std::string& cmd, std::map<int, Client>& clients, Server& server) {
	(void)cmd;
	(void)clients;

	std::string quitMessage = param.empty() ? "Quit" : param;
	std::string quitMsg = ":" + cli.getNickname() + "!" + cli.getUsername() + "@" + cli.getHostname() +
								" QUIT :" + quitMessage + "\r\n";

	std::map<std::string, Channel*>& channels = server.getChannels(); 
	for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it) {
		Channel* channel = it->second; 
		if (channel->hasUser(cli.getNickname())) { 
			channel->broadcast(cli.getNickname(), quitMsg, server); 
			channel->removeUser(cli.getNickname());
		}
	}


	cli.response(quitMsg); 
	cli.markDisconnected();
}