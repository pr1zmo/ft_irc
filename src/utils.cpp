/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 20:13:24 by zelbassa          #+#    #+#             */
/*   Updated: 2025/09/21 20:13:25 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.h"

void show_help(){
	std::ifstream file("src/help.md");
	if (!file.is_open()) {
		std::cerr << "Failed to open the file." << std::endl;
		return ;
	}
	std::string line;
	while (std::getline(file, line)) {
		std::cout << line << std::endl;
	}
	file.close();
}