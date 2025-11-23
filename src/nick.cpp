/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:13:57 by zelbassa          #+#    #+#             */
/*   Updated: 2025/11/23 21:39:41 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cctype>
#include <algorithm>
#include <sstream>
#include <string>
#include <map>
#include "ft_irc.h"

Nick::Nick(/* args */)
: Command()
{
}

Nick::~Nick()
{
}

static inline std::string trim(const std::string &s) {
	size_t a = 0;
	while (a < s.size() && isspace((unsigned char)s[a])) ++a;
	size_t b = s.size();
	while (b > a && isspace((unsigned char)s[b-1])) --b;
	return s.substr(a, b - a);
}

static inline std::string toLowerAscii(const std::string &s) {
	std::string r = s;
	for (size_t i = 0; i < r.size(); ++i) r[i] = (char)tolower((unsigned char)r[i]);
	return r;
}

void Nick::execute(Client &client, const std::string &params, const std::string &cmd, std::map<int, Client>& clients, Server& server)
{
	(void)server;
	(void)cmd;
	std::string p = trim(params);
	std::istringstream iss(p);
	std::string new_nick;

	if (client._isAuth == false) {
		client.response(":server 464 * :Password required\r\n");
		return;
	}

	std::string curNick = client.getNickname().empty() ? "*" : client.getNickname();
	if (!(iss >> new_nick)) {
		client.response(":server 431 " + curNick + " :No nickname given\r\n"); 
		return;
	}
	const size_t MAX_NICK = 30;
	if (new_nick.empty() || new_nick.size() > MAX_NICK) {
		client.response(":server 432 " + curNick + " " + new_nick + " :Erroneous nickname\r\n");
		return;
	}

	for (size_t i = 0; i < new_nick.size(); ++i) {
		unsigned char c = (unsigned char)new_nick[i];
		if (i == 0) {
			if (isdigit(c)) {
				client.response(":server 432 " + curNick + " " + new_nick + " :Erroneous nickname\r\n");
				return;
			}
			if (!isalpha(c) && c != '_' && c != '[' && c != ']' && c != '\\' && c != '^' && c != '{' && c != '|' && c != '}') {
				client.response(":server 432 " + curNick + " " + new_nick + " :Erroneous nickname\r\n");
				return;
			}
		} else {
			if (!isalnum(c) && c != '_' && c != '-' && c != '[' && c != ']' && c != '\\' && c != '^' && c != '{' && c != '|' && c != '}') {
				client.response(":server 432 " + curNick + " " + new_nick + " :Erroneous nickname\r\n");
				return;
			}
		}
	}


	std::string key = toLowerAscii(new_nick);
	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
		if (it->first == client.getFd()) continue; // skip self
		if (toLowerAscii(it->second.getNickname()) == key) {
			client.response(":server 433 " + curNick + " " + new_nick + " :Nickname is already in use\r\n");
			return;
		}
	}


	std::map<int, Client>::iterator me = clients.find(client.getFd());
	if (me != clients.end()) {
		me->second.setNickname(new_nick);

	} else {
		
		client.setNickname(new_nick);
	}

}

