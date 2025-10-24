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
    std::string target;
    std::string modeChanges;
    iss >> target >> modeChanges;
    if (target.empty() || modeChanges.empty()) {
        cli.response("Error: MODE command requires a target and mode changes.\r\n");
        return;
    }
    // Determine if target is a channel or user
    if (target[0] == '#') {
        // Handle channel mode changes
        Channel* channel = server.getChannel(target);
        if (!channel) {
            cli.response("Error: No such channel.\r\n");
            return;
        }
        // Apply mode changes to the channel
        channel->applyModeChanges(modeChanges);
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
