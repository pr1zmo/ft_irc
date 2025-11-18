/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCbot.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 09:43:28 by zelbassa          #+#    #+#             */
/*   Updated: 2025/11/17 15:25:49 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

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
#include <map>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include "BotConfig.hpp"

using namespace std;

class BotConf;

class Bot {
	private:
	struct sockaddr_in _server_addr;
		int		_sockfd;
		string	_server_pass;
		string	_bot_nick;
		size_t	startTime;

	public:
		Bot(){};
		Bot(BotConf &conf);
		~Bot();
		int serv_fd;
		int getSockfd() { return _sockfd; };
		bool connectToServer();
		vector<string> bot_params;
		int parseParams(char *msg, std::vector<std::string>& params);

		class ConfigFileException : public exception{
			public:
				virtual const char* what() const throw();
		};

		string getUpTime() const;
		int sendMessage(const string &str);

		void handle_response(const string &response, BotConf &conf);
		void handle_privmsg(const string &message, BotConf &conf, const string &sender);
		void handle_system(const string &message, BotConf &conf, const string &sender);

		void join_channels(BotConf &conf);

		void run(BotConf &conf);
		void quit();
};