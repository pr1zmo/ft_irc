#pragma once

#include "ft_irc.h"

class Server; // Forward declaration

class EventHandler {
private:
	Server& _server;
	int _epoll_fd;
	std::map<int, Client>& _clients;

	void handleNewConnection();
	void handleClientRead(int fd);
	void handleClientWrite(int fd);
	void handleClientDisconnect(int fd, uint32_t events);
	void cleanupClient(int fd);

public:
	EventHandler(Server& server, int epoll_fd, std::map<int, Client>& clients) : _server(server), _epoll_fd(epoll_fd), _clients(clients){};
	~EventHandler() {};

	void processEvent(const epoll_event& event);
};
