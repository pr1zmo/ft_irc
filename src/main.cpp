/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:14:00 by zelbassa          #+#    #+#             */
/*   Updated: 2025/11/20 18:56:15 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netinet/in.h>
#include <sys/ioctl.h>

#include "ft_irc.h"
int running = 1;

void on_signal(int sig) {
	if (sig == SIGINT || sig == SIGTERM || sig == SIGQUIT) {
		running = 0;
	}
}

static void install_handlers() {
	struct sigaction sa;
	std::memset(&sa, 0, sizeof(sa));
	sa.sa_handler = &on_signal;

	sigaction(SIGINT,  &sa, 0); // Handle ^C interrupt signal
	sigaction(SIGTERM, &sa, 0); // Handle ^\ terminate signal
	sigaction(SIGQUIT, &sa, 0); // Handle ^\ quit signal
}

int main(int ac, char *av[]){
	cout << "\n----- FT_IRC SERVER -----\n" << endl;

	install_handlers();

	if (ac != 3) {
		cerr << "Usage: ./ircserv [PORT] [PASSWORD]" << endl;
		return 1;
	}
	try{
		std::map<int, Client> clients;
		Server server(std::atoi(av[1]), std::string(av[2]));
		server.epoll_fd = server.setEpoll();
		server.startServer(server.epoll_fd, clients);
	}
	catch (const std::exception& e) {
		cerr << e.what() << endl;
		return 1;
	}
	return 0;
}