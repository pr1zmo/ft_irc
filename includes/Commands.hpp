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
		Command();
		~Command();

		int parseCommand(char *msg);
		void setOp(int isOp) { _isOp = isOp; }
};

#endif