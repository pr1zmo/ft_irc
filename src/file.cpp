/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 13:56:42 by zelbassa          #+#    #+#             */
/*   Updated: 2025/10/01 10:03:01 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.h"

/*
 * DCC (Direct Client-to-Client) file transfer command implementation
 * Handles sending and receiving files between clients
 * Uses TCP sockets for file transfer
 * The sever only facilitates the initial connection setup
 * Actual file transfer happens directly between clients
 * PRIVMSG <target> :\x01DCC SEND <filename> <ip> <port> <filesize>\x01{
 * 	target: recipient's nickname
 * 	:\x01DCC SEND: indicates a DCC file transfer request
 * 	<filename>: name of the file being sent
 * 	<ip>: sender's IP address in integer format
 * 	<port>: port number on which the sender is listening
 * 	<filesize>: size of the file in bytes
 *    \x01: indicates the end of the DCC command
 * }
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
	socklen_t rec_sk;

	rec_sk = socket(AF_INET, SOCK_STREAM, 0);
	if (rec_sk < 0) {
		perror("Socket creation failed");
		return -1;
	}
	struct sockaddr_in rec_addr;
	memset(&rec_addr, 0, sizeof(rec_addr));
	rec_addr.sin_family = AF_INET;
	rec_addr.sin_addr.s_addr = ip; // Sender's IP address
	rec_addr.sin_port = htons(port); // Sender's port number

	if (connect(rec_sk, (struct sockaddr*)&rec_addr, sizeof(rec_addr)) < 0) {
		perror("Connection to sender failed");
		close(rec_sk);
		return -1;
	}
	// Connection established successfully
	close(rec_sk);
	return 1;
}

int File::parseFile() {
	//
	return 1;
}

void File::execute(Client &cli, const string &msg){
	
}