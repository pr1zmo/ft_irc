/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   oper.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:13:51 by zelbassa          #+#    #+#             */
/*   Updated: 2025/10/29 12:51:51 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.h"

Oper::Oper(/* args */)
: Command()
{
}

Oper::~Oper()
{
}

void Oper::execute(Client &cli, const std::string& param, const std::string& cmd, std::map<int, Client>& clients, Server& server) {
	(void)cli;
	(void)param;
	(void)cmd;
	(void)clients;
	(void)server;
	// Implementation of OPER command execution
}