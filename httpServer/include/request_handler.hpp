//
// Created by kahoul on 21/01/19.
//

#ifndef ZIA_REQUEST_HANDLER_HPP
#define ZIA_REQUEST_HANDLER_HPP

#include <string>

#include "moduleManager.hpp"

struct reply;
struct request;

/// The common handler for all incoming requests.
class request_handler {
public:
    request_handler(const request_handler &) = delete;

    request_handler &operator=(const request_handler &) = delete;

    /// Construct with a directory containing files to be served.
    explicit request_handler(const std::string &doc_root);

    /// Handle a request and produce a reply.
    void handle_request(request &req, reply &rep);

    /// Fire Event
    void fireEvent(const Event &event, request &req, reply &scope);

    int loadModule(const std::string &path);

private:
    /// The directory containing the files to be served.
    std::string doc_root_;

    /// Perform URL-decoding on a string. Returns false if the encoding was
    /// invalid.
    static bool url_decode(const std::string &in, std::string &out);

    /// moduleManager
    ModuleManager _moduleManager;

};


#endif //ZIA_REQUEST_HANDLER_HPP
