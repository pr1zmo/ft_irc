/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 16:32:03 by zelbassa          #+#    #+#             */
/*   Updated: 2025/11/06 16:33:42 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.h"

int ft_error(int err, const string &msg)
{
	cerr << "Error: " << msg << ": (" << strerror(err) << ")" << endl;
	return err;
}

void add_fd(int epoll_fd, int fd, uint32_t events) {
	epoll_event ev;
	ev.events = events;
	ev.data.fd = fd;
	
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		ft_error(errno, "epoll_ctl ADD");
	}
}

void del_and_close(int epfd, int fd) {
	epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL); // ignore errors on DEL
	close(fd);
}