#include "Client.hpp"
#include <iostream>

Client::Client(){
}

Client::Client(int fd, struct sockaddr_in cli_addr)
	: _fd(fd), _address(cli_addr), _addrLen(sizeof(cli_addr)), _isAuth(false), _cmd(NULL) {
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
	// Simple authentication logic for demonstration
	return true; // Always authenticate successfully for now
}
