/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Help.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:14:09 by zelbassa          #+#    #+#             */
/*   Updated: 2025/09/23 11:40:03 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Help.cpp
#include "Commands.hpp"

bool Help::load() {
	std::string helpContent = 
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
		"  PRIVMSG #tea :hello world\n";

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

const std::vector<std::string>* Help::getTopic(const std::string& key) const {

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

// int main(int argc, char* argv[]) {
// 	Help help;
	
// 	if (!help.load()) {
// 		std::cerr << "Error: Could not load help content" << std::endl;
// 		return 1;
// 	}
	
// 	if (argc == 1) {
// 		for (size_t i = 0; i < help.full.size(); ++i)
// 			std::cout << help.full[i] << std::endl;
// 	} else if (argc == 2) {
// 		const std::vector<std::string>* topic = help.getTopic(argv[1]);
		
// 		if (topic != NULL) {
// 			for (size_t i = 0; i < topic->size(); ++i)
// 					std::cout << (*topic)[i] << std::endl;
// 		} else {
// 			std::cerr << "Error: Unknown topic '" << argv[1] << "'\n";
// 			printUsage();
// 			return 1;
// 		}
// 	} else {
// 		// More than one argument - print usage
// 		help.printUsage();
// 		return 1;
// 	}
// 	return 0;
// }