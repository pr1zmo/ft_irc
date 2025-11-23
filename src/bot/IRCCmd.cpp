#include "IRCBot.hpp"


void IRCBot::dispatchCommand(const std::string &nick, const std::string &target, const std::string &cmd, const std::string &args)
{
	if (cmd == "ping")
		cmdPing(nick, target);

	else if (cmd == "say")
		cmdSay(target, args);

	else if (cmd == "join")
		cmdJoin(nick, args);

	else if (cmd == "part")
		cmdPart(nick, args);

	else if (cmd == "help")
		cmdHelp(nick, target);

	else
		sendPriv(target, "Unknown command: " + cmd);
}

void IRCBot::cmdPing(const std::string &nick, const std::string &target)
{
	sendPriv(target, nick + ": pong!");
}

void IRCBot::cmdSay(const std::string &target, const std::string &args)
{
	if (args.empty())
		sendPriv(target, "Usage: !say <text>");
	else
		sendPriv(target, args);
}

void IRCBot::cmdJoin(const std::string &, const std::string &chan)
{
	sendMsg("JOIN " + chan + "\r\n");
}

void IRCBot::cmdPart(const std::string &, const std::string &chan)
{
	sendMsg("PART " + chan + "\r\n");
}

void IRCBot::cmdHelp(const std::string &, const std::string &target)
{
	sendPriv(target, "Commands: !ping, !say, !join, !part, !help");
}

void IRCBot::sendPriv(const std::string &target, const std::string &text)
{
	sendMsg("PRIVMSG " + target + " :" + text + "\r\n");
}

void IRCBot::sendRegistration()
{
	sendMsg("PASS " + _server_pass + "\r\n");
	sendMsg("NICK " + _nickname + "\r\n");
	sendMsg("USER " + _username + " 0 * :" + _realname + "\r\n");

}

void splitCommand(const std::string &input, std::string &cmd, std::string &args)
{
	size_t space = input.find(' ');
	if (space == std::string::npos)
	{
		cmd = input;
		args = "";
	}
	else
	{
		cmd = input.substr(0, space);
		args = input.substr(space + 1);
		trim(args);
	}
}

