/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:13:38 by zelbassa          #+#    #+#             */
/*   Updated: 2025/10/13 15:53:50 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.h"

/*
 * usage: PRIVMSG <target> :<message>
 * in case of file transfer:
 * PRIVMSG <target> :\x01DCC SEND <filename> <ip> <port> <filesize>\x01
 * target: recipient's nickname
 * :\x01DCC SEND: indicates a DCC file transfer request
 * <filename>: name of the file being sent
 * <ip>: sender's IP address in integer format
 * <port>: port number on which the sender is listening
 * <filesize>: size of the file in bytes
 *  \x01: indicates the end of the DCC command
*/

Privmsg::Privmsg(/* args */)
: Command()
{
}

Privmsg::~Privmsg()
{
}

void Privmsg::handleFileTransfer(Client &cli, const std::string& param){
	File file;

	cout << "Handling file transfer with param: " << param << endl;
	file.execute(cli, param);
}

void Privmsg::execute(Client &cli, const std::string& param, const std::string& cmd, std::map<int, Client>& clients, Server& server) {
	(void)cmd;
	(void)clients;
	(void)server;
	// Implementation of PRIVMSG command execution
	if (param.empty()) {
		cli.response("ERROR :No recipient given\r\n");
		return;
	}

	size_t colonPos = param.find(" :");
	if (colonPos == std::string::npos) {
		cli.response("ERROR :No message given\r\n");
		return;
	}
	std::string target = param.substr(0, colonPos);
	std::string message = param.substr(colonPos + 2); // Skip " :"
	if (target.empty() || message.empty()) {
		cli.response("ERROR :Invalid PRIVMSG format\r\n");
		return;
	}

	bool userFound = false;
	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
		if (it->second.getNickname() == target) {
			std::cout << "Sending message to user: " << target << std::endl;
			it->second.response(":" + cli.getNickname() + " PRIVMSG " + target + " :" + message + "\r\n");
			it->second.sendPendingMessages();
			userFound = true;
			break;
		}
	}
	if (!userFound) {
		cli.response(":server 401 " + cli.getNickname() + " " + target + " :No such nick/channel\r\n");
		return;;
	}


	// if (param.find("\\x01DCC SEND") != std::string::npos) {
	// 	handleFileTransfer(cli, param);
	// 	return;
	// }

	cout << "PRIVMSG command executed with param: " << param << endl;
}

