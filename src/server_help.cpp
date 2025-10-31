/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_help.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 14:59:05 by zelbassa          #+#    #+#             */
/*   Updated: 2025/10/31 14:06:54 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "EventHandler.hpp"

// void static listClients(map<int, Client>& clients) {
// 	cout << "Current connected clients:" << endl;
// 	for (map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
// 		cout << " - FD: " << it->first << ", IP: " << inet_ntoa(it->second.getAddress().sin_addr)
// 			<< ", Port: " << ntohs(it->second.getAddress().sin_port) << endl;
// 	}
// }

void EventHandler::handleNewConnection() {
	int cli_fd = _server.initConnection(_clients);
	if (cli_fd != -1) {
		// add_fd(_epoll_fd, cli_fd, EPOLLIN | EPOLLET);

		// Check for queud messages first
		std::map<int, Client>::iterator it = _clients.find(cli_fd);
		if (it != _clients.end() && it->second._has_msg) {
			enableWrite(_epoll_fd, cli_fd);
		}
	}
}

void EventHandler::handleClientRead(int fd) {
	std::map<int, Client>::iterator it = _clients.find(fd);
	if (it == _clients.end()) {
		std::cerr << "Client fd=" << fd << " not found for EPOLLIN" << std::endl;
		cleanupClient(fd);
		return;
	}

	int err = _server.handleCmd(it->second, _epoll_fd, _clients, _server);

	if (err == -2) {
		it->second.markDisconnected();
	}
}

void EventHandler::handleClientWrite(int fd) {
	std::map<int, Client>::iterator it = _clients.find(fd);
	if (it == _clients.end()) {
		std::cerr << "Client fd=" << fd << " not found for EPOLLOUT" << std::endl;
		cleanupClient(fd);
		return;
	}

	it->second.sendPendingMessages();

	// Client will be marked to disconnect in some cases / check if client should quit
	if (it->second.should_quit && !it->second._has_msg) {
		std::cout << "Client quit after sending all messages: fd=" << fd << std::endl;
		cleanupClient(fd);
		return;
	}

	// No more message => Disable epoll
	if (!it->second._has_msg) {
		_server.disableWrite(_epoll_fd, fd);
	}
}

void EventHandler::handleClientDisconnect(int fd, uint32_t events) {
	std::cout << "Client disconnected (";
	if (events & EPOLLHUP) std::cout << "HUP ";
	if (events & EPOLLERR) std::cout << "ERR ";
	std::cout << "): fd=" << fd << std::endl;
	
	cleanupClient(fd);
}

void EventHandler::cleanupClient(int fd) {
	std::map<int, Client>::iterator it = _clients.find(fd);
	if (it != _clients.end()) {
		del_and_close(_epoll_fd, fd);
		_clients.erase(it);
	} else {
		del_and_close(_epoll_fd, fd);
	}
}

void EventHandler::processEvent(const epoll_event& event) {
	int fd = event.data.fd;
	uint32_t events = event.events;

	if (fd == _server.getServerSocket()) {
		handleNewConnection();
		return;
	}

	if (events & (EPOLLHUP | EPOLLERR)) {
		handleClientDisconnect(fd, events);
		return;
	}

	std::map<int, Client>::iterator it = _clients.find(fd);
	if (it != _clients.end() && it->second.should_quit && !it->second._has_msg) {
		cleanupClient(fd);
		return;
	}

	// Handle incoming data
	if (events & EPOLLIN) {
		handleClientRead(fd);
		
		// Re-check if client was marked for disconnect
		it = _clients.find(fd);
		if (it == _clients.end()) {
			return;
		}
	}

	if (events & EPOLLOUT) {
		handleClientWrite(fd);
	}
}

void Server::startServer(int epoll_fd, map<int, Client>& clients) {
	epoll_event events[MAX_EVENTS];
	EventHandler handler(*this, epoll_fd, clients);

	while (running) {
		int event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		
		if (event_count == -1) {
			if (errno == EINTR) {
				continue;
			}
			ft_error(errno, "epoll_wait");
			break;
		}

		for (int i = 0; i < event_count; i++) {
			handler.processEvent(events[i]);
		}
	}

	close(epoll_fd);
	terminate(clients);
}