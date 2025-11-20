/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:13:32 by zelbassa          #+#    #+#             */
/*   Updated: 2025/11/20 17:13:44 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include <iostream>

Server::Server()
{
}

Server::Server(int port, const string &password)
	:_port(port), _password(password), _locked(false)
{
	if (!_password.empty())
		_locked = true;

	_serverAddr.sin_family = AF_INET;
	_serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	_serverAddr.sin_port = htons(port);

	if ((_serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw ServerFailedException("socket");

	if (fcntl(_serverSocket, F_SETFL, O_NONBLOCK) == -1) {
		throw ServerFailedException("fcntl");
	}

	int opt = 1;
	if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw ServerFailedException("setsockopt SO_REUSEADDR");

	if (bind(_serverSocket, (struct sockaddr *)&_serverAddr, sizeof(_serverAddr)) < 0)
		throw ServerFailedException("bind");

	if (listen(_serverSocket, SOMAXCONN) < 0)
		throw ServerFailedException("listen");
}

Server::~Server() {
	for (map<string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); it++)
		delete it->second;
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
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return -1;
		ft_error(errno, "accept");
		return -1;
	}

	if (fcntl(cli_fd, F_SETFL, O_NONBLOCK) == -1) {
		close(cli_fd);
		return -1;
	}

	clients[cli_fd] = Client(cli_fd, cli_addr, epoll_fd);

	add_fd(epoll_fd, cli_fd, EPOLLIN);

	string client_info = "Your connection info:\r\n";
	client_info += "\tIP: " + string(inet_ntoa(cli_addr.sin_addr)) + "\r\n";
	client_info += "\tPort: " + to_string98(ntohs(cli_addr.sin_port)) + "\r\n";
	client_info += "\tFile Descriptor: " + to_string98(cli_fd) + "\r\n";
	clients[cli_fd].response(client_info);
	enableWrite(epoll_fd, cli_fd);

	return cli_fd;
}

int Server::setEpoll() {
	int epoll_fd = epoll_create1(0);
	if (epoll_fd == -1)
		throw ServerFailedException("epoll_create1");

	add_fd(epoll_fd, _serverSocket, EPOLLIN);

	return epoll_fd;
}

int Server::handleCmd(Client &cli, int epoll_fd, map<int, Client>& clients, Server& server) {
	(void)epoll_fd;
	int fd = cli.getFd();
	char buffer[BUFFER_SIZE];
	size_t _btsRd = 0;

	for (;;) {
		ssize_t bytesRead = recv(fd, buffer, sizeof(buffer), 0);

		if (bytesRead > 0) {
			_btsRd += bytesRead;
			cli._msgBuffer.append(buffer, bytesRead);

			if (cli._msgBuffer.size() > 4096) {
				string err = "ERROR :Exceeded the character length limit\r\n";
				cli.response(err);
				cli._msgBuffer.clear();
				cli.markDisconnected();
				return 0;
			}
			continue;
		}
		else if (bytesRead == 0) {
			cli.markDisconnected();
			return 0;
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

	Executioner executioner;
	string complete_cmd;
	size_t pos;

	while ((pos = cli._msgBuffer.find("\r\n")) != string::npos || 
			(pos = cli._msgBuffer.find("\n")) != string::npos) {
		complete_cmd = cli._msgBuffer.substr(0, pos);

		if (cli._msgBuffer[pos] == '\r' && pos + 1 < cli._msgBuffer.size() && cli._msgBuffer[pos + 1] == '\n')
			cli._msgBuffer.erase(0, pos + 2);
		else
			cli._msgBuffer.erase(0, pos + 1);

		if (complete_cmd.size() > 510)
			return (cli.response("ERROR :Line too long\r\n"), -2);

		if (complete_cmd.empty())
			continue;

		int result = executioner.run(cli, complete_cmd, clients, server);
		if (result == -1) {
			return -1;
		}
	}

	return 0;
}

void Server::terminate(map<int, Client>& clients) {
	cout << "Closing server socket: " << endl;
	close(_serverSocket);
	for (map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
		close(it->first);
	}
}

void Server::disableWrite(int epoll_fd, int client_fd){
	epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = client_fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client_fd, &ev) == -1) {
		ft_error(errno, "epoll_ctl(MOD) disableWrite");
	}
}

Channel* Server::getChannel(const std::string& name) {
	std::map<std::string, Channel*>::iterator it = _channels.find(name);
	if (it == _channels.end())
		return NULL;
	return it->second;
}

void Server::addChannel(const std::string& name, Channel* channel) {
	if (name.empty() || channel == NULL) return;
	_channels[name] = channel;
}

void Server::removeChannel(const std::string& name) {
	std::map<std::string, Channel*>::iterator it = _channels.find(name);
	if (it != _channels.end()) {
		delete it->second; 
		_channels.erase(it);
	}
}
