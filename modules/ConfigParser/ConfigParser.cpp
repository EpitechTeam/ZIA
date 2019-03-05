//
// Created by kahoul on 04/03/19.
//

#include "./ConfigParser.hpp"
#include "Utils.hpp"

zany::Entity ConfigParserModule::parse(std::string const &filename) {

    return this->parseConfigFile(
            filename.empty() ? "./../../config/default-config.json" : filename);
}

zany::Entity ConfigParserModule::fromJson(json object) {

    if (object.is_object()) {

        zany::Entity obj(zany::Entity::Type::OBJ);

        for (auto &it : object.items()) {
            obj[it.key()] = this->fromJson(it.value());
        }
        return obj;
    } else if (object.is_array()) {
        zany::Entity array(zany::Entity::Type::ARR);

        for (auto &it : object.items()) {
            array.push(this->fromJson(it.value()));
        }
        return array;
    } else if (object.is_number()) {
        return object.get<int>();

    } else if (object.is_string()) {
        return object.get<std::string>();

    } else if (object.is_boolean()) {
        return object.get<bool>();

    }
    return "NULL";
}

zany::Entity ConfigParserModule::parseConfigFile(const std::string &path) {


    try {
        if (!this->isJsonFile(path))
            return false;
        std::ifstream jsonFile(path);

        if (!jsonFile.is_open()) {
            std::cerr << "Error opening file";
            return false;
        }
        json jsonConfig;
        jsonFile >> jsonConfig;

        this->_obj = this->fromJson(jsonConfig);

        std::cout << "\n==============================" << std::endl;
        std::cout << "||      CONFIG PARSING      ||" << std::endl;
        std::cout << "==============================\n" << std::endl;

        Utils::printEntity(this->_obj);

        std::cout << "\n==============================\n" << std::endl;

        return this->_obj;
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

bool
ConfigParserModule::isJsonFile(const std::string &path) const {
    return boost::filesystem::extension(path) == ".json";
}

extern "C" ZANY_DLL
        zany::Loader::AbstractModule    *entrypoint() {
    return new ConfigParserModule();
}
