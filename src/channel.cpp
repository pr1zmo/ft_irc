#include "Channel.hpp"
#include "Server.hpp" // needed to call findClientByNick()
#include <iostream>

Channel::Channel(const std::string& n) : name(n) {}
Channel::~Channel() {}

bool Channel::addUser(const std::string& nick, Client* clientPtr) {
    if (isBanned(nick)) return false;
    if (users_set.insert(nick).second) {
        users[nick] = clientPtr;
        invitedUsers.erase(nick); // clear invite on join
        return true;
    }
    return false;
}

bool Channel::removeUser(const std::string& nick) {
    if (users_set.erase(nick) > 0) {
        users.erase(nick);
        ops.erase(nick); // remove op if they leave
        return true;
    }
    return false;
}

bool Channel::contains(const std::string& nick) const {
    return users_set.find(nick) != users_set.end();
}

size_t Channel::userCount() const { return users.size(); }

bool Channel::addOp(const std::string& nick) {
    if (!contains(nick)) return false;
    ops.insert(nick);
    return true;
}

bool Channel::removeOp(const std::string& nick) {
    return ops.erase(nick) > 0;
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
    // remove from channel if present
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
    // kick only if member exists
    if (!contains(nick)) return false;
    removeUser(nick);
    // server or command handler should send KICK reply to user and channel
    // here we only modify channel state
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
    for (std::map<std::string, Client*>::const_iterator it = users.begin(); it != users.end(); ++it) {
        Client* clientPtr = it->second;
        if (clientPtr && clientPtr->getNickname() != nick) {
            clientPtr->response(msg);
            clientPtr->sendPendingMessages();
        }
    }
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

void Channel::applyModeChanges(const std::string& modeChanges, const std::string& target, Client& cli, Server& server) {
    bool adding = true;
    std::cout << "Applying mode changes: " << modeChanges << std::endl;
    // Separate mode changes by words
    std::istringstream iss(modeChanges);

    for (size_t i = 0; i < modeChanges.size(); ++i) {
        char ch = modeChanges[i];
        if (ch == '+') {
            adding = true;
        } else if (ch == '-') {
            adding = false;
        } else {
            switch (ch) {
                case 'o': // operator status
                    //check if target user is in channel
                    if (users.find(target) == users.end()) {
                        cli.response("Error: User " + target + " is not in channel " + name + ".\r\n");
                        return;
                    }
                    if (!users.empty()) {
                        // get target nick from command parameters in real implementation
                        std::string targetNick = target;
                        if (adding) {
                            addOp(targetNick);
                        } else {
                            removeOp(targetNick);
                        }
                    }
                    break;
                // Handle other modes i t k l
                case 'i': // invite only
                    // Implement invite-only logic
                    if (adding) {
                        // set invite-only flag
                        inviteOnly = true;
                    } else {
                        // unset invite-only flag
                        inviteOnly = false;
                    }
                    break;
                case 't': // topic set by ops only
                    // Implement topic restriction logic
                    if (adding) {
                        topicRestricted = true;
                    } else {
                        topicRestricted = false;
                    }
                    break;
                case 'k': // password protected
                    // Implement password protection logic
                    if (adding) {
                        // check for password parameter and set new password
                        if (target.empty()) {
                            cli.response("Error: MODE +k requires a password parameter.\r\n");
                            return;
                        }
                        //set password
                        setPassword(target);
                        passwordProtected = true;
                    } else {
                        // unset password
                        passwordProtected = false;
                    }
                    break;
                case 'l': // user limit
                    // Implement user limit logic
                    if (adding) {
                        // check for limit parameter and set new limit
                        if (target.empty()) {
                            cli.response("Error: MODE +l requires a limit parameter.\r\n");
                            return;
                        }
                        size_t limit = std::atoi(target.c_str());
                        setUserLimit(limit);
                        has_limit = true;
                    } else {
                        // unset user limit
                        has_limit = false;
                    }
                    break;
                default:
                    break;
            }
        }
    }
}
