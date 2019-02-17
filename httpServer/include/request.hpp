//
// Created by kahoul on 21/01/19.
//

#ifndef ZIA_REQUEST_HPP
#define ZIA_REQUEST_HPP

#include <string>
#include <vector>
#include "header.hpp"
#include "param.hpp"

struct request {
    std::string method;
    std::string uri;
    int http_version_major;
    int http_version_minor;
    std::vector <header> headers;
    std::vector <param> params;
    std::string query;
};

#endif //ZIA_REQUEST_HPP
