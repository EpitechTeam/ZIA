//
// Created by kahoul on 21/01/19.
//

#ifndef ZIA_MIME_TYPES_HPP
#define ZIA_MIME_TYPES_HPP

#include <string>

namespace mime_types {

/// Convert a file extension into a MIME type.
    std::string extension_to_type(const std::string &extension);

} // namespace mime_types

#endif //ZIA_MIME_TYPES_HPP
