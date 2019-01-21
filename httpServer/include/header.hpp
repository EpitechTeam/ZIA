//
// Created by kahoul on 21/01/19.
//

#ifndef ZIA_HEADER_HPP
#define ZIA_HEADER_HPP

#include <string>

namespace http {
    namespace server {

        struct header
        {
            std::string name;
            std::string value;
        };

    } // namespace server
} // namespace http

#endif //ZIA_HEADER_HPP
