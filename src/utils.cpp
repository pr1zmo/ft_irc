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