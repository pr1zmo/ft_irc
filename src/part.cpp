/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:13:46 by zelbassa          #+#    #+#             */
/*   Updated: 2025/11/06 16:30:04 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.h"

Part::Part(/* args */)
: Command()
{
}

Part::~Part()
{
}

static inline std::string trim_ws(const std::string& s) {
	size_t a = 0, b = s.size();
	while (a < b && (s[a] == ' ' || s[a] == '\t' || s[a] == '\r' || s[a] == '\n')) ++a;
	while (b > a && (s[b-1] == ' ' || s[b-1] == '\t' || s[b-1] == '\r' || s[b-1] == '\n')) --b;
	return s.substr(a, b - a);
}

void Part::execute(Client &cli, const std::string& param, const std::string& cmd, std::map<int, Client>& clients, Server& server) {
	(void)cmd;
	(void)clients;

	if (!cli.isRegistered()) {
		cli.response(":server 451 * :You have not registered\r\n");
		return;
	}
	if (param.empty()) {
		cli.response(":server 461 " + cli.getNickname() + " PART :Not enough parameters\r\n"); 
		return;
	}

	std::string channels_part = param;
	std::string message;
	size_t colonPos = param.find(" :");
	if (colonPos != std::string::npos) {
		channels_part = param.substr(0, colonPos);
		message = param.substr(colonPos + 2);
	} else {
		size_t sp = param.find(' ');
		if (sp != std::string::npos) {
			channels_part = trim_ws(param.substr(0, sp));
			message = trim_ws(param.substr(sp + 1));
			if (!message.empty() && message[0] == ':') message = message.substr(1);
		}
	}

	channels_part = trim_ws(channels_part);
	if (channels_part.empty()) {
		cli.response(":server 461 " + cli.getNickname() + " PART :Not enough parameters\r\n");
		return;
	}

	std::stringstream ss(channels_part);
	std::string item;
	while (std::getline(ss, item, ',')) {
		std::string channelName = trim_ws(item);
		if (channelName.empty()) continue;

		if (channelName[0] != '#') {
			cli.response(":server 403 " + cli.getNickname() + " " + channelName + " :No such channel\r\n");
			continue;
		}

		Channel* channel = server.getChannel(channelName);
		if (!channel) {
			cli.response(":server 403 " + cli.getNickname() + " " + channelName + " :No such channel\r\n");
			continue;
		}

		if (!channel->contains(cli.getNickname())) {
			cli.response(":server 442 " + cli.getNickname() + " " + channelName + " :You're not on that channel\r\n"); 
			continue;
		}
		std::string partLine = ":" + cli.getNickname() + "!" + cli.getUsername() + "@" + cli.getHostname()
									+ " PART " + channelName;
		if (!message.empty()) partLine += " :" + message;
		partLine += "\r\n";

		channel->broadcast(cli.getNickname(), partLine, server);

		channel->removeUser(cli.getNickname());

		if (channel->isEmpty()) {
			server.removeChannel(channelName);
		}

		cli.response(partLine);
	}
}