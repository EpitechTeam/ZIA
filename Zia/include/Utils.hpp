//
// Created by kahoul on 02/03/19.
//

#ifndef ZIA_UTILS_HPP
#define ZIA_UTILS_HPP

#include "Zany/HttpBase.hpp"
#include "Zany/Pipeline.hpp"

#include <fstream>

#define OK 200
#define CREATED 201
#define ACCEPTED 202
#define NO_CONTENT 204
#define MULTIPLES_CHOICES 300
#define MOVED_PERMANENTLY 301
#define MOVED_TEMPORARILY 302
#define NOT_MODIFIED 304
#define BAD_REQUEST 400
#define UNAUTHORIZED 401
#define FORBIDDEN 403
#define NOT_FOUND 404
#define INTERNAL_SERVER_ERROR 500
#define NOT_IMPLEMENTED 501
#define BAD_GATEWAY 502
#define SERVICE_UNAVAILABLE 503

namespace reply {

    const char ok[] = "";
    const char created[] =
            "<html>"
            "<head><title>Created</title></head>"
            "<body><h1>201 Created</h1></body>"
            "</html>";
    const char accepted[] =
            "<html>"
            "<head><title>Accepted</title></head>"
            "<body><h1>202 Accepted</h1></body>"
            "</html>";
    const char no_content[] =
            "<html>"
            "<head><title>No Content</title></head>"
            "<body><h1>204 Content</h1></body>"
            "</html>";
    const char multiple_choices[] =
            "<html>"
            "<head><title>Multiple Choices</title></head>"
            "<body><h1>300 Multiple Choices</h1></body>"
            "</html>";
    const char moved_permanently[] =
            "<html>"
            "<head><title>Moved Permanently</title></head>"
            "<body><h1>301 Moved Permanently</h1></body>"
            "</html>";
    const char moved_temporarily[] =
            "<html>"
            "<head><title>Moved Temporarily</title></head>"
            "<body><h1>302 Moved Temporarily</h1></body>"
            "</html>";
    const char not_modified[] =
            "<html>"
            "<head><title>Not Modified</title></head>"
            "<body><h1>304 Not Modified</h1></body>"
            "</html>";
    const char bad_request[] =
            "<html>"
            "<head><title>Bad Request</title></head>"
            "<body><h1>400 Bad Request</h1></body>"
            "</html>";
    const char unauthorized[] =
            "<html>"
            "<head><title>Unauthorized</title></head>"
            "<body><h1>401 Unauthorized</h1></body>"
            "</html>";
    const char forbidden[] =
            "<html>"
            "<head><title>Forbidden</title></head>"
            "<body><h1>403 Forbidden</h1></body>"
            "</html>";
    const char not_found[] =
            "<html>"
            "<head><title>Not Found</title></head>"
            "<body><h1>404 Not Found</h1></body>"
            "</html>";
    const char internal_server_error[] =
            "<html>"
            "<head><title>Internal Server Error</title></head>"
            "<body><h1>500 Internal Server Error</h1></body>"
            "</html>";
    const char not_implemented[] =
            "<html>"
            "<head><title>Not Implemented</title></head>"
            "<body><h1>501 Not Implemented</h1></body>"
            "</html>";
    const char bad_gateway[] =
            "<html>"
            "<head><title>Bad Gateway</title></head>"
            "<body><h1>502 Bad Gateway</h1></body>"
            "</html>";
    const char service_unavailable[] =
            "<html>"
            "<head><title>Service Unavailable</title></head>"
            "<body><h1>503 Service Unavailable</h1></body>"
            "</html>";

    std::string to_string(unsigned int status) {
        switch (status) {
            case OK:
                return ok;
            case CREATED:
                return created;
            case ACCEPTED:
                return accepted;
            case NO_CONTENT:
                return no_content;
            case MULTIPLES_CHOICES:
                return multiple_choices;
            case MOVED_PERMANENTLY:
                return moved_permanently;
            case MOVED_TEMPORARILY:
                return moved_temporarily;
            case NOT_MODIFIED:
                return not_modified;
            case BAD_REQUEST:
                return bad_request;
            case UNAUTHORIZED:
                return unauthorized;
            case FORBIDDEN:
                return forbidden;
            case NOT_FOUND:
                return not_found;
            case INTERNAL_SERVER_ERROR:
                return internal_server_error;
            case NOT_IMPLEMENTED:
                return not_implemented;
            case BAD_GATEWAY:
                return bad_gateway;
            case SERVICE_UNAVAILABLE:
                return service_unavailable;
            default:
                return internal_server_error;
        }
    }
}

