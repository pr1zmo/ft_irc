/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:13:43 by zelbassa          #+#    #+#             */
/*   Updated: 2025/09/29 09:42:44 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.h"

void Pass::execute(Client &cli, const std::string& param,	 const std::string& cmd, std::map<int, Client>& clients, Server& server) {
	(void)cli;
	(void)param;
	(void)cmd;
	(void)clients;
	//
	// Implementation of PASS command execution
	std::cout << "PASS command executed with param: " << param << std::endl;
	if (param.empty()) {
		cli.response(":server 461 * PASS :Not enough parameters\r\n");
		return;
	}
	if (cli._isAuth) {
		cli.response(":server 462 * :You may not re-register\r\n");
		return;
	}
	// Here you would typically check the password against a stored value
	if (param == server.getPassword()) {
		cli._isAuth = true;
		//cli.response(":server 001 * :Welcome to the IRC server!\r\n");
	} else {
		cli.response(":server 464 * :Password incorrect\r\n");
	}
}