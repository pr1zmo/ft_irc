/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:13:32 by zelbassa          #+#    #+#             */
/*   Updated: 2025/10/20 16:29:15 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <iostream>

Server::Server()
{
}

Server::Server(int port, int maxClients, const string &password)
	:_port(port), _maxClients(maxClients), _password(password), _locked(false)
{
	if (!_password.empty())
		_locked = true;

	_serverAddr.sin_family = AF_INET;
	_serverAddr.sin_addr.s_addr = INADDR_ANY;
	_serverAddr.sin_port = htons(port);

	if ((_serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 3)
		throw ServerFailedException("socket");

	if (fcntl(_serverSocket, F_SETFL, O_NONBLOCK) < 0)
		throw ServerFailedException("fcntl");

	int opt = 1;
	if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw ServerFailedException("setsockopt SO_REUSEADDR");

	if (bind(_serverSocket, (struct sockaddr *)&_serverAddr, sizeof(_serverAddr)) < 0)
		throw ServerFailedException("bind");

	if (listen(_serverSocket, SOMAXCONN) < 0)
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
 * Notifies when a client sends data or disconnects or is ready to writeMA
 * In this case we only handle read events
*/



/*
 * Accepts a new client connection
 * Sets the new socket to non-blocking mode
 * Adds the new client to the clients map
 * Returns the new client's file descriptor or -1 on error
*/

int Server::initConnection(map<int, Client>& clients) {
	struct sockaddr_in cli_addr;
	socklen_t cli_addr_len = sizeof(cli_addr);
	
	int cli_fd = accept(_serverSocket, (struct sockaddr *)&cli_addr, &cli_addr_len);
	if (cli_fd == -1) {
		ft_error(errno, "accept");
		return -1;
	}
	
	// Make socket non-blocking
	int flags = fcntl(cli_fd, F_GETFL, 0);
	if (flags == -1) {
		close(cli_fd);
		return -1;
	}
	
	if (fcntl(cli_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
		close(cli_fd);
		return -1;
	}
	// if (errno == EAGAIN || errno == EWOULDBLOCK) cout << "WTFFFFFFFFFFFFFFFF THIS WOULD BLOCKKK???\n";
	
	clients[cli_fd] = Client(cli_fd, cli_addr);

	string client_info = "Your connection info:\r\n";
	client_info += "\tIP: " + string(inet_ntoa(cli_addr.sin_addr)) + "\r\n";
	client_info += "\tPort: " + to_string98(ntohs(cli_addr.sin_port)) + "\r\n";
	client_info += "\tFile Descriptor: " + to_string98(cli_fd) + "\r\n";
	clients[cli_fd].response(client_info);
	
	return cli_fd;
}

int Server::setEpoll() {
	int epoll_fd = epoll_create1(0);
	if (epoll_fd == -1) {
		ft_error(errno, "epoll_create1");
		return -1;
	}

	add_fd(epoll_fd, _serverSocket, EPOLLIN | EPOLLET | EPOLLOUT);

	return epoll_fd;
}

int checkCommand(const string &msg, vector<string> validCmds) {
	// Basic validation: command should not be empty and should end with \r\n

	string firstWord;

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
	cerr << "Couldn't find command: " << firstWord << endl;
	cout << "------------------------------" << endl;
	return 0;
}

int Server::handleCmd(Client &cli, int epoll_fd) {
	Executioner executioner;
	int fd = cli.getFd();
	char buffer[BUFFER_SIZE];
	size_t _btsRd = 0;

	for (;;) {
		ssize_t bytesRead = recv(fd, buffer, sizeof(buffer), 0);

		if (bytesRead > 0) {
			_btsRd += bytesRead;
			cli._msgBuffer.append(buffer, bytesRead);

			if (cli._msgBuffer.size() > 4096) {
				string err = "ERROR :Input buffer overflow\r\n";
				cli.response(err);
				cli._msgBuffer.clear();
				cli.markDisconnected();
				return 0;
			}
			else
				cli._msgBuffer.append(buffer, bytesRead);
			continue;
		}
		else if (bytesRead == 0) {
			close(cli.getFd());
			break;
		}
		else {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				break;
			}
			if (errno == EINTR) {
				continue;
			}
			ft_error(errno, "recv");
				return -1;
		}
	}

	string complete_cmd;
	size_t pos;

	while ((pos = cli._msgBuffer.find("\r\n")) != string::npos) {
		complete_cmd = cli._msgBuffer.substr(0, pos);

		cli._msgBuffer.erase(0, pos + 2);

		if (complete_cmd.size() > 510) {
			cli.response("ERROR :Line too long\r\n");
			return -2;
		}

		if (complete_cmd.empty())
			continue;

		int result = executioner.run(cli, complete_cmd);
		if (result == -1){
			
			return -1;
		}

		if (cli._has_msg)
			enableWrite(epoll_fd, fd);
	}
	// leftover data
	if (!cli._msgBuffer.empty()) {
		cout << "Incomplete command in buffer (waiting for \\r\\n): " 
		<< cli._msgBuffer << endl;
	}
	return 0;
}

void Server::terminate(map<int, Client>& clients) {
	cout << "Closing server socket: " << endl;
	close(_serverSocket);
	for (map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
		close(it->first);
	}
	running = false;
}

/*
 * This function enables write events for a client socket in the epoll instance
 * Without it the server won't know when the socket is ready to send data
 * This is important for non-blocking sockets to avoid EAGAIN errors
*/
void Server::enableWrite(int epoll_fd, int client_fd){
	epoll_event ev;
	ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
	ev.data.fd = client_fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client_fd, &ev) == -1) {
		ft_error(errno, "epoll_ctl(MOD) enableWrite");
	}
}

/*
 * This function disables write events for a client socket in the epoll instance
 * This is used when there are no pending messages to send
 * Disabling write events helps reduce unnecessary wake-ups and CPU usage
*/

void Server::disableWrite(int epoll_fd, int client_fd){
	epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = client_fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client_fd, &ev) == -1) {
		ft_error(errno, "epoll_ctl(MOD) disableWrite");
	}
}