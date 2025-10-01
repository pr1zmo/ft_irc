/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:13:38 by zelbassa          #+#    #+#             */
/*   Updated: 2025/10/01 14:47:37 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.h"

Privmsg::Privmsg(/* args */)
: Command()
{
}

Privmsg::~Privmsg()
{
}

void Privmsg::execute(Client &cli, const std::string& param){
	(void)cli;
	(void)param;
	// cout << "PRIVMSG command executed with param: " << param << endl;
}

