#include "Server.hpp"
#include <iostream>

Server::Server()
	: _serverSocket(-1), _port(0), _maxClients(0), _serverAddr()
{
}

Server::Server(int port, int maxClients)
	: _serverSocket(-1), _port(port), _maxClients(maxClients), _serverAddr()
{
	_serverAddr.sin_family = AF_INET;
	_serverAddr.sin_addr.s_addr = INADDR_ANY;
	_serverAddr.sin_port = htons(port);

	if ((_serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 3)
		throw ServerFailedException("socket");
	std::cout << "Socket: " << _serverSocket << std::endl;

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

Server::~Server() {
}

void Server::startServer(int epoll_fd, std::map<int, Client>& clients) {
	epoll_event events[MAX_EVENTS];

	for (;;)
	{
		int ec_ = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (ec_ == -1) {
			if (errno == EINTR)
				continue; // Interrupted by signal, retry
			ft_error(errno, "epoll_wait");
			break;
		}

		for (int i = 0; i < ec_; i++) {
			int fd = events[i].data.fd;
			int cli_fd;
			if (fd == getServerSocket()) {
				cli_fd = initConnection(clients);
				if (cli_fd != -1) {
					add_fd(epoll_fd, cli_fd, EPOLLIN | EPOLLET);
				}
			} else if (events[i].events & EPOLLIN) {
				if (handleCmd(fd, this->_serverSocket) < 0) {
					del_and_close(epoll_fd, fd);
					clients.erase(clients.find(fd));
				}
			}
			else if (events[i].events & EPOLLOUT) {
				// Handle writable event if needed
			}
			else {
				// error condition
				del_and_close(epoll_fd, fd);
				clients.erase(clients.find(fd));
			}
		}
	}
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