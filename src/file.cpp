/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 13:56:42 by zelbassa          #+#    #+#             */
/*   Updated: 2025/10/02 17:37:13 by zelbassa         ###   ########.fr       */
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
: Command() , _fileStream(), _recipient_ip(0), _recipient_port(0), sender(NULL)
{
	// cout << this->_msg << 'n';
}

File::~File() {
}

int File::establish_connection(socklen_t ip, int port){
	socklen_t recip_fd;

	recip_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (recip_fd < 0) {
		perror("Socket creation failed");
		return -1;
	}
	struct sockaddr_in rec_addr;
	memset(&rec_addr, 0, sizeof(rec_addr));
	rec_addr.sin_family = AF_INET;
	rec_addr.sin_addr.s_addr = ip; // Sender's IP address
	rec_addr.sin_port = htons(port); // Sender's port number

	if (connect(recip_fd, (struct sockaddr*)&rec_addr, sizeof(rec_addr)) < 0) {
		perror("Connection to sender failed");
		close(recip_fd);
		return -1;
	}
	// Connection established successfully
	close(recip_fd);
	return 1;
}

int File::parseFile() {
	//
	return 1;
}

int File::parseCommand(const string &msg) {
	// DCC <file to send> <ip of the recipient> <port of the recipient>
	vector<string> parts = split(msg, ' ');

	if (parts.size() < 3) {
		sender->queueMessage("Error: DCC command requires 3 parameters.\r\n");
		return 0;
	}
	
	_fileStream.open(parts[0].c_str(), ios::in | ios::binary);
	if (!_fileStream.is_open()) {
		sender->queueMessage("Error: Could not open file " + parts[0] + "\r\n");
		return 0;
	}

	string temp;
	while (std::getline(_fileStream, temp)){
		cout << temp << endl;
	}

	_recipient_ip = static_cast<socklen_t>(std::strtoul(parts[1].c_str(), NULL, 10));
	_recipient_port = std::atoi(parts[2].c_str());
	cout << msg << endl;
	_fileStream.close();
	return 1;
}

void File::execute(Client &cli, const string &msg){
	sender = &cli;
	if (!parseCommand(msg))
		return ;
	cout << "Executing FILE command with msg: " << msg << endl;
}