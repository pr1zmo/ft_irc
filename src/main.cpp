#include <netinet/in.h>

#include "ft_irc.h"

int handleCmd(int fd, int server_fd) {

	char buffer[BUFFER_SIZE];
	int bytes_read = recv(fd, buffer, sizeof(buffer), 0);
	if (bytes_read <= 0) {
		if (bytes_read == 0 || errno != EAGAIN) {
			// Connection closed or error
			cout << "Client disconnected: fd=" << fd << endl;
			close(fd);
			return -1;
		}
	} else {
		buffer[bytes_read] = '\0'; // Null-terminate the received data
		cout << "Received from fd=" << fd << ": " << buffer << endl;

		// Echo back the received message (for testing purposes)
		std::string response = "Echo: " + std::string(buffer);
		// send(fd, response.c_str(), response.size(), 0);
	}
	return 0;
}

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
		Server server(std::atoi(av[1]), 10);
		int ep_fd = server.setEpoll();
		server.startServer(ep_fd, clients);
	}
	catch (const std::exception& e) {
		cerr << e.what() << endl;
		return 1;
	}


	return 0;
}