/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Executioner.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/20 19:00:51 by zelbassa          #+#    #+#             */
/*   Updated: 2025/11/24 16:07:04 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Executioner.hpp"

Executioner::Executioner()
{
	_commands["PASS"] = new Pass();
	_commands["NICK"] = new Nick();
	_commands["USER"] = new User();
	_commands["QUIT"] = new Quit();
	_commands["JOIN"] = new Join();
	_commands["TOPIC"] = new Topic();
	_commands["MODE"] = new Mode();
	_commands["INVITE"] = new Invite();
	_commands["LIST"] = new List();
	_commands["KICK"] = new Kick();
	_commands["PART"] = new Part();
	_commands["PRIVMSG"] = new Privmsg();
	_commands["PING"] = new Ping();
	_commands["PONG"] = new Pong();
	_commands["HELP"] = new Help();
}

Executioner::~Executioner() {
	for (std::map<std::string, Command*>::iterator it = _commands.begin(); it != _commands.end(); ++it)
		delete it->second;
	_commands.clear();
}

int Executioner::run(Client &cli, const std::string &msg, std::map<int, Client>& clients, Server& server) {
	Command command;
	int ret = command.parseCommand(const_cast<char*>(msg.c_str()));
	if (ret == 0) {
		cli.response("Error: Command too long.\r\n");
		return ret;
	}

	size_t pos = msg.find(' ');
	std::string cmd = (pos == std::string::npos) ? msg : msg.substr(0, pos);
	std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
	std::map<std::string, Command*>::iterator it = _commands.find(cmd);
	if (it != _commands.end()) {
		Command* cmdInstance = it->second;
		cmdInstance->execute(cli, (pos == std::string::npos) ? "" : msg.substr(pos + 1), cmd, clients, server);
		if (cli._isAuth && ( cmd == "NICK" || cmd == "USER")) {
			if (!cli.getNickname().empty() && !cli.getUsername().empty() && !cli.isRegistered()) {	
				cli.response(":server 001 " + cli.getNickname() + " :Welcome to the IRC server! Registered successfully\r\n");
				cli.registerClient();
			}
		}
	} else {
		cli.response(cli.getNickname() + ": Unknown command: " + cmd + "\r\n");
			return 0;
		return -1;
	}
	return 0;
}