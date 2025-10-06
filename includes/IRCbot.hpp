/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCbot.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 09:43:28 by zelbassa          #+#    #+#             */
/*   Updated: 2025/10/06 14:26:47 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCBOT_HPP
#define IRCBOT_HPP

#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

using namespace std;

class Bot {
	private:
		int _sockfd;
		struct sockaddr_in _server_addr;
		std::string _bot_nick;
		fd_set _read_fds;
		// Command _commandHandler;

	public:
		Bot(const std::string& server_ip, int server_port, const std::string& bot_nick);
		~Bot();
		size_t serv_fd;
		int getSockfd() { return _sockfd; };
		bool connectToServer();
		void run();
		vector<string> bot_params;
		int parseParams(char *msg, std::vector<std::string>& params);
};

#endif