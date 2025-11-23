/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:14:16 by zelbassa          #+#    #+#             */
/*   Updated: 2025/11/22 21:17:39 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <iostream>
#include <sys/ioctl.h>

Client::Client(): should_quit(false){
	_fd = -1;
	_addrLen = 0;
	_isRegistered = false;
	_pending_msg = "";
}

Client::Client(int fd, struct sockaddr_in cli_addr, int e_fd)
	: _fd(fd), _address(cli_addr), _addrLen(sizeof(cli_addr)), _nick("Guest" + to_string98(fd)), _pending_msg(""), _has_msg(false), last_activity(time(NULL)), should_quit(false), _isAuth(false), epoll_fd(e_fd){
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
		epoll_fd = other.epoll_fd;
	}
	return *this;
}

void Client::response(const std::string &msg) {
	_pending_msg += msg;
	_has_msg = true;
	enableWrite(epoll_fd, _fd);
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
// Command* Client::getCmd() const {
// 	return this->_cmd;
// }
void Client::setNickname(const std::string &nick) {
	// Assuming _nick is not const for this operation
	*(const_cast<std::string*>(&_nick)) = nick;
}

const std::string Client::getNickname() const {
	return _nick;
}
void Client::registerClient() {
	_isRegistered = true;
}
void Client::unregisterClient() {
	_isRegistered = false;
}
Channel* Client::getChannel() const {
	// Placeholder implementation
	return NULL;
}

void Client::joinChannel(Channel* channel) {
	// join the channel
	channel->addUser(this->getNickname(), const_cast<Client*>(this));
	
}
void Client::leaveChannel() {	
	// Placeholder implementation
	// Channel* channel = getChannel();
}
// mark the client as disconnected
// void Client::disconnect() {
// 	should_quit = true;
// }
