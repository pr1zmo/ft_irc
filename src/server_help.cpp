/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_help.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 14:59:05 by zelbassa          #+#    #+#             */
/*   Updated: 2025/10/17 14:00:56 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void static listClients(map<int, Client>& clients) {
	cout << "Current connected clients:" << endl;
	for (map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
		cout << " - FD: " << it->first << ", IP: " << inet_ntoa(it->second.getAddress().sin_addr)
			<< ", Port: " << ntohs(it->second.getAddress().sin_port) << endl;
	}
}

void Server::startServer(int epoll_fd, map<int, Client>& clients) {
	epoll_event events[MAX_EVENTS];

	while (running) {
		int ec_ = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (ec_ == -1) {
			if (errno == EINTR) // Interrupted by signal, retry
				continue;
			ft_error(errno, "epoll_wait");
			break;
		}
		
		for (size_t i = 0; i < ec_; i++) {
			int fd = events[i].data.fd;
			if (fd == getServerSocket()) {
				int cli_fd = initConnection(clients);
				if (cli_fd != -1) {
					add_fd(epoll_fd, cli_fd, EPOLLIN | EPOLLET);
				}
				continue;
			}
			map<int, Client>::iterator cli_it = clients.find(fd);
			if (cli_it == clients.end()) {
				cerr << "Client fd=" << fd << " not found in clients map." << endl;
				del_and_close(epoll_fd, fd);
				continue;
			}
			cout << "\n\033[1;32mevent type: " << events[i].events << "\033[0m\n";
			cout << "EPOLLIN: " << EPOLLIN << "\n";
			if ((events[i].events & (EPOLLHUP | EPOLLERR)) || clients[fd].should_quit) {
				cout << "Client disconnected (HUP/ERR): fd=" << fd << endl;
				del_and_close(epoll_fd, fd);
				clients.erase(clients.find(fd));
				continue;
			}
			if (events[i].events & EPOLLIN) {
				if (cli_it == clients.end()) {
					del_and_close(epoll_fd, fd);
					continue;
				}
				int err = handleCmd(cli_it->second, epoll_fd);
				if (err == -1)
					continue;
				if (err == -2)
					clients[fd].markDisconnected();
			}
			cli_it = clients.find(fd);
			if (cli_it == clients.end()) {
				continue;
			}
			if (events[i].events & EPOLLOUT) {
				cli_it->second.sendPendingMessages();
				
				// If queue is empty, disable EPOLLOUT
				if (!cli_it->second._has_msg) {
					disableWrite(epoll_fd, fd);
					
					// If marked for disconnect and no more messages, disconnect now
					if (cli_it->second.should_quit) {
						cout << "Client quit after sending all messages: fd=" << fd << endl;
						del_and_close(epoll_fd, fd);
						clients.erase(cli_it);
						continue;
					}
				}
			}
		}
	}
	close(epoll_fd);
	terminate(clients);
}