/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:13:46 by zelbassa          #+#    #+#             */
/*   Updated: 2025/09/21 20:13:47 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.h"

Part::Part(/* args */)
: Command()
{
}

Part::~Part()
{
}

void Part::execute(Client &cli, const std::string& param, const std::string& cmd, std::map<int, Client>& clients, Server& server) {
    (void)cli;
    (void)param;
    (void)cmd;
    (void)clients;
    // Implementation of PART command execution
}