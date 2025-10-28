#include "ft_irc.h"
#include "Channel.hpp"

Join::Join(/* args */)
: Command()
{
}
Join::~Join()
{
}

// helpers
static inline std::string trimWSCRLF(const std::string& s) {
	size_t a = 0, b = s.size();
	while (a < b && (s[a] == ' ' || s[a] == '\t' || s[a] == '\r' || s[a] == '\n')) ++a;
	while (b > a && (s[b-1] == ' ' || s[b-1] == '\t' || s[b-1] == '\r' || s[b-1] == '\n')) --b;
	return s.substr(a, b - a);
}

// RFC1459 case-fold (optional, helps unify map keys)
static inline char rfc1459_tolower(char c) {
	if (c >= 'A' && c <= 'Z') return char(c + ('a' - 'A'));
	if (c == '{') return '[';
	if (c == '}') return ']';
	if (c == '|') return '\\';
	return c;
}
static inline std::string canonChan(const std::string& name) {
	std::string out; out.reserve(name.size());
	for (size_t i = 0; i < name.size(); ++i) out.push_back(rfc1459_tolower(name[i]));
	return out;
}

void Join::execute(Client &cli, const std::string& rawParam, const std::string& /*cmd*/,
						std::map<int, Client>& /*clients*/, Server& server)
{
	if (!cli.isRegistered()) { cli.response(":server 451 * :You have not registered\r\n"); return; }

	std::string param = trimWSCRLF(rawParam);
	if (param.empty()) { cli.response(":server 461 " + cli.getNickname() + " JOIN :Not enough parameters\r\n"); return; }

	std::string channelName, key;
	size_t sp = param.find(' ');
	channelName = trimWSCRLF(sp == std::string::npos ? param : param.substr(0, sp));
	if (sp != std::string::npos) key = trimWSCRLF(param.substr(sp + 1));

	if (channelName.empty() || channelName[0] != '#') {
		cli.response(":server 403 " + cli.getNickname() + " " + channelName + " :No such channel\r\n");
		return;
	}

	const std::string chanKey = canonChan(channelName);

	Channel* channel = server.getChannel(chanKey);
	if (!channel) {
		channel = new Channel(channelName);       
		channel->setInviteOnly(false);
		channel->setTopicRestricted(false);
		channel->setUserLimit(0);
		channel->setHasLimit(false);
		server.addChannel(chanKey, channel);
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

	if (channel->userCount() == 1) channel->addOp(cli.getNickname(),server);

	cli.joinChannel(channel);

	const std::string joinLine =
	":" + cli.getNickname() + "!" + cli.getUsername() + "@" + cli.getHostname() +
	" JOIN :" + channelName + "\r\n";

	cli.response(joinLine);
	channel->broadcast(cli.getNickname(),
		":" + cli.getNickname() + "!" + cli.getUsername() + "@" + cli.getHostname() + " JOIN :" + channelName + "\r\n",
		server);

	if (!channel->getTopic().empty())
		cli.response(":server 332 " + cli.getNickname() + " " + channelName + " :" + channel->getTopic() + "\r\n");
	else
		cli.response(":server 331 " + cli.getNickname() + " " + channelName + " :No topic is set\r\n");

	// names list (353/366)
	{
		std::string names = channel->namesList(); // ensure this returns the full list including the joiner
		// As a safety: if namesList() for some reason omits the joiner, append it
		if (names.find(cli.getNickname()) == std::string::npos) {
			if (!names.empty()) names += " ";
			names += cli.getNickname();
		}
		cli.response(":server 353 " + cli.getNickname() + " = " + channelName + " :" + names + "\r\n");
		cli.response(":server 366 " + cli.getNickname() + " " + channelName + " :End of /NAMES list.\r\n");
	}

	channel->uninviteUser(cli.getNickname());

}
