#ifndef Client_HPP
#define Client_HPP

#include "ft_irc.h"

class Client {
	size_t _fd;
	struct sockaddr_in _address;
	socklen_t _addrLen;
	public:
		Client();
		Client(int fd, struct sockaddr_in cli_addr);
		~Client();

		int getFd() const { return _fd; }
		struct sockaddr_in getAddress() const { return _address; }
		socklen_t getAddrLen() const { return _addrLen; }

};

#endif