#ifndef INIPARSER_HPP
#define INIPARSER_HPP

#include <iostream>
#include <map>
#include <string>
#include <fstream>

typedef std::map<std::string, std::string> Section;
typedef std::map<std::string, Section> INIData;

std::string trim(std::string str);
class INIParser
{

    INIData _data;

    public:
        int load(const std::string &filename);
        std::string get(const std::string& section, const std::string& key, const std::string &fallback);
        // std::string& get(const std::string &section, const std::string &key) const;
};

#endif