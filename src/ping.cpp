/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:13:41 by zelbassa          #+#    #+#             */
/*   Updated: 2025/09/21 20:13:42 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.h"

Ping::Ping(/* args */)
: Command()
{
}

Ping::~Ping()
{
}

void Ping::execute(Client &cli, const std::string& param, const std::string& cmd, std::map<int, Client>& clients, Server& server) {
    (void)cli;
    (void)param;
    (void)cmd;
    (void)clients;
    // Implementation of PING command execution

    cli.response("PONG :server\r\n");
}