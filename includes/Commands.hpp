/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:14:29 by zelbassa          #+#    #+#             */
/*   Updated: 2025/10/06 14:49:22 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef Command_HPP
#define Command_HPP

#include "ft_irc.h"

typedef struct cli_commands {
	std::string cmd;
	int			(*func)(Client&, std::vector<std::string>&);
} t_cli_commands;

typedef struct op_commands {
	std::string cmd;
	int			(*func)(int, std::vector<std::string>&);
} t_op_commands;


class Command {
	public:
		t_cli_commands _cli_cmds[10];
		t_op_commands _op_cmds[10];
		size_t _size;
		std::string _msg;
		int _isOp;
		public:
		std::vector <std::string> validCmds;
		Command();
		virtual ~Command();

		int parseCommand(char *msg);
		void setOp(int isOp) { _isOp = isOp; }
		virtual void execute(Client &cli, const std::string& param, const std::string& cmd, std::map<int, Client>& clients);
};

class Help : public Command {
	public:
		Help();
		~Help();
std::vector<std::string> full;
		std::string const helpContent;
		std::map<std::string, std::vector<std::string> > topics;
		bool load();
		std::string printUsage() const;
		const std::vector<std::string>* getTopic(const std::string& key) const;
		void execute(Client &cli, const std::string& param, const std::string& cmd, std::map<int, Client>& clients);
};

// PASS command class

class Pass : public Command {
	std::string _pass;
	public:
		Pass() {};
		~Pass() {};

		void execute(Client &cli, const std::string& param,	 const std::string& cmd, std::map<int, Client>& clients);
};

class Nick : public Command {
	public:
		Nick(/* args */);
		~Nick();
		void execute(Client &cli, const std::string& param, const std::string& cmd, std::map<int, Client>& clients);
};

class User : public Command {
	public:
		User(/* args */);
		~User();
		// void execute(Client &cli);
		void execute(Client &cli, const std::string& param, const std::string& cmd, std::map<int, Client>& clients);
};

class Oper : public Command {
	public:
		Oper(/* args */);
		~Oper();
		// void execute(Client &cli);
		void execute(Client &cli, const std::string& param, const std::string& cmd, std::map<int, Client>& clients);
};

class Quit : public Command {
	public:
		Quit(/* args */);
		~Quit();
		// void execute(Client &cli);
		void execute(Client &cli, const std::string& param, const std::string& cmd, std::map<int, Client>& clients);
};

class Join : public Command {
	public:
		Join(/* args */);
		~Join();
		// void execute(Client &cli);
		void execute(Client &cli, const std::string& param, const std::string& cmd, std::map<int, Client>& clients);
};

class Part : public Command {
	public:
		Part(/* args */);
		~Part();
		// void execute(Client &cli);
		void execute(Client &cli, const std::string& param, const std::string& cmd, std::map<int, Client>& clients);
};

class Privmsg : public Command {
	public:
		Privmsg(/* args */);
		~Privmsg();

		int userExists(std::string uname);
		void handleFileTransfer(Client &cli, const std::string& param);
		void execute(Client &cli, const std::string& param, const std::string& cmd, std::map<int, Client>& clients);
};

class Notice : public Command {
	public:
		Notice(/* args */);
		~Notice();
		// void execute(Client &cli);
		void execute(Client &cli, const std::string& param, const std::string& cmd, std::map<int, Client>& clients);
};

class Ping : public Command {
	public:
		Ping(/* args */);
		~Ping();
		// void execute(Client &cli);
		void execute(Client &cli, const std::string& param, const std::string& cmd, std::map<int, Client>& clients);
};

class Pong : public Command {
	public:
		Pong(/* args */);
		~Pong();
		// void execute(Client &cli);
		void execute(Client &cli, const std::string& param, const std::string& cmd, std::map<int, Client>& clients);
};

class File : public Command {
	std::ifstream _fileStream;
	socklen_t _recipient_ip;
	int _recipient_port;
	// int fileSize;
	
	Client *sender;
	Client *receiver;
	public:
		File(/* args */);
		~File();
		
		int establish_connection(socklen_t ip, int port);
		int parseFile();
		int parseCommand(const std::string &msg);
		int sendFile();
		int receiveFile();
		void execute(Client &cli, const std::string &msg, const std::string& cmd, std::map<int, Client>& clients);
};

#endif