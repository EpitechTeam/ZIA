//
// Created by kahoul on 21/01/19.
//

#ifndef ZIA_APPENDMODULE_HPP
#define ZIA_APPENDMODULE_HPP

#include "IModule.hpp"

class AppendModule: public IModule {
public:
    AppendModule() : IModule("AppendModule") {

    }
    ~AppendModule() = default;

    int catchEvent(const Event &event, request &req, reply &scope, connection_ptr connection) {
        switch (event) {
            case AFTER_FILL_RESPONSE:
                std::cout << "AFTER_FILL_RESPONSE catched by AppendModule." << std::endl;
                scope.content.append("<p> Hello World </p>\n");
                scope = reply::stock_reply(reply::not_found);
                break;
            default:
                break;
        }
        return 0;
    }
};

#endif //R_TYPE_APPENDMODULE_HPP
