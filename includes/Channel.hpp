#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <set>
#include <deque>
#include <ctime>
#include <algorithm>
#include <iostream>

struct Message {
    std::string sender;
    std::string text;
    time_t timestamp;
};

class Channel {
public:
    Channel(const std::string& name);
    ~Channel();

    
    bool addUser(const std::string& nick);           // returns true if added
    bool removeUser(const std::string& nick);        // returns true if removed
    bool contains(const std::string& nick) const;
    size_t userCount() const;

    
    bool addOp(const std::string& nick);
    bool removeOp(const std::string& nick);
    bool isOp(const std::string& nick) const;

   
    bool inviteUser(const std::string& nick);
    bool uninviteUser(const std::string& nick);
    bool isInvited(const std::string& nick) const;

    
    bool banUser(const std::string& nick);
    bool unbanUser(const std::string& nick);
    bool isBanned(const std::string& nick) const;
    bool kickUser(const std::string& nick, const std::string& reason = "");

    
    void pushMessage(const Message& m);
    const Message& getMessage(size_t idx) const;
    size_t messageCount() const;
    void broadcast(const std::string& msg) const;

    
    void debugPrint() const;

private:
    std::string name;
    std::vector<std::string> users;            // ordered list (join order)
    std::set<std::string> users_set;           // fast membership checks
    std::deque<Message> messages;              // ordered history
    std::set<std::string> bannedUsers;         // banned nicks
    std::set<std::string> invitedUsers;        // invited nicks
    std::set<std::string> ops;                 // channel operators (mode +o)
};
#endif