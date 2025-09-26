#ifndef Executioner_HPP
#define Executioner_HPP

#include "ft_irc.h"
#include "Commands.hpp"

class Client;
class Command;

class Executioner {
	private:

	public:
		Executioner();
		~Executioner();

		std::map<std::string, Command*> _commands;
		int run(Client &cli, const std::string &msg);
};

#endif