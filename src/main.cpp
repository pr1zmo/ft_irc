/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:14:00 by zelbassa          #+#    #+#             */
/*   Updated: 2025/11/24 13:14:46 by zelbassa         ###   ########.fr       */
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

	sigaction(SIGINT,  &sa, 0); 
	sigaction(SIGTERM, &sa, 0);
	sigaction(SIGQUIT, &sa, 0);
}

int main(int ac, char *av[]){
	
	install_handlers();
	
	if (ac != 3) {
		cerr << "Usage: ./ircserv [PORT] [PASSWORD]" << endl;
		return 1;
	}
	
    int port;
	
    if (!isValidPort(av[1], port)) {
		cerr << "Error: invalid port number." << endl;
        return 1;
    }

	if (port < 1024) {
		cerr << "Error: ports below 1024 require root privileges." << endl;
		return 1;
	}

	if (!isValidPassword(std::string(av[2]))) {
		cerr << "Error: invalid server password." << endl;
		return 1;
    }
	
	cout << "\n----- FT_IRC SERVER -----\n" << endl;
    cout << "Starting server on port: " << port << endl;

	try {
		std::map<int, Client> clients;
		Server server(port, std::string(av[2]));
		server.epoll_fd = server.setEpoll();
		server.startServer(server.epoll_fd, clients);
	}
	catch (const std::exception& e) {
		cerr << e.what() << endl;
		return 1;
	}
	return 0;
}