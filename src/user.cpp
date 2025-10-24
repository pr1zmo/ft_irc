/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:13:29 by zelbassa          #+#    #+#             */
/*   Updated: 2025/09/21 20:13:30 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.h"

User::User(/* args */)
: Command()
{
}

User::~User()
{
}
void User::execute(Client &cli, const std::string& param, const std::string& cmd, std::map<int, Client>& clients) {
    (void)cli;
    (void)param;
    (void)cmd;
    (void)clients;
    // Implementation of USER command execution
    std::string p = param;
    std::istringstream iss(p);
    std::string username, hostname, servername, realname;
    if (!(iss >> username >> hostname >> servername)) {
        cli.response(":server 461 * USER :Not enough parameters\r\n");
        return;
    }

    std::string rest;
    if (!std::getline(iss, rest)) {
        cli.response(":server 461 * USER :Not enough parameters\r\n");
        return;
    }

    // trim leading spaces
    size_t start = rest.find_first_not_of(' ');
    if (start == std::string::npos) {
        cli.response(":server 461 * USER :Not enough parameters\r\n");
        return;
    }
    rest = rest.substr(start);

    // If realname starts with ':' take everything after it (including spaces) as one param
    if (!rest.empty() && rest[0] == ':') {
        realname = rest.substr(1);
        // strip trailing CR/LF
        while (!realname.empty() && (realname[realname.size() - 1] == '\r' || realname[realname.size() - 1] == '\n'))
            realname.erase(realname.size() - 1, 1);
    } else {
        // realname must be a single token (no spaces). If there are more words -> too many params
        std::istringstream tmp(rest);
        std::string token;
        if (!(tmp >> token)) {
            cli.response(":server 461 * USER :Not enough parameters\r\n");
            return;
        }
        std::string extra;
        if (tmp >> extra) {
            cli.response(":server 461 * USER :Too many parameters\r\n");
            return;
        }
        // strip trailing CR/LF from token
        while (!token.empty() && (token[token.size() - 1] == '\r' || token[token.size() - 1] == '\n'))
            token.erase(token.size() - 1, 1);
        realname = token;
    }

    //display extracted values
    std::cout << "Username: " << username << std::endl;
    std::cout << "Hostname: " << hostname << std::endl;
    std::cout << "Servername: " << servername << std::endl;
    std::cout << "Realname: " << realname << std::endl;
    cli.setUsername(username);
    cli.setHostname(hostname);
    cli.setServername(servername);
    cli.setRealname(realname);
    if (!cli.getNickname().empty()) {
        cli.response(":server 451 * :You have not registered\r\n");
        return;
    }
    
    clients[cli.getFd()] = cli;
}