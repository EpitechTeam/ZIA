//
// Created by kahoul on 04/03/19.
//

#include <fstream>
#include <boost/filesystem.hpp>
#include "Zany/Loader.hpp"
#include "./ConfigParser.hpp"

#include "nlohmann/json.hpp"

using json = nlohmann::json;

zany::Entity ConfigParserModule::parse(std::string const &filename) {

    return this->parseConfigFile(
            filename.empty() ? "./../../config/default-config.json" : filename);
}

zany::Entity ConfigParserModule::parseConfigFile(const std::string &path) {


    if (!this->isJsonFile(path))
        return false;
    std::ifstream jsonFile(path);

    if (!jsonFile.is_open()) {
        std::cerr << "Error opening file";
        return false;
    }

    json jsonConfig = json::parse(jsonFile);
    std::cout << "ConfigMap" << std::endl;

    for (json::iterator it = jsonConfig.begin(); it != jsonConfig.end(); it++) {
        std::cout << "map[" << it.key() << "]: " << it.value() << std::endl;
    }
    return true;
}

bool
ConfigParserModule::isJsonFile(const std::string &path) const {
    return boost::filesystem::extension(path) == ".json";
}

extern "C" ZANY_DLL
        zany::Loader::AbstractModule    *entrypoint() {
    return new ConfigParserModule();
}
