/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_help.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 14:59:05 by zelbassa          #+#    #+#             */
/*   Updated: 2025/10/07 14:06:10 by zelbassa         ###   ########.fr       */
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
			if (errno == EINTR)
				continue; // Interrupted by signal, retry
			ft_error(errno, "epoll_wait");
			break;
		}
		
		for (int i = 0; i < ec_; i++) {
			int fd = events[i].data.fd;
			if (fd == getServerSocket()) {
				int cli_fd = initConnection(clients);
				if (cli_fd != -1) {
					add_fd(epoll_fd, cli_fd, EPOLLIN | EPOLLET | EPOLLOUT | EPOLLHUP);
				}
				continue;
			}
			if (events[i].events & EPOLLIN) {
				map<int, Client>::iterator it = clients.find(fd);
				if (it == clients.end()) {
					del_and_close(epoll_fd, fd);
					continue;
				}
				int err = handleCmd(it->second, epoll_fd, clients);
				// if (err == 0){
				// 	continue;
				// }
				if (err == -1) {
					continue; // Changed from break - keep processing other events
				}
				if (err == -2)
				{
					// clients[fd].markDisconnected();
					del_and_close(epoll_fd, fd);
					clients.erase(it);
				}
			}
			if (events[i].events & EPOLLOUT) {
				map<int, Client>::iterator it = clients.find(fd);
				if (it == clients.end()) {
					cerr << "Client fd=" << fd << " not found for EPOLLOUT" << endl;
					del_and_close(epoll_fd, fd);
					continue;
				}
				it->second.sendPendingMessages();
			}
			if ((events[i].events & (EPOLLHUP | EPOLLERR)) || clients[fd].should_quit) {
				cout << "Client disconnected (HUP/ERR): fd=" << fd << endl;
				del_and_close(epoll_fd, fd);
				clients.erase(clients.find(fd));
			}
		}
	}
	close(epoll_fd);
	terminate(clients);
}