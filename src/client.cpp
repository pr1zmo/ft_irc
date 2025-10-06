/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:14:16 by zelbassa          #+#    #+#             */
/*   Updated: 2025/10/06 15:36:54 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <iostream>
#include <sys/ioctl.h>

Client::Client(): should_quit(false){
}

Client::Client(int fd, struct sockaddr_in cli_addr)
	: _fd(fd), _address(cli_addr), _addrLen(sizeof(cli_addr)), _pending_msg(""), _has_msg(false), last_activity(time(NULL)), should_quit(false), _isAuth(false){
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
	queueMessage(msg);
}

void Client::sendPendingMessages() {
	if (_has_msg && !_pending_msg.empty()) {
		send(_fd, _pending_msg.c_str(), _pending_msg.size(), 0);
		// struct winsize w;
		// ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
		
		// string row(w.ws_col, '-');
		// send(_fd, (row + "\r\n").c_str(), row.size() + 2, 0); // Debug line
		_has_msg = false;
		_pending_msg.clear();
	}
}

void Client::queueMessage(const std::string &msg) {
	_pending_msg += msg;
	_has_msg = true;
}

string Client::get_pending_msg() {
	return this->_pending_msg;
}

void Client::markDisconnected() {
	should_quit = true;
}