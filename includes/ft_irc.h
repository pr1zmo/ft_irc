/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:14:27 by zelbassa          #+#    #+#             */
/*   Updated: 2025/09/27 18:05:20 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ft_irc_h
# define ft_irc_h
# include <iostream>
# include <string>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <cstring>
# include <cstdlib>
# include <vector>
# include <map>
# include <sstream>
# include <sys/epoll.h>
# include <fcntl.h>
# include <ctime>
# include <algorithm>
# include <cctype>
# include <csignal>
# include <iomanip>
# include <fstream>
# include <cerrno>
# include <istream>
# include <sstream>
# include <exception>
# include "Server.hpp"
# include "Client.hpp"
# include "Commands.hpp"
# include "Executioner.hpp"

# define MAX_CLIENTS 1022
# define MAX_EVENTS 128
# define BUFFER_SIZE 512
# define DEFAULT_PORT 6667

extern bool running;

using namespace std;

int ft_error(int err, const string &msg);
void add_fd(int epfd, int fd, uint32_t events);
void mod_fd(int epfd, int fd, uint32_t events);
void del_and_close(int epfd, int fd);
// int handleCmd(int server_fd, Client &cli);

#endif