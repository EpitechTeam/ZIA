//
// Created by kahoul on 21/01/19.
//

#ifndef ZIA_IMODULE_HPP
#define ZIA_IMODULE_HPP

#include <iostream>

#include "reply.hpp"
#include "moduleManager.hpp"

class IModule {
public:
    IModule(const std::string &name) : _name(name) {

    }

    IModule() : _name("UNDEFINED") {

    }

    virtual ~IModule() = default;

    virtual int catchEvent(const Event &event, request &req, reply &scope, ConnectionPtr connection) = 0;

    std::string getName() const {
        return (this->_name);
    }

private:

    std::string _name;
};

#endif //R_TYPE_IMODULE_HPP
