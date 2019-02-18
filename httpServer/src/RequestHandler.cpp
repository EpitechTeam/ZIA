//
// Created by kahoul on 21/01/19.
//

#include "RequestHandler.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include "mime_types.hpp"
#include "reply.hpp"
#include "request.hpp"

RequestHandler::RequestHandler(const std::string &docRoot)
        : _docRoot(docRoot) {
}

void RequestHandler::handleRequest(request &req, reply &rep, ConnectionPtr connection) {

    req.docRoot = this->_docRoot;
    // Fire BEFORE_URL_TO_PATH Event
    this->fireEvent(BEFORE_URL_TO_PATH, req, rep, connection);

    // Decode url to path.
    if (!this->urlDecode(req.uri, req.url)) {
        rep = reply::stockReply(reply::bad_request);
        return;
    }

    // Fire AFTER_URL_TO_PATH Event
    this->fireEvent(AFTER_URL_TO_PATH, req, rep, connection);


    // Request path must be absolute and not contain "..".
    if (req.url.empty() || req.url[0] != '/'
        || req.url.find("..") != std::string::npos) {
        rep = reply::stockReply(reply::bad_request);
        return;
    }

    // If path ends in slash (i.e. is a directory) then add "index.html".
    if (req.url[req.url.size() - 1] == '/') {
        req.url += "index.html";
    }

    // Fire BEFORE_DETERMINATE_FILE_EXTENSION Event
    this->fireEvent(BEFORE_DETERMINATE_FILE_EXTENSION, req, rep, connection);

    std::size_t lastSlashPos = req.url.find_last_of("/");
    std::size_t lastDotPos = req.url.find_last_of(".");
    if (lastDotPos != std::string::npos && lastDotPos > lastSlashPos) {
        req.extension = req.url.substr(lastDotPos + 1);
    }

    // Fire AFTER_DETERMINATE_FILE_EXTENSION Event
    this->fireEvent(AFTER_DETERMINATE_FILE_EXTENSION, req, rep, connection);

    // Fire BEFORE_FILE_OPENING Event
    this->fireEvent(BEFORE_FILE_OPENING, req, rep, connection);

    // Open the file to send back.
    req.fullPath = this->_docRoot + req.url;
    std::ifstream is(req.fullPath.c_str(), std::ios::in | std::ios::binary);
    if (!is) {
        rep = reply::stockReply(reply::not_found);
        return;
    }

    // Fire AFTER_FILE_OPENING Event
    this->fireEvent(AFTER_FILE_OPENING, req, rep, connection);

    // Fire BEFORE_FILL_RESPONSE Event
    this->fireEvent(BEFORE_FILL_RESPONSE, req, rep, connection);

    // Fill out the reply to be sent to the client.
    rep.status = reply::ok;
    char buf[512];
    while (is.read(buf, sizeof(buf)).gcount() > 0)
        rep.content.append(buf, is.gcount());
    rep.headers.resize(2);
    rep.headers[0].name = "Content-Length";
    rep.headers[0].value = std::to_string(rep.content.size());
    rep.headers[1].name = "Content-Type";
    rep.headers[1].value = mime_types::extensionToType(req.extension);

    // Fire AFTER_FILL_RESPONSE Event
    this->fireEvent(AFTER_FILL_RESPONSE, req, rep, connection);

}

bool RequestHandler::urlDecode(const std::string &in, std::string &out) {
    out.clear();
    out.reserve(in.size());
    for (std::size_t i = 0; i < in.size(); ++i) {
        if (in[i] == '%') {
            if (i + 3 <= in.size()) {
                int value = 0;
                std::istringstream is(in.substr(i + 1, 2));
                if (is >> std::hex >> value) {
                    out += static_cast<char>(value);
                    i += 2;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        } else if (in[i] == '+') {
            out += ' ';
        } else {
            out += in[i];
        }
    }
    return true;
}

void RequestHandler::fireEvent(const Event &event, request &req, reply &scope, ConnectionPtr connection) {
    this->_moduleManager.fireEvent(event, req, scope, connection);
}

int RequestHandler::loadModule(const std::string &path) {
    return this->_moduleManager.loadModule(path);
}