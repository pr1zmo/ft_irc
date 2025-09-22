/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:14:29 by zelbassa          #+#    #+#             */
/*   Updated: 2025/09/22 15:46:13 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef Command_HPP
#define Command_HPP

#include "ft_irc.h"

enum{
	ERR_INPUTTOOLONG,
	ERR_NOSUCHSERVER,
};

typedef struct cli_commands {
	std::string cmd;
	int			(*func)(Client&, std::vector<std::string>&);
} t_cli_commands;

typedef struct op_commands {
	std::string cmd;
	int			(*func)(int, std::vector<std::string>&);
} t_op_commands;


class Command {
	t_cli_commands _cli_cmds[10];
	t_op_commands _op_cmds[10];
	size_t _size;
	std::string _msg;
	int _isOp;
	public:
	std::vector <std::string> validCmds;
	Command();
	~Command();

	int parseCommand(char *msg);
	void setOp(int isOp) { _isOp = isOp; }
};

class Help {
	public:
		std::vector<std::string> full;
		std::map<std::string, std::vector<std::string> > topics;
		bool load();
		void printUsage() const;
		const std::vector<std::string>* getTopic(const std::string& key) const;
};

// PASS command class

class Pass : public Command {
	std::string _pass;
	public:
		Pass(const std::string &pass) : _pass(pass) {};
		~Pass() {};
};

class Nick : public Command {
	public:
		Nick(/* args */);
		~Nick();
};

class User : public Command {
	public:
		User(/* args */);
		~User();
};

class Oper : public Command {
	public:
		Oper(/* args */);
		~Oper();
};

class Quit : public Command {
	public:
		Quit(/* args */);
		~Quit();
};

class Join : public Command {
	public:
		Join(/* args */);
		~Join();
};

class Part : public Command {
	public:
		Part(/* args */);
		~Part();
};

class Privmsg : public Command {
	public:
		Privmsg(/* args */);
		~Privmsg();
};

class Notice : public Command {
	public:
		Notice(/* args */);
		~Notice();
};

class Ping : public Command {
	public:
		Ping(/* args */);
		~Ping();
};

#endif