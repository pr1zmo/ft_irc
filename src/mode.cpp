#include "Commands.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include "ft_irc.h"

Mode::Mode(/* args */) {
// Constructor implementation
}

Mode::~Mode() {
// Destructor implementation
}

void Mode::execute(Client &cli, const std::string& param, const std::string& cmd, std::map<int, Client>& clients, Server& server) {
	(void)cmd;
	std::istringstream iss(param);
	std::string channelName;
	std::string target;
	std::string modeChanges;

	if (!cli.isRegistered()) {
		cli.response("ERROR :You have not registered\r\n");
		return;
	}

	iss >> channelName >> modeChanges >> target;
	if (channelName.empty() || modeChanges.empty()) {
		cli.response("Error: MODE command requires a channel and mode changes.\r\n");
		return;
	}
	if (channelName[0] == '#') {
		Channel* channel = server.getChannel(channelName);
		if (!channel) {
			cli.response("Error: No such channel.\r\n");
			return;
		}
		if (!channel->contains(cli.getNickname())) {
			cli.response(":server 442 " + cli.getNickname() + " " + channelName + " :You're not on that channel\r\n");
			return;
		}

		std::cout << "Checking if " << cli.getNickname() << " is op on channel " << channelName << std::endl;
		if (!channel->isOp(cli.getNickname())) {
			cli.response(":server 482 " + cli.getNickname() + " " + channelName + " :You're not channel operator\r\n");
			return;
		}

		channel->applyModeChanges(modeChanges, target, cli, server);
	} else {
		// Handle user mode changes
		Client* user = NULL;
		for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
			if (it->second.getNickname() == target) {
				user = &(it->second);
				break;
			}
		}
		if (!user) {
			cli.response("Error: No such user.\r\n");
			return;
		}
	}
}
