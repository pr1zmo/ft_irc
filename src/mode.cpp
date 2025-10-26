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

    // Parse parameters to extract target and mode changes
    std::istringstream iss(param);
    std::string channelName;
    std::string target;
    std::string modeChanges;
    // last param is user or key for channel and its optional
    iss >> channelName >> modeChanges >> target;
    if (channelName.empty() || modeChanges.empty()) {
        cli.response("Error: MODE command requires a channel and mode changes.\r\n");
        return;
    }
    // Determine if target is a channel or user
    if (channelName[0] == '#') {
        // Handle channel mode changes
        Channel* channel = server.getChannel(channelName);
        if (!channel) {
            cli.response("Error: No such channel.\r\n");
            return;
        }
        // issuer must be on the channel
        if (!channel->contains(cli.getNickname())) {
            cli.response(":server 442 " + cli.getNickname() + " " + channelName + " :You're not on that channel\r\n");
            return;
        }
        // issuer must be operator
        if (!channel->isOp(cli.getNickname())) {
            cli.response(":server 482 " + cli.getNickname() + " " + channelName + " :You're not channel operator\r\n");
            return;
        }

        // Apply mode changes to the channel
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
        // Apply mode changes to the user
        // user->applyModeChanges(modeChanges);
    }
}
