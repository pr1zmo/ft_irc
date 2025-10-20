/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:13:57 by zelbassa          #+#    #+#             */
/*   Updated: 2025/09/21 20:13:58 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cctype>
#include <algorithm>
#include <sstream>
#include <string>
#include <map>
#include "ft_irc.h"

Nick::Nick(/* args */)
: Command()
{
}

Nick::~Nick()
{
}




static inline std::string trim(const std::string &s) {
    size_t a = 0;
    while (a < s.size() && isspace((unsigned char)s[a])) ++a;
    size_t b = s.size();
    while (b > a && isspace((unsigned char)s[b-1])) --b;
    return s.substr(a, b - a);
}

static inline std::string toLowerAscii(const std::string &s) {
    std::string r = s;
    for (size_t i = 0; i < r.size(); ++i) r[i] = (char)tolower((unsigned char)r[i]);
    return r;
}

void Nick::execute(Client &client, const std::string &params, const std::string &cmd, std::map<int, Client>& clients)
{

    std::string p = trim(params);
    std::istringstream iss(p);
    std::string new_nick;
    if (!(iss >> new_nick)) {
        client.response(":server 431 * :No nickname given\r\n"); 
        return;
    }

    const size_t MAX_NICK = 30;
    if (new_nick.empty() || new_nick.size() > MAX_NICK) {
        client.response(":server 432 " + new_nick + " :Erroneous nickname\r\n");
        return;
    }

    // first char not digit, allowed chars - letters, digits, underscore, dash
    for (size_t i = 0; i < new_nick.size(); ++i) {
        unsigned char c = (unsigned char)new_nick[i];
        if (i == 0) {
            if (isdigit(c)) {
                client.response(":server 432 " + new_nick + " :Erroneous nickname\r\n");
                return;
            }
            if (!isalpha(c) && c != '_' && c != '[' && c != ']' && c != '\\' && c != '^' && c != '{' && c != '|' && c != '}') {
                client.response(":server 432 " + new_nick + " :Erroneous nickname\r\n");
                return;
            }
        } else {
            if (!isalnum(c) && c != '_' && c != '-' && c != '[' && c != ']' && c != '\\' && c != '^' && c != '{' && c != '|' && c != '}') {
                client.response(":server 432 " + new_nick + " :Erroneous nickname\r\n");
                return;
            }
        }
    }

    // case-insensitive
    std::string key = toLowerAscii(new_nick);
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->first == client.getFd()) continue; // skip self
        if (toLowerAscii(it->second.getNickname()) == key) {
            client.response(":server 433 * " + new_nick + " :Nickname is already in use\r\n");
            return;
        }
    }

    // update nickname on the stored client object in the map (find by fd)
    std::map<int, Client>::iterator me = clients.find(client.getFd());
    if (me != clients.end()) {
        me->second.setNickname(new_nick);

    } else {
        // fallback: set on given client
        client.setNickname(new_nick);
        if (cli)
    }

}









// void Nick::execute(Client &client, const std::string &params,    const std::string &cmd, std::map<int, Client>& clients)
// {
//     (void)client;
//     (void)params;
//     // Implementation of NICK command execution
//     std::string new_nick = params;
//     std::cout << "Setting nickname to: " << new_nick << std::endl;
//     std::cout << "client FD: " << client.getFd() << std::endl;
//     // parse the nickname 
//     for (size_t i = 0; i < new_nick.size(); ++i) {
//         char c = new_nick[i];
//         if (!isalnum(c) && c != '_') {
//             client.response("432 " + new_nick + " :Erroneous nickname\r\n");
//             return;
//         }
//         if (i == 0 && isdigit(c)) {
//             client.response("432 " + new_nick + " :Erroneous nickname\r\n");
//             return;
//         }
//     }

//     // check if nickname is already used
//     for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
//         if (it->second.getNickname() == new_nick) {
//             client.response("433 " + new_nick + " :Nickname is already in use\r\n");
//             return;
//         }
//     }
//     client.setNickname(new_nick);
//     clients[client.getFd()] = client;
// }
