/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:13:32 by zelbassa          #+#    #+#             */
/*   Updated: 2025/10/06 15:06:28 by zelbassa         ###   ########.fr       */
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



/*
 * Accepts a new client connection
 * Sets the new socket to non-blocking mode
 * Adds the new client to the clients map
 * Returns the new client's file descriptor or -1 on error
*/

int Server::initConnection(map<int, Client> &clients){
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
	// if (clients.size() >= static_cast<size_t>(_maxClients)) {
	// 	cerr << "Max clients reached. Rejecting new connection." << endl;
	// 	close(cli_fd);
	// 	return -1;
	// }

	clients[cli_fd] = Client(cli_fd, cli_address);

	string client_info = "Your connection info:\r\n";
	client_info += "\tIP: " + string(inet_ntoa(cli_address.sin_addr)) + "\r\n";
	client_info += "\tPort: " + to_string98(ntohs(cli_address.sin_port)) + "\r\n";
	client_info += "\tFile Descriptor: " + to_string98(cli_fd) + "\r\n";
	clients[cli_fd].response(client_info);
	
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
	int fd = cli.getFd();
	char buffer[BUFFER_SIZE];

	// cout << "[DEBUG] handleCmd: Starting for fd=" << fd << endl;

	// For draining the socket from all available data, read until EAGAIN/EWOULDBLOCK
	for (;;) {
		ssize_t bytesRead = recv(fd, buffer, sizeof(buffer), 0);
		
		if (bytesRead > 0) {
			// cout << "[DEBUG] handleCmd: Read " << bytesRead << " bytes from fd=" << fd << endl;
			// When new data is read, append it to the client's message buffer
			cli._msgBuffer.append(buffer, bytesRead);
			
			// Buffer overflow
			if (cli._msgBuffer.size() > 4096) {
				// cout << "[DEBUG] handleCmd: Buffer overflow for fd=" << fd << ", size=" << cli._msgBuffer.size() << endl;
				string err = "ERROR :Input buffer overflow\r\n";
				cli.response(err);
				return -2;
			}
			continue;
		}
		else if (bytesRead == 0) {
			// cout << "[DEBUG] handleCmd: EOF received from fd=" << fd << " (client sent ^D)" << endl;
			return -2;
		}
		else {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				// cout << "[DEBUG] handleCmd: EAGAIN/EWOULDBLOCK for fd=" << fd << ", breaking recv loop" << endl;
				break;
			}
			if (errno == EINTR) {
				// cout << "[DEBUG] handleCmd: EINTR for fd=" << fd << ", retrying" << endl;
				// Interrupted by signal, retry
				continue;
			}
			// Real error
			// cout << "[DEBUG] handleCmd: recv error for fd=" << fd << ", errno=" << errno << endl;
			ft_error(errno, "recv");
			return -1;
		}
	}

	// cout << "[DEBUG] handleCmd: Finished recv loop for fd=" << fd << ", buffer size=" << cli._msgBuffer.size() << endl;

	// 2. Process complete commands (ending with \r\n)
	Executioner executioner;
	string complete_cmd;
	size_t pos;

	while ((pos = cli._msgBuffer.find("\r\n")) != string::npos) {
		// Extract complete command (without \r\n)
		complete_cmd = cli._msgBuffer.substr(0, pos);
		// Remove processed command from buffer (including \r\n)
		cli._msgBuffer.erase(0, pos + 2);

		// Check command length (IRC RFC limit: 512 bytes including \r\n)
		if (complete_cmd.size() > 510) {
			cli.response("ERROR :Line too long\r\n");
			return -2;
		}

		// Skip empty commands
		if (complete_cmd.empty()) {
			continue;
		}
		cout << "Processing complete command from fd=" << fd << ": " << complete_cmd << endl;

		// Process the command
		int result = executioner.run(cli, complete_cmd);

		if (cli._has_msg){
			enableWrite( epoll_fd, fd);
		}

		if (result == -1) {
			return -1;
		}
	}
	

	// 3. If there's leftover data (incomplete command), keep it for next time
	if (!cli._msgBuffer.empty()) {
		// cout << "Leftover incomplete command in buffer for fd=" << fd << ": " << cli._msgBuffer << endl;
	}
	
	return 0; // Success, client stays connected
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