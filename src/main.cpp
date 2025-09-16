#include <netinet/in.h>

#include "ft_irc.h"

int ft_error(int err, const string &msg)
{
	cerr << "Error: " << msg << ": (" << strerror(err) << ")" << endl;
	return err;
}

int main(int ac, char *av[]){
	cout << "\n----- FT_IRC SERVER -----\n" << endl;

	epoll_event events[MAX_EVENTS];
	sockaddr_in server;
	int sk = -1;

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(ac == 2 ? atoi(av[1]) : DEFAULT_PORT);

	if ((sk = socket(AF_INET, SOCK_STREAM, 0)) < 3)
		return ft_error(errno, "socket");
	cout << "Socket: " << sk << endl;

	if (fcntl(sk, F_SETFD, O_NONBLOCK) < 0){
		return ft_error(errno, "fcntl");
	}

	int opt = 1;
	setsockopt(sk, SOL_SOCKET, SO_REUSEPORT, &opt, opt);
	if (bind(sk, (struct sockaddr *)&server, sizeof(server)) < 0)
		return ft_error(errno, "bind");
	if (listen(sk, 3) < 0)
		return ft_error(errno, "listen");

	while (1)
	{
		int cli_fd;
		sockaddr_in client;
		socklen_t len = sizeof(client);
		if ((cli_fd = accept(sk, (sockaddr *)&client, &len)) < 0)
			return ft_error(errno, "accept");
		else{
			cout << "New connection, socket fd is " << cli_fd << endl;
			char buff[BUFFER_SIZE];
			int bytes = recv(cli_fd, buff, BUFFER_SIZE, 0);
			cout << buff << "\n";
		}
	}
	return 0;
}