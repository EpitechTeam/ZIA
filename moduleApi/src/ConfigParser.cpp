/*
** EPITECH PROJECT, 2021
** Zia
** File description:
** Created by monty.criel@epitech.eu,
*/

#include <ConfigParser.hpp>

#include "ConfigParser.hpp"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

ConfigParser::ConfigParser(const std::string &name) : IModule(name)
{
}

ConfigParser::~ConfigParser()
{
}

ServerConfig &ConfigParser::loadDefaultConfig()
{
	return this->parseConfigFile("./../../config/default-config.json");
}

int ConfigParser::catchEvent(const Event &event, request &req, reply &scope,
				ConnectionPtr connection)
{
	(void)connection;
	return 0;
}

ServerConfig &ConfigParser::loadFromConfigFile(std::string pathName)
{
	if (!ConfigParser::isJsonFile(pathName)) {
		return _config;
	}
	std::ifstream jsonFile(pathName);
	if (!jsonFile.is_open()) {
		std::cerr << "Error opening file";
		return _config;
	}
	json jsonConfig = json::parse(jsonFile);
	for (json::iterator it = jsonConfig.begin(); it != jsonConfig.end(); ++it) {
		_config[it.key()] = it.value();
	}
	return _config;
}

bool ConfigParser::isJsonFile(std::string &path)
{
	std::string end = ".json";

	if (path.length() > end.length()) {
		return (0 == path.compare (path.length() - end.length(), end.length(), end));
	} else {
		return false;
	}
}

ServerConfig &ConfigParser::parseConfigFile(std::string pathName)
{
	if (!ConfigParser::isJsonFile(pathName)) {
		return _config;
	}
	std::ifstream jsonFile(pathName);
	if (!jsonFile.is_open()) {
		std::cerr << "Error opening file";
		return _config;
	}
	json jsonConfig = json::parse(jsonFile);
	for (json::iterator it = jsonConfig.begin(); it != jsonConfig.end(); ++it) {
		_config[it.key()] = it.value();
	}
}



