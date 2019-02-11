//
// Created by kahoul on 21/01/19.
//

#include <stdlib.h>
#include <map>

#include "moduleManager.hpp"
#include "PhpCgiModule.hpp"
#include <boost/filesystem.hpp>

int
ModuleManager::loadModule(const std::string &path) {


    /* std::string fileName;

     std::size_t len = path.find_last_of('/');
     if (len != std::string::npos) {
         fileName = path.substr(len);
     } else {
         fileName = path;
     }

     std::ifstream ifs(fileName);

     if (!ifs.is_open()) {
         std::cout << "Invalid Path." << std::endl;
         return (1);
     }*/

    for (auto &it : this->_modules) {
        if (it->getName() == "AppendModule" /*path*/ ) {
            std::cout << "Module already loaded." << std::endl;
            return (1);
        }
    }

    this->_modules.push_back(new PhpCgiModule);
    std::cout << "Module PhpCgiModule loaded." << std::endl;
    return (0);
}

int
ModuleManager::unloadModule(const std::string &name) {

    return (0);
}

int
ModuleManager::fireEvent(const Event &event, request &req, reply &scope, ConnectionPtr connection) {
    for (auto &it : this->_modules) {
        it->catchEvent(event, req, scope, connection);
    }
    return (0);
}
