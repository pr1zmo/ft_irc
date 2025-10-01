/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   help.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:14:09 by zelbassa          #+#    #+#             */
/*   Updated: 2025/10/01 16:24:23 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Help.cpp
#include "Commands.hpp"

Help::Help() : helpContent( 
			"*** ft_irc — HELP ***\n"
			"\n"
			"Server start (shell):\n"
			"  ./ircserv <port> <password>\n"
			"\n"
			"Registration (do these first):\n"
			"  PASS <password>                  — provide server password\n"
			"  NICK <nickname>                  — set your nickname\n"
			"  USER <username> 0 * :<realname>  — set username & real name\n"
			"\n"
			"Messaging:\n"
			"  PRIVMSG <nick>|<#channel> :<text>   — send a private or channel message\n"
			"File transfer:\n"
			"  DCC <filename> <ip> <port> — send a file to a user\n"
			"\n"
			"Channels:\n"
			"  JOIN <#chan>[,<#chan>...] [<key>[,<key>...]]   — join/create channels\n"
			"\n"
			"Channel operators:\n"
			"  KICK <#chan> <nick> [<reason>]                 — remove a user\n"
			"  INVITE <nick> <#chan>                          — invite a user\n"
			"  TOPIC <#chan> [:<new-topic>]                   — set/view the topic\n"
			"  MODE <#chan> {[+|-]i|t|k|o|l} [<args>...]      — channel modes:\n"
			"     +i/-i         invite-only on/off\n"
			"     +t/-t         restrict TOPIC to ops / allow all\n"
			"     +k <key>/-k   set/remove channel key (password)\n"
			"     +o <nick>/-o  give/take operator privilege\n"
			"     +l <n>/-l     set/remove user limit\n"
			"\n"
			"Conventions:\n"
			"  • Commands are case-insensitive and end with CRLF (\\r\\n).\n"
			"  • Channels start with '#'. The last parameter may contain spaces and must be prefixed with ':'.\n"
			"  • Targets accept either a nickname or a #channel, depending on the command.\n"
			"\n"
			"Quick start:\n"
			"  PASS s3cr3t\n"
			"  NICK alice\n"
			"  USER alice 0 * :Alice Liddell\n"
			"  JOIN #tea\n"
			"  PRIVMSG #tea :hello world\n") {
	
}

Help::~Help() {
	
}

bool Help::load() {
	std::istringstream iss(helpContent);
	std::string line;
	while (std::getline(iss, line)) {
		full.push_back(line);
	}

	std::string currentSection = "";
	std::vector<std::string> currentContent;

	for (size_t i = 0; i < full.size(); ++i) {
		const std::string& line = full[i];

		if (!line.empty() && line[line.length() - 1] == ':' && 
				!line.empty() && std::isupper(line[0])) {

				if (!currentSection.empty()) {
					topics[currentSection] = currentContent;
				}

				currentSection = line.substr(0, line.length() - 1);
				std::transform(currentSection.begin(), currentSection.end(), 
								currentSection.begin(), ::toupper);
				currentContent.clear();
				currentContent.push_back(line);
		} else if (!currentSection.empty()) {
				currentContent.push_back(line);
		}
	}

	if (!currentSection.empty()) {
		topics[currentSection] = currentContent;
	}
	
	return true;
}

const std::vector<std::string>* Help::getTopic(const std::string& key) const
{
	std::string upperKey = key;
	std::transform(upperKey.begin(), upperKey.end(), upperKey.begin(), ::toupper);

	std::map<std::string, std::vector<std::string> >::const_iterator it = topics.find(upperKey);
	if (it != topics.end())
		return &(it->second);
	return NULL;
}

void Help::printUsage() const {
	std::cout << "Usage: ./help [topic]\n";
	std::cout << "Available topics: ";
	for (std::map<std::string, std::vector<std::string> >::const_iterator it = topics.begin(); 
			it != topics.end(); ++it) {
		if (it != topics.begin()) std::cout << ", ";
		std::cout << it->first;
	}
	std::cout << std::endl;
}

void Help::execute(Client &cli, const std::string& param) {
	(void)cli;
	cout << "Running Help command" << endl;
	
	if (param.empty()) {
		// std::cout << helpContent;
		send(cli.getFd(), helpContent.c_str(), helpContent.size(), 0);
		return;
	}
	
	const std::vector<std::string>* topicContent = getTopic(param);
	if (topicContent) {
		for (size_t i = 0; i < topicContent->size(); ++i) {
			std::cout << (*topicContent)[i] << std::endl;
		}
	} else {
		std::cout << "No help available for topic: " << param << std::endl;
		printUsage();
	}
}