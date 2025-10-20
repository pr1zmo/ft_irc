/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:14:16 by zelbassa          #+#    #+#             */
/*   Updated: 2025/10/20 14:27:50 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <iostream>
#include <sys/ioctl.h>

Client::Client(): should_quit(false){
}

Client::Client(int fd, struct sockaddr_in cli_addr)
	: _fd(fd), _address(cli_addr), _addrLen(sizeof(cli_addr)), _pending_msg(""), _has_msg(false), last_activity(time(NULL)), should_quit(false), _isAuth(false), _nick("Guest" + to_string98(fd)){
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
	_pending_msg += msg;
	_has_msg = true;
}

void Client::sendPendingMessages() {
	if (!_has_msg || _pending_msg.empty()) {
		return;
	}
	
	ssize_t sent = send(_fd, _pending_msg.c_str(), _pending_msg.size(), MSG_NOSIGNAL);
	
	if (sent > 0) {
		_pending_msg.erase(0, sent);
		if (_pending_msg.empty()) {
			_has_msg = false;
		}
	} else if (sent < 0) {
		if (errno != EAGAIN && errno != EWOULDBLOCK) {
			std::cerr << "Send error on fd=" << _fd << ": " << strerror(errno) << std::endl;
			_has_msg = false;
			_pending_msg.clear();
		}
	}
}

string Client::get_pending_msg() {
	return this->_pending_msg;
}

void Client::markDisconnected() {
	should_quit = true;
}