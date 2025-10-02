/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:13:20 by zelbassa          #+#    #+#             */
/*   Updated: 2025/10/02 16:22:21 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef Client_HPP
#define Client_HPP

#include "ft_irc.h"

class Command;

class Client {
	size_t 					_fd;
	struct sockaddr_in	_address;
	socklen_t 				_addrLen;
	Command* 				_cmd;
	const std::string		_name;
	const std::string		_pass;
	const std::string		_nick;
	std::string				_pending_msg;
	// const std::string _cmd;
	public:
		bool						_has_msg;
		std::string	_msgBuffer;
		size_t		last_activity;
		bool _isAuth; // Example member to track authentication status
		Client();
		Client(int fd, struct sockaddr_in cli_addr);
		Client& operator=(const Client &other);
		~Client();

		int getFd() const { return _fd; }
		struct sockaddr_in getAddress() const { return _address; }
		socklen_t getAddrLen() const { return _addrLen; }
		bool authenticate(const std::string &msg);
		void response(const std::string &msg);
		void sendPendingMessages();
		void queueMessage(const std::string &msg);

		std::string get_pending_msg();
};

#endif