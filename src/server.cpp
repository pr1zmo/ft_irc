/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:13:32 by zelbassa          #+#    #+#             */
/*   Updated: 2025/09/26 14:29:19 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <iostream>

Server::Server()
{
}

Server::Server(int port, int maxClients, const std::string &password)
	:_port(port), _maxClients(maxClients), _password(password), _locked(false)
{
	if (!_password.empty())
		_locked = true;

	_serverAddr.sin_family = AF_INET;
	_serverAddr.sin_addr.s_addr = INADDR_ANY;
	_serverAddr.sin_port = htons(port);

	if ((_serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 3)
		throw ServerFailedException("socket");
	std::cout << "Socket: " << _serverSocket << std::endl;

	if (fcntl(_serverSocket, F_SETFL, O_NONBLOCK) < 0)
		throw ServerFailedException("fcntl");

	int opt = 1;
	if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw ServerFailedException("setsockopt SO_REUSEADDR");

	if (bind(_serverSocket, (struct sockaddr *)&_serverAddr, sizeof(_serverAddr)) < 0)
		throw ServerFailedException("bind");

	if (listen(_serverSocket, 3) < 0)
		throw ServerFailedException("listen");
}

Server::~Server() {
}

/*
 * Main server loop using epoll to handle multiple clients
 * Epoll is set to edge-triggered mode for efficiency
 * Handles new connections and incoming data
 * Adds file descriptors to epoll instance
 * and then waits for events
 * Notifies when a client sends data or disconnects or is ready to write
 * In this case we only handle read events
*/

void Server::startServer(int epoll_fd, std::map<int, Client>& clients) {
	epoll_event events[MAX_EVENTS];

	while (running)
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
				// while(true){
					int err = handleCmd(clients[fd]);
					if (err == -1) {
						del_and_close(epoll_fd, fd);
						clients.erase(clients.find(fd));
						break;
					}
					else if (err == 0) {
						// No complete command yet, wait for more data
						continue;
					}
					// break;
				// }
			}
			else if (events[i].events & EPOLLOUT) {
				// handle write: dummy for now
				cout << "Ready to write to client: fd=" << fd << endl;
			}
			else {
				// error condition
				del_and_close(epoll_fd, fd);
				clients.erase(clients.find(fd));
				cout << "Epoll error on fd=" << fd << endl;
			}
		}
	}
	close(epoll_fd);
	terminate(clients);
}

/*
 * Accepts a new client connection
 * Sets the new socket to non-blocking mode
 * Adds the new client to the clients map
 * Returns the new client's file descriptor or -1 on error
*/

int Server::initConnection(std::map<int, Client> &clients){
	struct sockaddr_in cli_address = {};
	socklen_t addrLen = sizeof(cli_address);
	memset(&cli_address, 0, sizeof(cli_address));
	
	int cli_fd = accept(_serverSocket, reinterpret_cast<struct sockaddr*>(&cli_address), &addrLen);
	
	if (cli_fd < 0){
		// EAGAIN and EWOULDBLOCK just means we have processed all incoming connections
		if (errno == EWOULDBLOCK || errno == EAGAIN)
			return -1;
		// EINTR means the call was interrupted by a signal before a valid connection arrived
		if (errno == EINTR)
			return -1;
		ft_error(errno, "accept");
		return -1;
	}

	fcntl(cli_fd, F_SETFL, O_NONBLOCK);
	if (clients.size() >= static_cast<size_t>(_maxClients)) {
		std::cerr << "Max clients reached. Rejecting new connection." << std::endl;
		close(cli_fd);
		return -1;
	}

	clients[cli_fd] = Client(cli_fd, cli_address);
	return cli_fd;
}

/*
 * Sets up the epoll instance and adds the server socket to it
 * Returns the epoll file descriptor or -1 on error
*/

int Server::setEpoll() {
	int epoll_fd = epoll_create1(0);
	if (epoll_fd == -1) {
		ft_error(errno, "epoll_create1");
		return -1;
	}

	add_fd(epoll_fd, _serverSocket, EPOLLIN);

	return epoll_fd;
}

int checkCommand(const std::string &msg, std::vector<std::string> validCmds) {
	// Basic validation: command should not be empty and should end with \r\n

	// if (msg.empty() || msg.find("\n") == std::string::npos) {
	// 	std::cerr << "message empty or no \\n" << std::endl;
	// 	return 0;
	// }

	std::string firstWord;

	for (int i = 0; i < static_cast<int>(msg.size()); i++) {
		if (msg[i] == ' ' || msg[i] == '\r' || msg[i] == '\n') {
			break;
		}
		firstWord += msg[i];
	}

	for (size_t i = 0; i < validCmds.size(); ++i) {
		if (firstWord == validCmds[i])
			return 1;
	}
	std::cerr << "Couldn't find command: " << firstWord << std::endl;
	std::cout << "------------------------------" << std::endl;
	return 0;
}

/*
void static printBuffer(const std::string& label, const char* buffer, int size) {
	std::cout << label << " (size=" << size << "): ";
	for (int i = 0; i < size; i++) {
		unsigned char c = static_cast<unsigned char>(buffer[i]);
		if (c == '\r') {
			std::cout << "\\r";
		} else if (c == '\n') {
			std::cout << "\\n";
		} else if (c == '\t') {
			std::cout << "\\t";
		} else if (c < 32 || c > 126) {
			// Non-printable characters as hex
			std::cout << "\\x" << std::hex << std::setw(2) << std::setfill('0') << (int)c << std::dec;
		} else {
			std::cout << c;
		}
	}
	std::cout << std::endl;
}
*/

int Server::handleCmd(Client &cli) {
	Command command;
	int fd = cli.getFd();
	char buffer[BUFFER_SIZE];
	int bytes_read = recv(fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes_read <= 0) {
		// EAGAIN means no more data to read
		if (errno != EAGAIN && errno != EWOULDBLOCK) {
			cout << "Client disconnected: fd=" << fd << endl;
			close(fd);
			return -1;
		}
		return 0;
	} else {
		buffer[bytes_read] = '\0';

		cli._msgBuffer += string(buffer, bytes_read);
		cli.last_activity = time(NULL);

		if (cli._msgBuffer.size() > BUFFER_SIZE) {
			cli.response("Error: Message too long.\r\n");
			close(fd);
			return -1;
		}

		size_t pos;
		while ((pos = cli._msgBuffer.find("\r\n")) != std::string::npos) {
			std::string single_cmd = cli._msgBuffer.substr(0, pos + 2);
			cli._msgBuffer.erase(0, pos + 2);
			
			
			if (!checkCommand(single_cmd, command.validCmds)) {
				cli.response("Error : Invalid command.\r\n");
				continue;
			}
		}
	}

	command.execute(cli);

	return 1;
}

void Server::terminate(std::map<int, Client>& clients) {
	std::cout << "Closing server socket: " << std::endl;
	close(_serverSocket);
	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
		close(it->first);
	}
	running = false;
}