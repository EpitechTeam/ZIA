//
// Created by kahoul on 21/01/19.
//

#ifndef ZIA_MODULEMANAGER_HPP
#define ZIA_MODULEMANAGER_HPP

#include <iostream>
#include <fstream>

#include <list>
#include <functional>

#include "reply.hpp"
#include "request.hpp"

class Connection;
typedef std::shared_ptr <Connection> ConnectionPtr;

class IModule;

enum Event {
    BEFORE_CONNECTION,
    AFTER_CONNECTION,
    BEFORE_URL_TO_PATH,
    AFTER_URL_TO_PATH,
    BEFORE_DETERMINATE_FILE_EXTENSION,
    AFTER_DETERMINATE_FILE_EXTENSION,
    BEFORE_FILE_OPENING,
    AFTER_FILE_OPENING,
    BEFORE_FILL_RESPONSE,
    AFTER_FILL_RESPONSE
};

class ModuleManager {
public:
    ModuleManager() = default;

    int loadModule(const std::string &path);

    int unloadModule(const std::string &name);

    int fireEvent(const Event &event, request &req, reply &scope, ConnectionPtr connection);

private:

    std::list<IModule*> _modules;
};

#endif //R_TYPE_MODULEMANAGER_HPP
