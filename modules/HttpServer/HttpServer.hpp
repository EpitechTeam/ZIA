//
// Created by kahoul on 02/03/19.
//

#ifndef ZIA_HTTPSERVER_HPP
#define ZIA_HTTPSERVER_HPP

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <sstream>

#include "Zany/Loader.hpp"
#include "Zany/Pipeline.hpp"
#include "Zany/Orchestrator.hpp"

class HttpServerModule : public zany::Loader::AbstractModule {
public:
    HttpServerModule() = default;

    ~HttpServerModule() = default;

    virtual auto name() const -> const std::string & {
        static const std::string name("HttpServer");
        return name;
    }

    virtual void init() final;


private:
    void _beforeHandleRequest(zany::Pipeline::Instance &i);

    void _onHandleRequest(zany::Pipeline::Instance &i);

    void _beforeHandleResponse(zany::Pipeline::Instance &i);

    void _onHandleResponse(zany::Pipeline::Instance &i);

    void _parsePath(zany::Pipeline::Instance &i, std::string const &path);

    void _parseRequest(zany::Pipeline::Instance &i);

};

#endif //ZIA_HTTPSERVER_HPP
