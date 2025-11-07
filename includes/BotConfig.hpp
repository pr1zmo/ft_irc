/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BotConfig.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelbassa <zelbassa@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 09:45:44 by zelbassa          #+#    #+#             */
/*   Updated: 2025/11/07 14:23:34 by zelbassa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "IRCbot.hpp"

using namespace std;

class ConfigException : public std::exception
{
	private:
		std::string _msg;

	public:
		explicit ConfigException(const std::string &msg) : _msg(msg) {}
		virtual ~ConfigException() throw() {}
		virtual const char* what() const throw() { return _msg.c_str(); }
	};

	class FileNotFoundException : public ConfigException
	{
	public:
		explicit FileNotFoundException(const std::string &file)
			: ConfigException("Configuration file not found: " + file) {}
	};

	class ParseException : public ConfigException
	{
	public:
		explicit ParseException(const std::string &msg)
			: ConfigException("Configuration parsing error: " + msg) {}
	};

	class MissingKeyException : public ConfigException
	{
	public:
		explicit MissingKeyException(const std::string &key)
			: ConfigException("Missing required configuration key: " + key) {}
};

class BotConf {
	map<string, string> values;
	public:
		BotConf();
		~BotConf();

		void load(const string &f);

		string get(const string &key) const;
};