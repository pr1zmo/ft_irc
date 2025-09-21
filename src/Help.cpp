/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Help.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:14:09 by zelbassa          #+#    #+#             */
/*   Updated: 2025/09/21 21:00:55 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Help.cpp
#include "Commands.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

std::string findLast(const std::string &str, char delimiter) {
	int i = 0;
	while (i < static_cast<int>(str.size()) && str[i] == delimiter)
		i++;
	int j = str.size() - 1;
	while (j >= 0 && str[j] == delimiter)
		j--;
	if (j < i)
		return "";
	return str.substr(i, j - i + 1);
}

bool Help::load(){
	std::string path = "help.md";
	std::ifstream file(path);
	if (!file.is_open()){
		std::string fileName = path.substr(findLast(path, '/').empty() ? 0 : path.find_last_of('/') + 1);
		std::cerr << "Error: Failed to open file: " << fileName << std::endl; 
		return false;
	}

	std::ostringstream oss;
	oss << file.rdbuf();
	full = oss.str();
	
	std::string temp;
	while (std::getline(file, temp)){
		std::string first_word = temp.substr(temp.find(" "));
	}
	return true;
}

const std::vector<std::string>* Help::getTopic(const std::string& key) const {
	
}