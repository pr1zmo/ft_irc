#include "Executioner.hpp"

Executioner::Executioner()
{
	_commands["PASS"] = new Pass();
	_commands["NICK"] = new Nick();
	_commands["USER"] = new User();
	_commands["OPER"] = new Oper();
	_commands["QUIT"] = new Quit();
	_commands["JOIN"] = new Join();
	_commands["PART"] = new Part();
	_commands["PRIVMSG"] = new Privmsg();
	_commands["NOTICE"] = new Notice();
	_commands["PING"] = new Ping();
	_commands["PONG"] = new Pong();
	_commands["HELP"] = new Help();
	_commands["DCC"] = new File();
}

Executioner::~Executioner() {
	for (std::map<std::string, Command*>::iterator it = _commands.begin(); it != _commands.end(); ++it) {
		delete it->second;
	}

	_commands.clear();
}

int Executioner::run(Client &cli, const std::string &msg) {
	size_t pos = msg.find(' ');
	std::string cmd = (pos == std::string::npos) ? msg : msg.substr(0, pos);
	
	std::map<std::string, Command*>::iterator it = _commands.find(cmd);
	if (it != _commands.end()) {
		Command* cmdInstance = it->second;
		
		std::string params = (pos == std::string::npos) ? "" : msg.substr(pos + 1);
		
		cmdInstance->execute(cli, params);
		
		return 0;
	} else {
		cli.response(cli.getNickname() + ": Unknown command: " + cmd + "\r\n");
			return 0;
		return -1;
	}
}