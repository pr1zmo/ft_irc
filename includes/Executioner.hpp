/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Executioner.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/20 18:59:56 by zelbassa          #+#    #+#             */
/*   Updated: 2025/11/20 18:59:59 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ft_irc.h"
#include "Server.hpp"

class Client;
class Command;

class Executioner {
	private:

	public:
		Executioner();
		~Executioner();

		std::map<std::string, Command*> _commands;
		
		int run(Client &cli, const std::string &msg, std::map<int, Client>& clients, Server& server);
};