namespace Utils {

    static auto getMethodFromString(std::string &method) {
        if (method == "GET") {
            return zany::HttpRequest::RequestMethods::GET;
        } else if (method == "POST") {
            return zany::HttpRequest::RequestMethods::POST;
        } else if (method == "HEAD") {
            return zany::HttpRequest::RequestMethods::HEAD;
        } else if (method == "OPTION") {
            return zany::HttpRequest::RequestMethods::OPTIONS;
        } else if (method == "PATCH") {
            return zany::HttpRequest::RequestMethods::PATCH;
        } else if (method == "CONNECT") {
            return zany::HttpRequest::RequestMethods::CONNECT;
        } else if (method == "PUT") {
            return zany::HttpRequest::RequestMethods::PUT;
        } else if (method == "TRACE") {
            return zany::HttpRequest::RequestMethods::TRACE;
        } else if (method == "DELETE") {
            return zany::HttpRequest::RequestMethods::DELETE;
        }
        return zany::HttpRequest::RequestMethods::ERROR;
    }

    void _writeReponseHeader(zany::Pipeline::Instance &i) {

        auto &istream = i.connection->stream();

        istream << i.response.protocol << '/' << i.response.protocolVersion
                << ' ' << i.response.status << ' ' << reply::to_string(i.response.status);

        for (auto &header : i.response.headers) {
            istream << "\r\n" << header.first << ": " << *header.second;
        }
        istream << "\r\n";

    }

    void writeResponse(zany::Pipeline::Instance &i, const std::string &data) {
        _writeReponseHeader(i);

        i.connection->stream() << "\r\n" << data << "\r\n";
    }

    void printPipelineContent(zany::Pipeline::Instance &i) {

        std::cout << "\n==================================================" << std::endl;
        std::cout << "REQUEST" << std::endl;
        std::cout << "  MethodString: " << i.request.methodString << std::endl;
        std::cout << "  Host: " << i.request.host << std::endl;
        std::cout << "  Port: " << i.request.port << std::endl;
        std::cout << "  Path: " << i.request.path << std::endl;
        std::cout << "  FullQuery: " << i.request.fullQuery << std::endl;
        std::cout << "  QUERY" << std::endl;
        for (auto &it : i.request.query) {
            std::cout << "      " << it.first << ": " << it.second << std::endl;
        }
        std::cout << "  Protocol: " << i.request.protocol << std::endl;
        std::cout << "  ProtocolVersion: " << i.request.protocolVersion << std::endl;
        std::cout << "  HEADERS" << std::endl;
        for (auto &it : i.request.headers) {
            std::cout << "      " << it.first << ": " << it.second.getData() << std::endl;
        }

        std::cout << "\nRESPONSE" << std::endl;
        std::cout << "  status: " << i.response.status << std::endl;
        std::cout << "  HEADERS" << std::endl;
        for (auto &it : i.response.headers) {
            std::cout << "      " << it.first << ": " << it.second.getData() << std::endl;
        }
        std::cout << "==================================================" << std::endl << std::endl;

    }

    void printEntity(zany::Entity entity) {
        if (entity.isArray()) {
            std::cout << "Array [" << std::endl;
            int i = 0;
            for (auto &it : entity.value<zany::Array>()) {
                std::cout << "[" << i << "]: ";
                printEntity(it);
                i++;
            }
            std::cout << "]" << std::endl;

        } else if (entity.isObject()) {
            std::cout << "Object { " << std::endl;
            for (auto &it : entity.value<zany::Object>()) {
                std::cout << "[" << it.first << "]: ";
                printEntity(it.second);
            }
            std::cout << "}" << std::endl;

        } else if (entity.isNumber()) {
            std::cout << "Number(" << entity.to<int>() << ")" << std::endl;
        } else if (entity.isString()) {
            std::cout << "String(" << entity.to<std::string>() << ")" << std::endl;
        } else if (entity.isBool()) {
            std::cout << "Bool(" << (entity.to<bool>() == true ? "true" : "false") << ")" << std::endl;
        } else if (entity.isNull()) {
            std::cout << "NULL" << std::endl;
        }
    }

	bool isOnLinux() {
		#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
		return false;
		#else
		return true;
		#endif
	}
}


#endif //ZIA_UTILS_HPP
