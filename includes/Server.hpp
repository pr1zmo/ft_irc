/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:14:23 by zelbassa          #+#    #+#             */
/*   Updated: 2025/10/02 16:31:21 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef Server_HPP
#define Server_HPP

#include "ft_irc.h"

int ft_error(int err, const std::string &msg);

class Client;

class Server {
	private:
		sockaddr_in					_serverAddr;
		int							_serverSocket;
		int							_port;
		int							_maxClients;
		const std::string 		_password;
		bool 							_locked;
	//	std::map<int, Client>	_clients;
	public:
		Server();
		Server(int port, int maxClients, const std::string &password);
		~Server();

		class ServerFailedException : public std::exception {
		public:
			explicit ServerFailedException(const std::string& msg)
				: message_("Server failed to start => ") {ft_error(errno, msg);}

			virtual const char* what() const throw() {
				return message_.c_str();
			}

			// also must match base dtor: throw()
			virtual ~ServerFailedException() throw() {}

		private:
			std::string message_;
		};

		void startServer(int epoll_fd, std::map<int, Client>& clients);
		int getServerSocket() const { return _serverSocket; }
		int getPort() const { return _port; }

		int initConnection(std::map<int, Client>& clients);
		int setEpoll();
		int handleCmd(Client &cli, int epoll_fd, std::map<int, Client>& clients);

		void terminate(std::map<int, Client>& clients);

		void enableWrite(int epoll_fd, int client_fd);
		void disableWrite(int epoll_fd, int client_fd);
};

#endif