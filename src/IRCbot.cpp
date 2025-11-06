/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCbot.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 21:50:52 by zelbassa          #+#    #+#             */
/*   Updated: 2025/11/06 18:56:22 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCbot.hpp"
#include <sys/wait.h>
#include <sys/types.h>

/*
 * An irc bot that connects to the server like any other client
 * and responds to specific commands or messages
 * Simple Usage: ./irc_bot <server_ip> <server_port> <bot_nick>
*/

int ft_error(int err, const string &msg)
{
	cerr << "Error: " << msg << ": (" << strerror(err) << ")" << endl;
	return err;
}

Bot::Bot(const std::string& server_ip, int server_port, const std::string& bot_nick)
	: _bot_nick(bot_nick) {
	_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_sockfd < 0) {
		perror("Socket creation failed");
		throw std::invalid_argument("Socket creation failed");
	}

	memset(&_server_addr, 0, sizeof(_server_addr));
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_port = htons(server_port);
	if (inet_pton(AF_INET, server_ip.c_str(), &_server_addr.sin_addr) <= 0) {
		perror("Invalid address/ Address not supported");
		close(_sockfd);
		throw std::invalid_argument("Invalid address/ Address not supported");
	}
}

Bot::~Bot() {
	close(_sockfd);
}

bool Bot::connectToServer(){
	if ((serv_fd = connect(_sockfd, (sockaddr *)&_server_addr, sizeof(_server_addr))) < 0)
		return (ft_error(errno, "connect"), false);
	return true;
}

int Bot::parseParams(char *msg, std::vector<std::string>& params){
	char *token = strtok(msg, " \r\n");
	while (token != NULL) {
		params.push_back(std::string(token));
		token = strtok(NULL, " \r\n");
	}
	return params.size();
}

int sendMessage(int sockfd, const std::string& message){
	ssize_t bytes_sent = send(sockfd, message.c_str(), message.size(), 0);
	if (bytes_sent < 0) {
		ft_error(errno, "send failed");
		return -1;
	}
	usleep(500000); // Sleep for 0.5 seconds to allow server to respond
	return bytes_sent;
}

void Bot::run(){
	char buffer[4096];
	memset(buffer, 0, sizeof(buffer));
	int bytes_received = recv(_sockfd, buffer, sizeof(buffer) - 1, 0);
	if (bytes_received < 0) {
		ft_error(errno, "recv failed");
		return;
	} else if (bytes_received == 0) {
		cout << "Connection closed by server." << endl;
		return;
	}
	buffer[bytes_received] = '\0';
	cout << buffer;

	std::vector<std::string> params;
	if (parseParams(buffer, params) < 1)
		return;

	sendMessage(_sockfd, "PASS a\r\n");
	sendMessage(_sockfd, "NICK " + _bot_nick + "\r\n");
	sendMessage(_sockfd, "USER " + _bot_nick + " 0 * :IRC Bot\r\n");
	sendMessage(_sockfd, "JOIN #general\r\n");
	memset(buffer, 0, sizeof(buffer));
	while (true){
		bytes_received = recv(_sockfd, buffer, sizeof(buffer) - 1, 0);
		if (bytes_received < 0) {
			ft_error(errno, "recv failed");
			return;
		} else if (bytes_received == 0) {
			cout << "Connection closed by server." << endl;
			return;
		}
		buffer[bytes_received] = '\0';
		cout << buffer << endl;
	}
}

int main(int ac, char **av) {
	if (ac < 4) {
		std::cerr << "Usage: " << av[0] << " <server_ip> <server_port> <bot_nick>" << std::endl;
		return 1;
	}
	try{
		Bot bot(av[1], atoi(av[2]), av[3]);
		if (bot.connectToServer()){
			// while (true){
			// 	bot.run();
			// }
			bot.run();
		}
		else 
			cout << "Could not connect\n";
	} catch (std::exception &e){
		cout << e.what() << endl;
	}

	return 0;
}