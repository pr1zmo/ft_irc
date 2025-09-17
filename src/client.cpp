#include "Client.hpp"
#include <iostream>

Client::Client()
	: _fd(-1), _address(), _addrLen(0) {
}

Client::Client(int fd, struct sockaddr_in cli_addr)
	: _fd(fd), _address(cli_addr), _addrLen(sizeof(cli_addr)){
}

Client::~Client() {
}
