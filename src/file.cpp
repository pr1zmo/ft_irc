/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   File.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 13:56:42 by zelbassa          #+#    #+#             */
/*   Updated: 2025/09/26 14:25:30 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.h"

/*
 * DCC (Direct Client-to-Client) file transfer command implementation
 * Handles sending and receiving files between clients
 * Uses TCP sockets for file transfer
 * The sever only facilitates the initial connection setup
 * Actual file transfer happens directly between clients
 * PRIVMSG <target> :\x01DCC SEND <filename> <ip> <port> <filesize>\x01
 * Example: PRIVMSG john :\x01DCC SEND example.txt 3232235776 5000 12345\x01
 * The server connects the two clients then they handle the file transfer directly
*/

File::File()
: Command()
{
	cout << this->_msg << 'n';
}

File::~File() {
}

int File::establish_connection(socklen_t ip, int port){
	//
	return 1;
}

int File::parseFile() {
	//
	return 1;
}