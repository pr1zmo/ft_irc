#include "Server.hpp"
#include <iostream>

Server::Server()
	: _serverSocket(-1), _port(0), _maxClients(0), _serverAddr(), _password("")
{
}

Server::Server(int port, int maxClients, const std::string &password)
	: _serverSocket(-1), _port(port), _maxClients(maxClients), _serverAddr(), _password(password)
{
	if (!_password.empty()) {
		_locked = true;
	}

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
				if (handleCmd(clients[fd]) < 0) {
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

int checkCommand(const std::string &msg) {
	Command command;
	// Basic validation: command should not be empty and should end with \r\n

	if (msg.empty() || msg.find("\r\n") == std::string::npos) {
		return 0; // Invalid format
	}

	std::string firstWord = msg.substr(0, msg.find(' '));

	for (size_t i = 0; i < command.validCmds.size(); ++i) {
		if (firstWord == command.validCmds[i]) {
			return 1; // Valid command
		}
	}
	return 0;
}

int Server::handleCmd(Client &cli) {
	int fd = cli.getFd();
	char buffer[BUFFER_SIZE];
	int bytes_read = recv(fd, buffer, sizeof(buffer), 0);
	if (bytes_read <= 0) {
		if (bytes_read == 0 || errno != EAGAIN) {
			cout << "Client disconnected: fd=" << fd << endl;
			close(fd);
			return -1;
		}
	} else {
		buffer[bytes_read] = '\0';
		int tries = 0;
		std::string msg(buffer);
		cout << "Client: " << fd;

		if (!checkCommand(msg)){
			std::string err = "Invalid command format.\r\n";
			send(fd, err.c_str(), err.size(), 0);
			return 0;
		}

		while (_locked && tries < 3){
			msg = recv(fd, buffer, sizeof(buffer), 0);
			if (msg.empty()) {
				cout << "Client disconnected during auth: fd=" << fd << endl;
				close(fd);
				return -1;
			}
			std::string pass = msg.substr(0, msg.find("\r\n"));
			if (pass == _password) {
				cli._isAuth = true;
				std::string welcome = "Welcome to the IRC server!\r\n";
				send(fd, welcome.c_str(), welcome.size(), 0);
				break;
			} else {
				std::string deny = "Authentication failed. Try again.\r\n";
				send(fd, deny.c_str(), deny.size(), 0);
				tries++;
				continue;
			}
		}
		if (_locked && !cli._isAuth) {
			std::string lock_msg = "Server is locked. Disconnecting.\r\n";
			send(fd, lock_msg.c_str(), lock_msg.size(), 0);
			close(fd);
			return -1;
		}
		std::string response = "Echo: " + std::string(buffer);
		send(fd, response.c_str(), response.size(), 0);
	}
	return 0;
}