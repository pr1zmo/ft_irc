/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:13:24 by zelbassa          #+#    #+#             */
/*   Updated: 2025/11/19 16:53:33 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.h"
#include "IRCbot.hpp"

void show_help(){
	ifstream file("src/help.md");
	if (!file.is_open()) {
		cerr << "Failed to open the file." << endl;
		return ;
	}
	string line;
	while (getline(file, line)) {
		cout << line << endl;
	}
	file.close();
}

string to_string98(int n) {
	ostringstream oss;
	oss << n;
	return oss.str();
}

vector<string> split(const string &s, char delimiter) {
	vector<string> parts;
	string part;
	istringstream ss(s);
	while (getline(ss, part, delimiter)) {
		parts.push_back(part);
	}
	return parts;
}

void enableWrite(int epoll_fd, int cli_fd){
	epoll_event ev;
	ev.events = EPOLLIN | EPOLLOUT;
	ev.data.fd = cli_fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, cli_fd, &ev) == -1) {
		ft_error(errno, "epoll_ctl(MOD) enableWrite");
	}
}
