#ifndef Server_HPP
#define Server_HPP

#include <iostream>
#include "ft_irc.h"

int ft_error(int err, const std::string &msg);

class Client;

class Server {
	private:
		sockaddr_in	_serverAddr;
		int			_serverSocket;
		int			_port;
		int			_maxClients;
	public:
		Server();
		Server(int port, int maxClients);
		~Server();

		class ServerFailedException : public std::exception {
		public:
			explicit ServerFailedException(const std::string& msg)
				: message_("Server failed to start => ") {ft_error(errno, msg);}

			virtual const char* what() const throw() {
				return message_.c_str();
			}

			// also must match base dtor: throw()
			virtual ~ServerFailedException() throw() {}

		private:
			std::string message_;
		};

		void startServer(int epoll_fd, std::map<int, Client>& clients);
		int getServerSocket() const { return _serverSocket; }
		int getPort() const { return _port; }

		int initConnection(std::map<int, Client>& clients);
		int setEpoll();
};

#endif