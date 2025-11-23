#include "ft_irc.h"
#include "Channel.hpp"
#include <memory>
Join::Join(/* args */)
: Command()
{
}
Join::~Join()
{
}

void Join::execute(Client &cli, const std::string& rawParam, const std::string& /*cmd*/,
                        std::map<int, Client>& /*clients*/, Server& server)
{
	if (!cli.isRegistered()) { cli.response(":server 451 * :You have not registered\r\n"); return; }

	std::string param = rawParam; 
	if (param.empty()) { cli.response(":server 461 " + cli.getNickname() + " JOIN :Not enough parameters\r\n"); return; }

	std::string channelName, key;
	size_t sp = param.find(' ');
	channelName = (sp == std::string::npos ? param : param.substr(0, sp));
	if (sp != std::string::npos) key = param.substr(sp + 1); 

	if (channelName.empty() || channelName[0] != '#') {
		cli.response(":server 403 " + cli.getNickname() + " " + channelName + " :No such channel\r\n");
		return;
	}

    Channel* channel = server.getChannel(channelName);
	if (!channel) {
		channel = new Channel(channelName);       
		channel->setInviteOnly(false);
		channel->setTopicRestricted(false);
		if (key.empty())
		{
			channel->setPasswordProtected(false);
		} else {
			channel->setPasswordProtected(true);
			channel->setPassword(key);
		}
		channel->setUserLimit(0);
		channel->setHasLimit(false);
		server.addChannel(channelName, channel);
	}

	if (channel->isPasswordProtected()) {
		if (key.empty() || !channel->checkPassword(key)) {
			cli.response(":server 475 " + cli.getNickname() + " " + channelName + " :Cannot join channel (+k)\r\n");
			return;
		}
	}
	if (channel->isInviteOnly() && !channel->isInvited(cli.getNickname())) {
		cli.response(":server 473 " + cli.getNickname() + " " + channelName + " :Cannot join channel (invite only)\r\n");
		return;
	}
	if (channel->hasLimit() && channel->userCount() >= channel->getUserLimit()) {
		cli.response(":server 471 " + cli.getNickname() + " " + channelName + " :Cannot join channel (is full)\r\n");
		return;
	}
	if (!channel->addUser(cli.getNickname(), &cli)) {
		cli.response(":server 443 " + cli.getNickname() + " " + channelName + " :is already on channel\r\n");
		return;
	}

	if (channel->userCount() == 1) channel->addOp(cli,cli.getNickname(), &server);

	cli.joinChannel(channel);

	const std::string joinLine =
	":" + cli.getNickname() + "!" + cli.getUsername() + "@" + cli.getHostname() +
	" JOIN :" + channelName + "\r\n";

	cli.response(joinLine);
	channel->broadcast(cli.getNickname(),
		":" + cli.getNickname() + "!" + cli.getUsername() + "@" + cli.getHostname() + " JOIN :" + channelName + "\r\n",
		server);

	if (!channel->getTopic().empty()) {
		cli.response(":server 332 " + cli.getNickname() + " " + channelName + " :" + channel->getTopic() + "\r\n");
		cli.response(":server 333 " + cli.getNickname() + " " + channelName + " " + channel->getTopicSetBy() + " " + to_string98(channel->getTopicTimestamp()) + "\r\n");
	} else {
		cli.response(":server 331 " + cli.getNickname() + " " + channelName + " :No topic is set\r\n");
	}

	// Send the list of users in the channel
	std::string names = channel->namesList();
	cli.response(":server 353 " + cli.getNickname() + " = " + channelName + " :" + names + "\r\n");
	cli.response(":server 366 " + cli.getNickname() + " " + channelName + " :End of /NAMES list.\r\n");

	channel->uninviteUser(cli.getNickname());

}
