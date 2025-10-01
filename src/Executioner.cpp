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

// void static printBuffer(const std::string& label, const char* buffer, int size) {
// 	std::cout << label << " (size=" << size << "): ";
// 	for (int i = 0; i < size; i++) {
// 		unsigned char c = static_cast<unsigned char>(buffer[i]);
// 		if (c == '\r') {
// 			std::cout << "\\r";
// 		} else if (c == '\n') {
// 			std::cout << "\\n";
// 		} else if (c == '\t') {
// 			std::cout << "\\t";
// 		} else if (c < 32 || c > 126) {
// 			// Non-printable characters as hex
// 			std::cout << "\\x" << std::hex << std::setw(2) << std::setfill('0') << (int)c << std::dec;
// 		} else {
// 			std::cout << c;
// 		}
// 	}
// 	std::cout << std::endl;
// }

int Executioner::run(Client &cli, const std::string &msg) {
	Command command;

	int ret = command.parseCommand(const_cast<char*>(msg.c_str()));
	if (ret != 0) {
		cli.response("Error: Command too long.\r\n");
		return ret;
	}

	// size_t pos = msg.find(' ');
	size_t pos = msg.find(' ');
	
	std::string cmd = (pos == std::string::npos) ? msg : msg.substr(0, pos);

	std::map<std::string, Command*>::iterator it = _commands.find(cmd);
	if (it != _commands.end()) {
		Command* cmdInstance = it->second;
		// cmdInstance->setOp(cli.isOperator() ? 1 : 0);
		cmdInstance->execute(cli, (pos == std::string::npos) ? "" : msg.substr(pos + 1));
	} else {
		cli.response("Error: Unknown command.\r\n");
		return -1;
	}
	return 0;
}