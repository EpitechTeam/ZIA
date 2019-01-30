//
// Created by kahoul on 21/01/19.
//

#ifndef ZIA_PHPCGIMODULE_HPP
#define ZIA_PHPCGIMODULE_HPP

#include "IModule.hpp"

class PhpCgiModule: public IModule {
public:
    PhpCgiModule() : IModule("PhpCgiModule") {

    }
    ~PhpCgiModule() = default;

    int catchEvent(const Event &event, request &req, reply &scope) {
        switch (event) {
            case AFTER_FILL_RESPONSE:
                std::cout << "AFTER_FILL_RESPONSE catched by AppendModule." << std::endl;
                break;
            default:
                break;
        }
        return 0;
    }
};

#endif //R_TYPE_PHPCGIMODULE_HPP
