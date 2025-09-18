#include "ft_irc.h"

Command::Command()
: _size(0), _msg("") {
}

Command::~Command() {
}

int Command::parseCommand(char *msg) {
	_size = strlen(msg);
	if (_size > BUFFER_SIZE) {
		return ERR_INPUTTOOLONG;
	}
	_msg = std::string(msg);
	return 0;
}

