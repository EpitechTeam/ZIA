/*
** EPITECH PROJECT, 2021
** Zia
** File description:
** Created by monty.criel@epitech.eu,
*/

#ifndef ZIA_CONFIGPARSER_HPP
#define ZIA_CONFIGPARSER_HPP

#include "IModule.hpp"

typedef std::map<std::string, std::string>	ServerConfig;

class ConfigParser : public IModule {
	public:
	    ConfigParser(const std::string &name);
	    ~ConfigParser();

	    int catchEvent(const Event &event, request &req, reply &scope,
	    			ConnectionPtr connection);
	    bool 		isJsonFile(std::string &path);
	    ServerConfig	&loadDefaultConfig();
	    ServerConfig	&loadFromConfigFile(std::string pathName);
	    ServerConfig	&parseConfigFile(std::string pathName);

	private:
	    ServerConfig	_config;
};

#endif /* ZIA_CONFIGPARSER_HPP */
