//
// Created by kahoul on 21/01/19.
//

#include "RequestParser.hpp"
#include "request.hpp"

RequestParser::RequestParser()
        : _state(method_start) {
}

void RequestParser::reset() {
    this->_state = method_start;
}

RequestParser::resultType RequestParser::consume(request &req, char input) {
    switch (this->_state) {
        case method_start:
            if (!isChar(input) || isCtl(input) || isTspecial(input)) {
                return bad;
            } else {
                this->_state = method;
                req.method.push_back(input);
                return indeterminate;
            }
        case method:
            if (input == ' ') {
                this->_state = uri;
                return indeterminate;
            } else if (!isChar(input) || isCtl(input) || isTspecial(input)) {
                return bad;
            } else {
                req.method.push_back(input);
                return indeterminate;
            }
        case uri:
            if (input == ' ') {
                this->_state = http_version_h;
                return indeterminate;
            } else if (isCtl(input)) {
                return bad;
            } else {
                req.uri.push_back(input);
                return indeterminate;
            }
        case http_version_h:
            if (input == 'H') {
                this->_state = http_version_t_1;
                return indeterminate;
            } else {
                return bad;
            }
        case http_version_t_1:
            if (input == 'T') {
                this->_state = http_version_t_2;
                return indeterminate;
            } else {
                return bad;
            }
        case http_version_t_2:
            if (input == 'T') {
                this->_state = http_version_p;
                return indeterminate;
            } else {
                return bad;
            }
        case http_version_p:
            if (input == 'P') {
                this->_state = http_version_slash;
                return indeterminate;
            } else {
                return bad;
            }
        case http_version_slash:
            if (input == '/') {
                req.http_version_major = 0;
                req.http_version_minor = 0;
                this->_state = http_version_major_start;
                return indeterminate;
            } else {
                return bad;
            }
        case http_version_major_start:
            if (isDigit(input)) {
                req.http_version_major = req.http_version_major * 10 + input - '0';
                this->_state = http_version_major;
                return indeterminate;
            } else {
                return bad;
            }
        case http_version_major:
            if (input == '.') {
                this->_state = http_version_minor_start;
                return indeterminate;
            } else if (isDigit(input)) {
                req.http_version_major = req.http_version_major * 10 + input - '0';
                return indeterminate;
            } else {
                return bad;
            }
        case http_version_minor_start:
            if (isDigit(input)) {
                req.http_version_minor = req.http_version_minor * 10 + input - '0';
                this->_state = http_version_minor;
                return indeterminate;
            } else {
                return bad;
            }
        case http_version_minor:
            if (input == '\r') {
                this->_state = expecting_newline_1;
                return indeterminate;
            } else if (isDigit(input)) {
                req.http_version_minor = req.http_version_minor * 10 + input - '0';
                return indeterminate;
            } else {
                return bad;
            }
        case expecting_newline_1:
            if (input == '\n') {
                this->_state = header_line_start;
                return indeterminate;
            } else {
                return bad;
            }
        case header_line_start:
            if (input == '\r') {
                this->_state = expecting_newline_3;
                return indeterminate;
            } else if (!req.headers.empty() && (input == ' ' || input == '\t')) {
                this->_state = header_lws;
                return indeterminate;
            } else if (!isChar(input) || isCtl(input) || isTspecial(input)) {
                return bad;
            } else {
                req.headers.push_back(header());
                req.headers.back().name.push_back(input);
                this->_state = header_name;
                return indeterminate;
            }
        case header_lws:
            if (input == '\r') {
                this->_state = expecting_newline_2;
                return indeterminate;
            } else if (input == ' ' || input == '\t') {
                return indeterminate;
            } else if (isCtl(input)) {
                return bad;
            } else {
                this->_state = header_value;
                req.headers.back().value.push_back(input);
                return indeterminate;
            }
        case header_name:
            if (input == ':') {
                this->_state = space_before_header_value;
                return indeterminate;
            } else if (!isChar(input) || isCtl(input) || isTspecial(input)) {
                return bad;
            } else {
                req.headers.back().name.push_back(input);
                return indeterminate;
            }
        case space_before_header_value:
            if (input == ' ') {
                this->_state = header_value;
                return indeterminate;
            } else {
                return bad;
            }
        case header_value:
            if (input == '\r') {
                this->_state = expecting_newline_2;
                return indeterminate;
            } else if (isCtl(input)) {
                return bad;
            } else {
                req.headers.back().value.push_back(input);
                return indeterminate;
            }
        case expecting_newline_2:
            if (input == '\n') {
                this->_state = header_line_start;
                return indeterminate;
            } else {
                return bad;
            }
        case expecting_newline_3:
            if (input == '\n') {
                this->_state = param_line_start;
                return after_header;
            } else {
                return bad;
            }
        case param_line_start:
            if (!isChar(input) || isCtl(input) || isTspecial(input)) {
                return bad;
            } else {
                req.params.push_back(param());
                req.params.back().name.push_back(input);
                this->_state = param_name;
                return after_header;
            }
        case param_name:
            if (input == '=') {
                this->_state = param_value;
                return indeterminate;
            } else if (!isChar(input) || isCtl(input) || isTspecial(input)) {
                return bad;
            } else {
                req.params.back().name.push_back(input);
                return after_header;
            }
        case param_value:
            if (input == '&') {
                this->_state = param_line_start;
                return indeterminate;
            } else if (!isChar(input) || isCtl(input) || isTspecial(input)) {
                return bad;
            } else {
                req.params.back().value.push_back(input);
                return after_header;
            }
        default:
            return bad;
    }
}

bool RequestParser::isChar(int c) {
    return c >= 0 && c <= 127;
}

bool RequestParser::isCtl(int c) {
    return (c >= 0 && c <= 31) || (c == 127);
}

bool RequestParser::isTspecial(int c) {
    switch (c) {
        case '(':
        case ')':
        case '<':
        case '>':
        case '@':
        case ',':
        case ';':
        case ':':
        case '\\':
        case '"':
        case '/':
        case '[':
        case ']':
        case '?':
        case '=':
        case '{':
        case '}':
        case ' ':
        case '\t':
            return true;
        default:
            return false;
    }
}

bool RequestParser::isDigit(int c) {
    return c >= '0' && c <= '9';
}
