/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:13:35 by zelbassa          #+#    #+#             */
/*   Updated: 2025/10/28 21:04:52 by zelbassa         ###   ########.fr       */
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
	std::string quitMessage = param.empty() ? "Client Quit" : param;
	cli.response("ERROR :Closing Link: " + cli.getNickname() + " (" + quitMessage + ")\r\n");
	cli.markDisconnected();
}