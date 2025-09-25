/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:14:16 by zelbassa          #+#    #+#             */
/*   Updated: 2025/09/25 10:22:16 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <iostream>

Client::Client(){
}

Client::Client(int fd, struct sockaddr_in cli_addr)
	: _fd(fd), _address(cli_addr), _addrLen(sizeof(cli_addr)), _cmd(NULL), _isAuth(false) {
}

Client::~Client() {
}

Client& Client::operator=(const Client &other) {
	if (this != &other) {
		_fd = other._fd;
		_address = other._address;
		_addrLen = other._addrLen;
		_isAuth = other._isAuth;
		_cmd = other._cmd; // Shallow copy for now...
	}
	return *this;
}

bool Client::authenticate(const std::string &msg) {
	(void)msg;
	// Simple authentication logic for demonstration
	return true; // Always authenticate successfully for now
}

void Client::response(const std::string &msg) {
	send(_fd, msg.c_str(), msg.size(), 0);
	this->_msgBuffer.clear();
}