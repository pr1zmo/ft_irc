/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:13:38 by zelbassa          #+#    #+#             */
/*   Updated: 2025/10/28 21:04:51 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.h"
#include <sstream>
#include <vector>
#include <cstdlib> // for strtoul

static std::string trim_str(const std::string &s) {
	size_t l = s.find_first_not_of(' ');
	if (l == std::string::npos) return std::string();
	size_t r = s.find_last_not_of(' ');
	return s.substr(l, r - l + 1);
}

Privmsg::Privmsg(/* args */)
: Command()
{
}

Privmsg::~Privmsg()
{
}

// void Privmsg::handleFileTransfer(Client &cli, const std::string& param){

// }

void Privmsg::execute(Client &cli, const std::string& param, const std::string& cmd, std::map<int, Client>& clients, Server& server) {
	(void)cmd;
	// control-A sentinel used for DCC detection
	std::string ctrl(1, '\001');
	
	if (param.empty()) {
		cli.response("ERROR :No recipient given\r\n");
		return;
	}

	size_t colonPos = param.find(" :");
	if (colonPos == std::string::npos) {
		cli.response("ERROR :No message given\r\n");
		return;
	}

	std::string targets_str = trim_str(param.substr(0, colonPos));
	std::string message = param.substr(colonPos + 2); // Skip " :"

	if (targets_str.empty() || message.empty()) {
		cli.response("ERROR :Invalid PRIVMSG format\r\n");
		return;
	}

	// Split comma-separated targets
	std::vector<std::string> targets;
	std::stringstream ss(targets_str);
	std::string item;
	while (std::getline(ss, item, ',')) {
		std::string t = trim_str(item);
		if (!t.empty()) targets.push_back(t);
	}

	for (size_t ti = 0; ti < targets.size(); ++ti) {
		const std::string target = targets[ti];
		bool isChannel = !target.empty() && (target[0] == '#' || target[0] == '&' || target[0] == '+' || target[0] == '!');

		if (isChannel) {
			Channel* channel = server.getChannel(target);
			if (!channel) {
				cli.response(":server 403 " + cli.getNickname() + " " + target + " :No such channel\r\n");
				continue;
			}
			if (!channel->contains(cli.getNickname())) {
				cli.response(":server 404 " + cli.getNickname() + " " + target + " :Cannot send to channel\r\n");
				continue;
			}
			channel->broadcast(cli.getNickname(), ":" + cli.getNickname() + " PRIVMSG " + target + " :" + message + "\r\n", server);
		} else {
			bool userFound = false;
			for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
				if (it->second.getNickname() == target) {
					it->second.response(":" + cli.getNickname() + " PRIVMSG " + target + " :" + message + "\r\n");
					it->second.sendPendingMessages();
					userFound = true;
					break;
				}
			}
			if (!userFound) {
				cli.response(":server 401 " + cli.getNickname() + " " + target + " :No such nick/channel\r\n");
			}
			// check for DCC in the message payload (use ctrl char)
			// if (message.find(ctrl + "DCC SEND") != std::string::npos) {
			//     handleFileTransfer(cli, message);
			//     return;
			// }
		}
	}

	std::cout << "PRIVMSG executed: targets=[" << targets_str << "] message=\"" << message << "\"\n";
}

