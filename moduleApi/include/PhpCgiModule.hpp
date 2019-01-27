//
// Created by kahoul on 21/01/19.
//

#ifndef ZIA_PHPCGIMODULE_HPP
#define ZIA_PHPCGIMODULE_HPP

#include "IModule.hpp"


void print_request(request &req) {
    std::cout << "\033[1;31m [" << req.method << "] \033[0m";
    std::cout << "\033[1;37m " << req.uri << " \033[0m\n";
    std::cout << "\033[1;31m major: " << req.http_version_major << " \033[0m";
    std::cout << "\033[1;31m minor: " << req.http_version_minor << " \033[0m\n";
    for(unsigned int i = 0; req.headers.size() != i; i++) {
        std::cout << "\033[1;32m" << req.headers[i].name << " : \033[0m";
        std::cout << "\033[1;37m" << req.headers[i].value << "\033[0m" << std::endl;
    }
}

class PhpCgiModule: public IModule {
public:
    PhpCgiModule() : IModule("PhpCgiModule") {

    }
    ~PhpCgiModule() = default;

    int catchEvent(const Event &event, request &req, reply &scope) {
        switch (event) {
            case AFTER_FILL_RESPONSE:
                print_request(req);
                std::cout << "AFTER_FILL_RESPONSE catched by AppendModule." << std::endl;
                break;
            default:
                print_request(req);
                std::cout << "event" << std::endl;
                break;
        }
        return 0;
    }
};

/*
struct request {
    std::string method;
    std::string uri;
    int http_version_major;
    int http_version_minor;
    std::vector <header> headers;
};*/
#endif //R_TYPE_PHPCGIMODULE_HPP
