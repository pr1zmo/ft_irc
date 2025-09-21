/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:14:00 by zelbassa          #+#    #+#             */
/*   Updated: 2025/09/21 20:14:01 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netinet/in.h>

#include "ft_irc.h"

int ft_error(int err, const string &msg)
{
	cerr << "Error: " << msg << ": (" << strerror(err) << ")" << endl;
	return err;
}

void add_fd(int epfd, int fd, uint32_t events) {
	struct epoll_event ev = {0};
	ev.events = events;
	ev.data.fd = fd;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		ft_error(errno, "epoll_ctl(ADD)");
		close(fd);
	}
}

void mod_fd(int epfd, int fd, uint32_t events) {
	struct epoll_event ev = {0};
	ev.events = events;
	ev.data.fd = fd;
	if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) == -1) {
		perror("epoll_ctl(MOD)");
	}
}

void del_and_close(int epfd, int fd) {
	epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL); // ignore errors on DEL
	close(fd);
}

int main(int ac, char *av[]){
	cout << "\n----- FT_IRC SERVER -----\n" << endl;

	if (ac != 3) {
		cerr << "Usage: ./ircserv [PORT] [PASSWORD]" << endl;
		return 1;
	}
	
	try{
		std::map<int, Client> clients;
		Server server(std::atoi(av[1]), 10, std::string(av[2]));
		int ep_fd = server.setEpoll();
		server.startServer(ep_fd, clients);
	}
	catch (const std::exception& e) {
		cerr << e.what() << endl;
		return 1;
	}


	return 0;
}