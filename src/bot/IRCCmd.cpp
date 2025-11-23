#include "IRCBot.hpp"


void IRCBot::dispatchCommand(const std::string &nick, const std::string &target, const std::string &cmd, const std::string &args)
{
<<<<<<< HEAD
	if (cmd == "ping")
		cmdPing(nick, target);

	else if (cmd == "say")
		cmdSay(nick, target, args);

	else if (cmd == "join")
		cmdJoin(nick, args);

	else if (cmd == "part")
		cmdPart(nick, args);

	else if (cmd == "help")
		cmdHelp(nick, target);

	else
		sendPriv(target, "Unknown command: " + cmd);
=======
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
>>>>>>> 546865a6eb5ab010618260e237c9889e34a29b11
}

void IRCBot::cmdPing(const std::string &nick, const std::string &target)
{
<<<<<<< HEAD
	sendPriv(target, nick + ": pong!");
}

void IRCBot::cmdSay(const std::string& /*nick*/ , const std::string &target, const std::string &args)
{
	if (args.empty())
		sendPriv(target, "Usage: !say <text>");
	else
		sendPriv(target, args);
=======
    sendPriv(target, nick + ": pong!");
}

void IRCBot::cmdSay(const std::string &target, const std::string &args)
{
    if (args.empty())
        sendPriv(target, "Usage: !say <text>");
    else
        sendPriv(target, args);
>>>>>>> 546865a6eb5ab010618260e237c9889e34a29b11
}

void IRCBot::cmdJoin(const std::string &, const std::string &chan)
{
<<<<<<< HEAD
	sendMsg("JOIN " + chan + "\r\n");
=======
    sendMsg("JOIN " + chan + "\r\n");
>>>>>>> 546865a6eb5ab010618260e237c9889e34a29b11
}

void IRCBot::cmdPart(const std::string &, const std::string &chan)
{
<<<<<<< HEAD
	std::cout << "chan " << chan << std::endl;
	sendMsg("PART " + chan + "\r\n");
=======
    sendMsg("PART " + chan + "\r\n");
>>>>>>> 546865a6eb5ab010618260e237c9889e34a29b11
}

void IRCBot::cmdHelp(const std::string &, const std::string &target)
{
<<<<<<< HEAD
	sendPriv(target, "Commands: !ping, !say, !join, !part, !help");
=======
    sendPriv(target, "Commands: !ping, !say, !join, !part, !help");
>>>>>>> 546865a6eb5ab010618260e237c9889e34a29b11
}

void IRCBot::sendPriv(const std::string &target, const std::string &text)
{
<<<<<<< HEAD
	sendMsg("PRIVMSG " + target + " :" + text + "\r\n");
=======
    sendMsg("PRIVMSG " + target + " :" + text + "\r\n");
>>>>>>> 546865a6eb5ab010618260e237c9889e34a29b11
}

void IRCBot::sendRegistration()
{
<<<<<<< HEAD
	sendMsg("PASS " + _server_pass + "\r\n");
	sendMsg("NICK " + _nickname + "\r\n");
	sendMsg("USER " + _username + " 0 * :" + _realname + "\r\n");
=======
    sendMsg("PASS " + _server_pass + "\r\n");
    sendMsg("NICK " + _nickname + "\r\n");
    sendMsg("USER " + _username + " 0 * :" + _realname + "\r\n");

>>>>>>> 546865a6eb5ab010618260e237c9889e34a29b11
}

void splitCommand(const std::string &input, std::string &cmd, std::string &args)
{
<<<<<<< HEAD
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
=======
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
>>>>>>> 546865a6eb5ab010618260e237c9889e34a29b11
}

