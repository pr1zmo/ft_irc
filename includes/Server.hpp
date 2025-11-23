/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:14:23 by zelbassa          #+#    #+#             */
/*   Updated: 2025/11/22 20:45:54 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef Server_HPP
#define Server_HPP

#include "ft_irc.h"

class Client;
class Channel;

int ft_error(int err, const std::string &msg);

class Server {
	private:
		sockaddr_in					_serverAddr;
		int							_serverSocket;
		int							_port;
		int							_maxClients;
		const std::string 		_password;
		bool 							_locked;

		std::map<std::string, Channel*> _channels;
	//	std::map<int, Client>	_clients;
	public:
		Server();
		Server(int port, const std::string &password);
		int epoll_fd;
		~Server();

		Channel* getChannel(const std::string& name);
		std::map<std::string, Channel*>& getChannels() { return _channels; }
		std::string getPassword() const { return _password; }
		//add channel to server
		void addChannel(const std::string& name, Channel* channel);
		void removeChannel(const std::string& name);

		class ServerFailedException : public std::exception {
		public:
			ServerFailedException(const std::string& msg) { std::cout << "Server failed to start => ", ft_error(errno, msg); };

			virtual const char* what() const throw() {
				return "";
			}

			virtual ~ServerFailedException() throw() {}
		};
		public:
		void startServer(int epoll_fd, std::map<int, Client>& clients);
		int getServerSocket() const { return _serverSocket; }
		int getPort() const { return _port; }

		int initConnection(std::map<int, Client>& clients);
		int setEpoll();
		int handleCmd(Client &cli, int epoll_fd, std::map<int, Client>& clients, Server& server);

		void terminate(std::map<int, Client>& clients);
		void disableWrite(int epoll_fd, int client_fd);

};

#endif