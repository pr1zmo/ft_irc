#include "Commands.hpp"
#include <cctype>
#include <algorithm>
#include <sstream>
#include <string>
#include <map>
#include "ft_irc.h"
#include "Server.hpp"
#include "Channel.hpp"


Kick::Kick(/* args */)
: Command()
{
}
Kick::~Kick()
{
}

void Kick::execute(Client &cli, const std::string& param, const std::string& cmd, std::map<int, Client>& clients, Server& server) {
	std::istringstream iss(param);
	std::string channelName;
	std::string target_nick;
	std::string reason;

	if (!cli.isRegistered()) {
		cli.response(":server 451 * :You have not registered\r\n");
		return;
	}
	if (!(iss >> channelName >> target_nick)) {
		cli.response("Usage: " + cmd + " #channel <nick> [reason]\r\n");
		return;
	}
	std::getline(iss, reason);
	if (!reason.empty() && reason[0] == ' ') reason = reason.substr(1);
	if (!reason.empty() && reason[0] == ':') reason = reason.substr(1);
	if (reason.empty()) reason = "Kicked";

	if (channelName.empty() || channelName[0] != '#') {
		cli.response(":server 403 " + cli.getNickname() + " " + channelName + " :No such channel\r\n");
		return;
	}

	Channel* channel = server.getChannel(channelName);
	if (!channel) {
		cli.response(":server 403 " + cli.getNickname() + " " + channelName + " :No such channel\r\n");
		return;
	}

	if (!channel->contains(cli.getNickname())) {
		cli.response(":server 442 " + cli.getNickname() + " " + channelName + " :You're not on that channel\r\n");
		return;
	}

	if (!channel->isOp(cli.getNickname())) {
		cli.response(":server 482 " + cli.getNickname() + " " + channelName + " :You're not channel operator\r\n");
		return;
	}

	std::map<int, Client>::iterator it = clients.begin();
	Client* target_client_ptr = NULL;
	for (; it != clients.end(); ++it) {
		if (it->second.getNickname() == target_nick) {
			target_client_ptr = &it->second;
			break;
		}
	}

	if (!channel->contains(target_nick)) {
		cli.response(":server 441 " + cli.getNickname() + " " + target_nick + " " + channelName + " :They aren't on that channel\r\n");
		cli.response(":server NOTICE " + channelName +
					" :User " + target_nick + " is not in channel " + channelName + "\r\n");
	return;
	}

	channel->removeUser(target_nick);

	std::string kickLine = ":" + cli.getNickname() + "!" + cli.getUsername() + "@" + cli.getHostname()
									+ " KICK " + channelName + " " + target_nick + " :" + reason + "\r\n";
	channel->broadcast(cli.getNickname(), kickLine, server);
	cli.response(kickLine); 

	if (target_client_ptr) {
		target_client_ptr->response(kickLine);
	}
}


