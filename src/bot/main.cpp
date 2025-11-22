


#include <iostream>
#include "INIParser.hpp"

#include "IRCBot.hpp"


int main(int ac, char **av)
{
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
}