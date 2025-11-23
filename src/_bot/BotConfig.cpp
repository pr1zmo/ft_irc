/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BotConfig.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/20 19:00:37 by zelbassa          #+#    #+#             */
/*   Updated: 2025/11/20 19:00:38 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BotConfig.hpp"
#include <filesystem>

BotConf::BotConf() {
}

BotConf::~BotConf() {
}

static string to_string98(int n) {
	ostringstream oss;
	oss << n;
	return oss.str();
}

void	BotConf::load(const string &filename){
	BotConf conf;
	string confDir = "./config/" + filename;
	ifstream f(confDir.c_str());

	if (!f.is_open())
		throw ConfigException("Cannot open configuration file: " + filename);
	
	string line;
	size_t lineNum = 0;
	
	while (getline(f, line))
	{
		++lineNum;

		// Trim whitespace
		size_t start = line.find_first_not_of(" \t\r\n");
		if (start == string::npos || line[start] == '#')
			continue; // empty or comment line

		size_t eq = line.find('=');
		if (eq == string::npos)
			throw ConfigException("Invalid line (missing '=') at line " + to_string98(lineNum));

		string key = line.substr(0, eq);
		string value = line.substr(eq + 1);

		// Trim both sides
		while (!key.empty() && isspace(key[key.size()-1])) key.erase(key.size()-1);
		while (!value.empty() && isspace(value[0])) value.erase(0,1);

		values[key] = value;
	}
	f.close();
}

string BotConf::get(const string &key) const {
	return values.at(key);
}

bool BotConf::has(const string &key) const {
	return values.find(key) != values.end();
}