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

Join::Join(/* args */)
: Command()
{
}

Join::~Join()
{
}

void Join::execute(Client &cli, const std::string& param, const std::string& cmd, std::map<int, Client>& clients) {
    (void)cli;
    (void)param;
    (void)cmd;
    (void)clients;
    // Implementation of JOIN command execution
}