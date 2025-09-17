#include <netinet/in.h>

#include "ft_irc.h"

int handleCmd(int fd){
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
		// Process the received data
		cout << "Received data from fd=" << fd << ": ";
		cout.write(buffer, bytes_read);
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
		Server server;
		epoll_event events[MAX_EVENTS];
		std::map<int, Client> clients;
		server.startServer(DEFAULT_PORT);
		int ep_fd = server.setEpoll();
		for (;;)
		{
			int ec_ = epoll_wait(ep_fd, events, MAX_EVENTS, -1);
			if (ec_ == -1) {
				if (errno == EINTR)
					continue; // Interrupted by signal, retry
				ft_error(errno, "epoll_wait");
				break;
			}

			for (int i = 0; i < ec_; i++) {
				int fd = events[i].data.fd;
				if (fd == server.getServerSocket()) {
					int cli_fd = server.initConnection(clients);
					if (cli_fd != -1) {
						add_fd(ep_fd, cli_fd, EPOLLIN | EPOLLET);
					}
				} else {
					if (handleCmd(fd) < 0)
						clients.erase(fd);
				}
			}
		}
	}
	catch (const std::exception& e) {
		cerr << e.what() << endl;
		return 1;
	}


	return 0;
}