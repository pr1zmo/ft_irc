/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:13:35 by zelbassa          #+#    #+#             */
/*   Updated: 2025/09/21 20:13:36 by zelbassa         ###   ########.fr       */
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
    (void)cli;
    (void)param;
    (void)cmd;
    (void)clients;
    // Implementation of QUIT command execution
}