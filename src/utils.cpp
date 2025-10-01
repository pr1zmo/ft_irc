/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:13:24 by zelbassa          #+#    #+#             */
/*   Updated: 2025/10/01 15:51:09 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.h"

void show_help(){
	ifstream file("src/help.md");
	if (!file.is_open()) {
		cerr << "Failed to open the file." << endl;
		return ;
	}
	string line;
	while (getline(file, line)) {
		cout << line << endl;
	}
	file.close();
}

string to_string98(int n) {
	ostringstream oss;
	oss << n;
	return oss.str();
}

vector<string> split(const string &s, char delimiter) {
	vector<string> parts;
	string part;
	istringstream ss(s);
	while (getline(ss, part, delimiter)) {
		parts.push_back(part);
	}
	return parts;
}