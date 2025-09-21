/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:14:11 by zelbassa          #+#    #+#             */
/*   Updated: 2025/09/21 20:14:12 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.h"

Command::Command()
: _size(0), _msg("") {
	_isOp = 0;
	std::string cmds[] = {"PASS", "NICK", "USER", "OPER", "QUIT", "JOIN", "PART", "PRIVMSG", "NOTICE", "PING", "PONG", "MODE", "TOPIC", "KICK", "LIST", "NAMES"};
	for (size_t i = 0; i < sizeof(cmds)/sizeof(cmds[0]); ++i) {
		validCmds.push_back(cmds[i]);
	}
}

Command::~Command() {
}

int Command::parseCommand(char *msg) {
	_size = strlen(msg);
	if (_size > BUFFER_SIZE) {
		return ERR_INPUTTOOLONG;
	}
	_msg = std::string(msg);
	return 0;
}

