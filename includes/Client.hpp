/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:13:20 by zelbassa          #+#    #+#             */
/*   Updated: 2025/11/20 18:59:29 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ft_irc.h"
#include "Channel.hpp"
class Channel;

class Command;

class Client {
	size_t 					_fd;
	struct sockaddr_in	_address;
	socklen_t 				_addrLen;
	Command* 				_cmd;
	// User information
	std::string				_username;
	std::string				_realname;
	std::string				_hostname;
	std::string				_servername;
	std::string		_name;
	std::string		_pass;
	std::string		_nick;
	std::string				_pending_msg;
	bool						_isRegistered;
	// const std::string _cmd;
	public:
		bool			_has_msg;
		std::string	_msgBuffer;
		size_t		last_activity;
		bool			should_quit;
		bool			_isAuth; // Example member to track authentication status
		int			epoll_fd;
		Client();
		Client(int fd, struct sockaddr_in cli_addr, int e_fd);
		Client& operator=(const Client &other);
		~Client();

		int getFd() const { return _fd; }
		Command* getCmd() const { return _cmd; }
		struct sockaddr_in getAddress() const { return _address; }
		socklen_t getAddrLen() const { return _addrLen; }
		void response(const std::string &msg);
		void sendPendingMessages();
		void markDisconnected();

		std::string get_pending_msg();
		void setNickname( std::string &nick);
		std::string getNickname();
		void setUsername(std::string &username) { _username = username; }
		void setRealname( std::string &realname) { _realname = realname; }
		void setHostname( std::string &hostname) { _hostname= hostname; }
		void setServername( std::string &servername) { _servername = servername; }
		std::string getUsername()  { return _username; }
		std::string getRealname()  { return _realname; }
		std::string getHostname()  { return _hostname; }
		std::string getServername() const { return _servername; }
		bool isRegistered() const { return _isRegistered; }
		void registerClient();
		void unregisterClient();
		Channel* getChannel() const;
		void joinChannel(Channel* channel);
		void leaveChannel();
		
};
