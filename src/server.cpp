#include "Server.hpp"
#include <iostream>

Server::Server()
	: _serverSocket(-1), _port(DEFAULT_PORT), _maxClients(MAX_CLIENTS), _serverAddr()
{
}

Server::Server(const Server& other)
	// : _target(other._target)
{
	*this = other;
}

Server& Server::operator=(const Server& other) {
	// if (this != &other) {
	// 	this->type = other.type;
	// 	this->_target = other._target;
	// }
	return *this;
}

Server::~Server() {
}

void Server::startServer(int port) {

	_serverAddr.sin_family = AF_INET;
	_serverAddr.sin_addr.s_addr = INADDR_ANY;
	_serverAddr.sin_port = htons(port);

	if ((_serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 3)
		throw ServerFailedException("socket");
	cout << "Socket: " << _serverSocket << endl;

	if (fcntl(_serverSocket, F_SETFD, O_NONBLOCK) < 0){
		throw ServerFailedException("fcntl");
	}

	int opt = 1;
	setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEPORT, &opt, opt);
	if (bind(_serverSocket, (struct sockaddr *)&_serverAddr, sizeof(_serverAddr)) < 0)
		throw ServerFailedException("bind");
	if (listen(_serverSocket, 3) < 0)
		throw ServerFailedException("listen");
}

int Server::initConnection(std::map<int, Client> &clients){
	struct sockaddr_in cli_address = {};
	socklen_t addrLen = sizeof(cli_address);
	memset(&cli_address, 0, sizeof(cli_address));
	
	int cli_fd = accept(_serverSocket, reinterpret_cast<struct sockaddr*>(&cli_address), &addrLen);
	
	if (cli_fd < 0){
		// Don't print errors for EAGAIN - it's normal
		if (errno == EWOULDBLOCK || errno == EAGAIN)
			return -1;
		if (errno == EINTR)
			return -1;  // Caller should retry
		// Print errors for real problems
		ft_error(errno, "accept");
		return -1;
	}

	fcntl(cli_fd, F_SETFD, O_NONBLOCK);
	if (clients.size() >= static_cast<size_t>(_maxClients)) {
		std::cerr << "Max clients reached. Rejecting new connection." << std::endl;
		close(cli_fd);
		return -1;
	}

	// Use cli_fd as key instead of cli_count
	clients[cli_fd] = Client(cli_fd, cli_address);
	cout << "New client connected: fd=" << cli_fd << endl;
	return cli_fd;
}

int Server::setEpoll() {
	int epoll_fd = epoll_create1(0);
	if (epoll_fd == -1) {
		ft_error(errno, "epoll_create1");
		return -1;
	}

	add_fd(epoll_fd, _serverSocket, EPOLLIN);

	return epoll_fd;
}