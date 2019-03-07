//
// Created by kahoul on 04/03/19.
//

#include "./ConfigParser.hpp"
#include "Utils.hpp"

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

zany::Entity ConfigParserModule::parse(std::string const &filename) {


    try {
        if (!this->isJsonFile(filename)) {
            std::cerr << "Error: "  << filename << "is not a json file." << std::endl;
            return false;

        }
        std::ifstream jsonFile(filename);

        if (!jsonFile.is_open()) {
            std::cerr << "Error when try to open " << filename << "." << std::endl;
            return false;
        }
        json jsonConfig;
        jsonFile >> jsonConfig;

        this->_obj = this->fromJson(jsonConfig);

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
