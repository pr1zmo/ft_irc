/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:13:38 by zelbassa          #+#    #+#             */
/*   Updated: 2025/10/05 21:31:35 by zelbassa         ###   ########.fr       */
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
	(void)cli;
	(void)param;
	cout << "Handling file transfer with param: " << param << endl;
}

void Privmsg::execute(Client &cli, const std::string& param){
	if (param.empty()) {
		cli.response("ERROR :No recipient given\r\n");
		return;
	}

	if (param.find("\\x01DCC SEND") != std::string::npos) {
		handleFileTransfer(cli, param);
		return;
	}

	cout << "PRIVMSG command executed with param: " << param << endl;
}

