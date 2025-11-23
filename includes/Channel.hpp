/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/20 19:00:15 by zelbassa          #+#    #+#             */
/*   Updated: 2025/11/23 21:44:18 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include <set>
#include <deque>
#include <ctime>
#include <algorithm>
#include <iostream>
#include "Client.hpp"


class Client;
class Server;
struct Message {
	std::string sender;
	std::string text;
	time_t timestamp;
};

class Channel {
public:
	Channel(const std::string& name);
	~Channel();


	bool addUser(const std::string& nick, Client* clientPtr);           // returns true if added
	bool removeUser(const std::string& nick);        // returns true if removed
	bool contains(const std::string& nick) const;
	size_t userCount() const;

	bool addOp(Client& cli, const std::string& nick, Server* server);
	// if 'server' is non-null the method will broadcast a MODE -o message to the channel
	bool removeOp(Client& cli, const std::string& nick, Server* server = NULL);
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
	void broadcast(const std::string& nick, const std::string& msg, class Server& server) const;

	
	void debugPrint() const;
	std::string getName() const { return name; }
	std::string getTopic() const { return topic; }
	// void setTopic(const std::string& t) { topic = t; }

	void setPassword(const std::string& p) { password = p; }
	bool checkPassword(const std::string& p) const { return password == p; }    
	void applyModeChanges(const std::string& modeChanges, const std::string& target, Client& cli, class Server& server);
	void setUserLimit(size_t limit) { userLimit = limit; }
	size_t getUserLimit() const { return userLimit; }
	void setInviteOnly(bool flag) { inviteOnly = flag; }
	bool isInviteOnly() const { return inviteOnly; }
	void setTopicRestricted(bool flag) { topicRestricted = flag; }
	bool isTopicRestricted() const { return topicRestricted; }
	void setPasswordProtected(bool flag) { passwordProtected = flag; }
	bool isPasswordProtected() const { return passwordProtected; }
	void setHasLimit(bool flag) { has_limit = flag; }
	bool hasLimit() const { return has_limit; }
	std::string namesList() const;
	bool hasUser(const std::string& nick) const;
	bool isEmpty() const {
		return users.empty();
	}

	std::string getTopicSetBy() const;
	time_t getTopicTimestamp() const;
	void setTopic(const std::string& newTopic, const std::string& setter);

private:
	std::string name;
	std::string topic;
	std::string password;
	size_t userLimit;
	bool has_limit;
	bool inviteOnly;
	bool topicRestricted;
	bool passwordProtected;
	std::string topicSetBy; 
	time_t topicTimestamp;  

	std::map<std::string, Client*> users;      // ordered list (join order)
	std::set<std::string> users_set;           // fast membership checks
	std::deque<Message> messages;              // ordered history
	std::set<std::string> bannedUsers;         // banned nicks
	std::set<std::string> invitedUsers;        // invited nicks
	std::set<std::string> ops;                 // channel operators (mode +o)
};