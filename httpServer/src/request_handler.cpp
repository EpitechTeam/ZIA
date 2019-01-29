//
// Created by kahoul on 21/01/19.
//

#include "request_handler.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include "mime_types.hpp"
#include "reply.hpp"
#include "request.hpp"

request_handler::request_handler(const std::string &doc_root)
        : doc_root_(doc_root) {
}

void request_handler::handle_request(request &req, reply &rep, connection_ptr connection) {

    // Fire BEFORE_URL_TO_PATH Event
    this->fireEvent(BEFORE_URL_TO_PATH, req, rep, connection);

    // Decode url to path.
    std::string request_path;
    if (!url_decode(req.uri, request_path)) {
        rep = reply::stock_reply(reply::bad_request);
        return;
    }

    // Fire AFTER_URL_TO_PATH Event
    this->fireEvent(AFTER_URL_TO_PATH, req, rep, connection);


    // Request path must be absolute and not contain "..".
    if (request_path.empty() || request_path[0] != '/'
        || request_path.find("..") != std::string::npos) {
        rep = reply::stock_reply(reply::bad_request);
        return;
    }

    // If path ends in slash (i.e. is a directory) then add "index.html".
    if (request_path[request_path.size() - 1] == '/') {
        request_path += "index.html";
    }

    // Fire BEFORE_DETERMINATE_FILE_EXTENSION Event
    this->fireEvent(BEFORE_DETERMINATE_FILE_EXTENSION, req, rep, connection);

    // Determine the file extension.
    std::size_t last_slash_pos = request_path.find_last_of("/");
    std::size_t last_dot_pos = request_path.find_last_of(".");
    std::string extension;
    if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos) {
        extension = request_path.substr(last_dot_pos + 1);
    }

    // Fire AFTER_DETERMINATE_FILE_EXTENSION Event
    this->fireEvent(AFTER_DETERMINATE_FILE_EXTENSION, req, rep, connection);

    // Fire BEFORE_FILE_OPENING Event
    this->fireEvent(BEFORE_FILE_OPENING, req, rep, connection);

    // Open the file to send back.
    std::string full_path = doc_root_ + request_path;
    std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
    if (!is) {
        rep = reply::stock_reply(reply::not_found);
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
    rep.headers[1].value = mime_types::extension_to_type(extension);

    // Fire AFTER_FILL_RESPONSE Event
    this->fireEvent(AFTER_FILL_RESPONSE, req, rep, connection);

}

bool request_handler::url_decode(const std::string &in, std::string &out) {
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

void request_handler::fireEvent(const Event &event, request &req, reply &scope, connection_ptr connection) {
    this->_moduleManager.fireEvent(event, req, scope, connection);
}

int request_handler::loadModule(const std::string &path) {
    return this->_moduleManager.loadModule(path);
}