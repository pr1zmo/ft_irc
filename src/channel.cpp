#include "Channel.hpp"
#include "Server.hpp" // needed to call findClientByNick()
#include <iostream>

Channel::Channel(const std::string& n) : name(n) {}
Channel::~Channel() {}

bool Channel::addUser(const std::string& nick, Client* clientPtr) {
	if (isBanned(nick)) return false;
	if (users_set.insert(nick).second) {
		users[nick] = clientPtr;
		invitedUsers.erase(nick); 
		return true;
	}
	return false;
}
bool Channel::hasUser(const std::string& nick) const {
	return users_set.find(nick) != users_set.end();
}

bool Channel::removeUser(const std::string& nick) {
	if (users_set.erase(nick) > 0) {
		users.erase(nick);
		ops.erase(nick); 
		return true;
	}
	return false;
}

bool Channel::contains(const std::string& nick) const {
	return users_set.find(nick) != users_set.end();
}

size_t Channel::userCount() const { return users.size(); }

bool Channel::addOp(Client& cli, const std::string& nick, Server* server) {
	if (!contains(nick)) return false;

	std::string modeMsg = ":" + cli.getNickname() + "!" + cli.getUsername() + "@" + cli.getHostname() +
								" MODE " + name + " +o " + nick + "\r\n";
	this->broadcast("", modeMsg, *server);

	ops.insert(nick); 
	return true;
}

bool Channel::removeOp(Client& cli, const std::string& nick, Server* server) {
	if (ops.find(nick) == ops.end()) return false;

	std::string modeMsg = ":" + cli.getNickname() + "!" + cli.getUsername() + "@" + cli.getHostname() +
								" MODE " + name + " -o " + nick + "\r\n";
	this->broadcast("", modeMsg, *server);

	ops.erase(nick); // Remove the user from the ops set
	return true;
}

bool Channel::isOp(const std::string& nick) const {
	return ops.find(nick) != ops.end();
}

bool Channel::inviteUser(const std::string& nick) {
	invitedUsers.insert(nick);
	return true;
}

bool Channel::uninviteUser(const std::string& nick) {
	return invitedUsers.erase(nick) > 0;
}

bool Channel::isInvited(const std::string& nick) const {
	return invitedUsers.find(nick) != invitedUsers.end();
}

bool Channel::banUser(const std::string& nick) {

	removeUser(nick);
	bannedUsers.insert(nick);
	return true;
}

bool Channel::unbanUser(const std::string& nick) {
	return bannedUsers.erase(nick) > 0;
}

bool Channel::isBanned(const std::string& nick) const {
	return bannedUsers.find(nick) != bannedUsers.end();
}

bool Channel::kickUser(const std::string& nick, const std::string& reason) {
	if (!contains(nick)) return false;
	removeUser(nick);
	(void)reason;
	return true;
}

void Channel::pushMessage(const Message& m) {
	messages.push_back(m);
	const size_t MAX_HISTORY = 200;
	if (messages.size() > MAX_HISTORY) messages.pop_front();
}

const Message& Channel::getMessage(size_t idx) const {
	return messages.at(idx);
}

size_t Channel::messageCount() const {
	return messages.size();
}

void Channel::broadcast(const std::string& nick, const std::string& msg, Server& server) const {
	(void)server;
	for (std::map<std::string, Client*>::const_iterator it = users.begin(); it != users.end(); ++it) {
		Client* clientPtr = it->second;
		if (clientPtr && clientPtr->getNickname() != nick) {
			clientPtr->response(msg);
		}
	}
}

//channel namelist

std::string Channel::namesList() const {
	std::string names;
	for (std::map<std::string, Client*>::const_iterator it = users.begin(); it != users.end(); ++it) {
		const std::string& nick = it->first;
		if (isOp(nick)) {
			names += "@" + nick + " ";
		} else {
			names += nick + " ";
		}
	}
	if (!names.empty() && names[names.size() - 1] == ' ') {
		names.erase(names.size() - 1); // remove trailing space
	}
	return names;
}


void Channel::debugPrint() const {
	std::cout << "Channel #" << name << " users(" << users.size() << "):";
	for (std::map<std::string, Client*>::const_iterator it = users.begin(); it != users.end(); ++it) {
		std::cout << " " << it->first;
	}
	std::cout << "\nops:";
	for (std::set<std::string>::const_iterator it = ops.begin(); it != ops.end(); ++it) std::cout << " " << *it;
	std::cout << std::endl;
}
std::string Channel::getTopicSetBy() const {
	return topicSetBy;
}

time_t Channel::getTopicTimestamp() const {
	return topicTimestamp;
}
void Channel::setTopic(const std::string& newTopic, const std::string& setter) {
	topic = newTopic;
	topicSetBy = setter;
	topicTimestamp = time(NULL); 
}
void Channel::applyModeChanges(const std::string& modeChanges, const std::string& target, Client& cli, Server& server) {
	bool adding = true;
	std::cout << "Applying mode changes: " << modeChanges << std::endl;

	for (size_t i = 0; i < modeChanges.size(); ++i) {
		char ch = modeChanges[i];
		if (ch == '+') {
			adding = true;
		} else if (ch == '-') {
			adding = false;
		} else {
			std::string modeMsg = ":" + cli.getNickname() + "!" + cli.getUsername() + "@" + cli.getHostname() + " MODE " + name + " " + (adding ? "+" : "-") + ch;

			switch (ch) {
				case 'o': // operator status
					if (users.find(target) == users.end()) {
						cli.response(":server 441 " + cli.getNickname() + " " + target + " " + name + " :They aren't on that channel\r\n");
						return;
					}
					if (adding) {
						addOp(cli, target, &server);
					} else {
						if (ops.size() == 1 && isOp(target)) {
							cli.response(":server 482 " + cli.getNickname() + " " + name + " :Cannot remove the last channel operator\r\n");
							return;
						}
						removeOp(cli,target, &server);
					}
					break;

				case 'i': // invite-only
					inviteOnly = adding;
					broadcast("", modeMsg + "\r\n", server);
					break;

				case 't': // topic protection
					topicRestricted = adding;
					broadcast("", modeMsg + "\r\n", server);
					break;

				case 'k': // channel key
					if (adding) {
						if (target.empty()) {
							cli.response(":server 461 " + cli.getNickname() + " MODE :Not enough parameters\r\n");
							return;
						}
						setPassword(target);
						passwordProtected = true;
						modeMsg += " " + target;
					} else {
						passwordProtected = false;
					}
					broadcast("", modeMsg + "\r\n", server);
					break;

				case 'l': // user limit
					if (adding) {
						if (target.empty()) {
							cli.response(":server 461 " + cli.getNickname() + " MODE :Not enough parameters\r\n");
							return;
						}
						size_t limit = std::atoi(target.c_str());
						setUserLimit(limit);
						has_limit = true;
						modeMsg += " " + target;
					} else {
						has_limit = false;
					}
					broadcast("", modeMsg + "\r\n", server);
					break;

				default: // unknown mode
					cli.response(":server 472 " + cli.getNickname() + " " + ch + " :is unknown mode char to me\r\n");
					return;
			}
		}
	}
}
