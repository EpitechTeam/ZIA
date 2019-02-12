//
// Created by kahoul on 21/01/19.
//

#ifndef ZIA_REQUEST_HANDLER_HPP
#define ZIA_REQUEST_HANDLER_HPP

#include <string>

#include "moduleManager.hpp"

struct reply;
struct request;

class RequestHandler {
public:
    RequestHandler(const RequestHandler &) = delete;

    RequestHandler &operator=(const RequestHandler &) = delete;

    explicit RequestHandler(const std::string &docRoot);

    void handleRequest(request &req, reply &rep, ConnectionPtr connection);

    void fireEvent(const Event &event, request &req, reply &scope, ConnectionPtr connection);

    int loadModule(const std::string &path);

private:
    std::string _docRoot;

    static bool urlDecode(const std::string &in, std::string &out);

    ModuleManager _moduleManager;

};


#endif //ZIA_REQUEST_HANDLER_HPP
