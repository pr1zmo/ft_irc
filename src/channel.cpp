// ...existing code...
#include "Channel.hpp"

Channel::Channel(const std::string& n) : name(n) {}
Channel::~Channel() {}

bool Channel::addUser(const std::string& nick) {
    if (isBanned(nick)) return false;
    if (users_set.insert(nick).second) {
        users.push_back(nick);
        invitedUsers.erase(nick); // clear invite on join
        return true;
    }
    return false;
}

bool Channel::removeUser(const std::string& nick) {
    if (users_set.erase(nick) > 0) {
        users.erase(std::remove(users.begin(), users.end(), nick), users.end());
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
void Channel::broadcast(const std::string& msg) const {
    for (size_t i = 0; i < users.size(); ++i) {
        //send the message to each user
        std::cout << "To " << users[i] << ": " << msg << std::endl;
    }
}

void Channel::debugPrint() const {
    std::cout << "Channel #" << name << " users(" << users.size() << "):";
    for (size_t i = 0; i < users.size(); ++i) std::cout << " " << users[i];
    std::cout << "\nops:";
    for (std::set<std::string>::const_iterator it = ops.begin(); it != ops.end(); ++it) std::cout << " " << *it;
    std::cout << std::endl;
}
