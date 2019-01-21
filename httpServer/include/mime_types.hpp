//
// Created by kahoul on 21/01/19.
//

#ifndef ZIA_MIME_TYPES_HPP
#define ZIA_MIME_TYPES_HPP

#include <string>

namespace http {
    namespace server {
        namespace mime_types {

/// Convert a file extension into a MIME type.
            std::string extension_to_type(const std::string& extension);

        } // namespace mime_types
    } // namespace server
} // namespace http

#endif //ZIA_MIME_TYPES_HPP
