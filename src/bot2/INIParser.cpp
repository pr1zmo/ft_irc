#include "INIParser.hpp"



std::string trim(std::string str)
{
    size_t i = 0;
    for(; i < str.size(); i++)
        if (!std::isspace(str[i]))
            break;
    size_t j = str.size();
    for (; j > i; j--)
        if (!std::isspace(str[j - 1]))
            break;
    return str.substr(i, j - i); 
}


int INIParser::load(const std::string &filename)
{
    std::ifstream file(filename.c_str());

    if (!file.is_open())
        return 1;
    std::string line;
    std::string section = "global";
    while (std::getline(file, line))
    {
        line = trim(line);
        if (line.empty())
            continue;
        if (line[0] == '[' && line[line.size() - 1] == ']')
        {
            section = trim(line.substr(1, line.size() - 2));
            continue;
        }
        
        size_t eq = line.find('=');
        if (eq == std::string::npos)
            continue;
        std::string key = trim(line.substr(0, eq));
        std::string value = trim(line.substr(eq + 1));
        _data[section][key] = value;
    }
    return 0;
}


std::string INIParser::get(const std::string& section, const std::string& key, const std::string& fallback)
{
    INIData::iterator itSec = _data.find(section);
    if (itSec == _data.end())
        return fallback; 
    Section::iterator itKey =  itSec->second.find(key);
    if (itKey == itSec->second.end())
        return fallback; 
    return  itKey->second;
}








