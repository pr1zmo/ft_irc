<<<<<<< HEAD



#include <iostream>
#include "INIParser.hpp"

=======
#include <iostream>
#include "INIParser.hpp"
>>>>>>> 546865a6eb5ab010618260e237c9889e34a29b11
#include "IRCBot.hpp"


int main(int ac, char **av)
{
<<<<<<< HEAD
	if (ac > 1)
	{
		INIParser ini; 
		ini.load(av[1]);
		try {
			IRCBot bot(ini);
			bot.run();
		} catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}   
	}
	else
		std::cerr << "Config required" << std::endl;

	return 0;
=======
    if (ac > 1)
    {
        handle_signals();
        INIParser ini; 
        ini.load(av[1]);
        try {
            IRCBot bot(ini);
            bot.run();
        } catch (std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }   
    }
    else
        std::cerr << "Config required" << std::endl;

    return 0;
>>>>>>> 546865a6eb5ab010618260e237c9889e34a29b11
}