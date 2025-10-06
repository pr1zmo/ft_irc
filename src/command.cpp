/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:14:11 by zelbassa          #+#    #+#             */
/*   Updated: 2025/10/06 14:49:24 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.h"

Command::Command()
: _size(0), _msg(""), _isOp(0) {
	std::string cmds = "PASS NICK USER OPER MODE QUIT JOIN PART TOPIC NAMES LIST INVITE KICK PRIVMSG NOTICE PING PONG WHOIS WHO MODE AWAY DCC";

	for (size_t i = 0; i < cmds.size(); ) {
		size_t j = cmds.find(' ', i);
		if (j == std::string::npos) j = cmds.size();
		validCmds.push_back(cmds.substr(i, j - i));
		i = j + 1;
	}
}

Command::~Command() {
}

int Command::parseCommand(char *msg) {
	_size = strlen(msg);
	cout << "The size: " << _size << '\n';
	if (_size > BUFFER_SIZE) {
		return 0;
	}
	_msg = std::string(msg);
	return 1;
}

void Command::execute(Client &cli, const std::string& param) {
	(void)cli;
	(void)param;
}