/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pong.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/27 18:48:45 by zelbassa          #+#    #+#             */
/*   Updated: 2025/09/27 18:49:18 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.h"

Pong::Pong() {
	
}

Pong::~Pong() {
	
}

void Pong::execute(Client &cli, const std::string& param, const std::string& cmd, std::map<int, Client>& clients, Server& server) {
    (void)cli;
    (void)param;
    (void)cmd;
    (void)clients;
    // Implementation of PONG command execution
    cli.response("PONG :server\r\n");
}