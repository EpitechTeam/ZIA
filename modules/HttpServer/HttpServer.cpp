//
// Created by kahoul on 02/03/19.
//

#include "HttpServer.hpp"
#include "Utils.hpp"

void HttpServerModule::init() {

    garbage << this->master->getPipeline().getHookSet<zany::Pipeline::Hooks::BEFORE_HANDLE_REQUEST>()
            .addTask<zany::Pipeline::Priority::HIGH>(
                    std::bind(&HttpServerModule::_beforeHandleRequest, this, std::placeholders::_1));

    garbage << this->master->getPipeline().getHookSet<zany::Pipeline::Hooks::ON_HANDLE_REQUEST>()
            .addTask<zany::Pipeline::Priority::HIGH>(
                    std::bind(&HttpServerModule::_onHandleRequest, this, std::placeholders::_1));

    garbage << this->master->getPipeline().getHookSet<zany::Pipeline::Hooks::BEFORE_HANDLE_RESPONSE>()
            .addTask<zany::Pipeline::Priority::HIGH>(
                    std::bind(&HttpServerModule::_beforeHandleResponse, this, std::placeholders::_1));

    garbage << this->master->getPipeline().getHookSet<zany::Pipeline::Hooks::ON_HANDLE_RESPONSE>()
            .addTask<zany::Pipeline::Priority::HIGH>(
                    std::bind(&HttpServerModule::_onHandleResponse, this, std::placeholders::_1));

}

void HttpServerModule::_parsePath(zany::Pipeline::Instance &i, std::string const &path) {

}

void HttpServerModule::_parseRequest(zany::Pipeline::Instance &i) {
    std::string line;

    i.connection->stream() >> std::ws;
    std::getline(i.connection->stream(), line);
    if (line.empty()) { return; }

    line.erase(--line.end());
    std::istringstream stm(line);
    std::string str;

    stm >> str;
    i.request.method = Utils::getMethodFromString(str);
    i.request.methodString = str;

    stm >> str;
    i.request.path = str;

    stm >> str;
    std::istringstream pstm(str);
    std::string proto;

    std::getline(pstm, proto, '/');
    i.request.protocol = proto;
    std::getline(pstm, proto, '/');
    i.request.protocolVersion = proto;

    i.response.protocol = i.request.protocol;
    i.response.protocolVersion = i.request.protocolVersion;

    while (std::getline(i.connection->stream(), line)
           && line != "" && line != "\r") {
        line.erase(--line.end());
        std::istringstream splitor(line);
        std::string key;

        std::getline(splitor, key, ':');
        boost::to_lower(key);
        auto &value = *i.request.headers[key];
        std::getline(splitor, value);
        boost::trim(value);

        if (boost::to_lower_copy(key) == "host") {
            std::istringstream splitor(value);
            std::string port;

            std::getline(splitor, i.request.host, ':');
            std::getline(splitor, port);

            i.request.port = static_cast<int>(zany::HttpHeader(port).getNumber());
        }
    }
}

void HttpServerModule::_beforeHandleRequest(zany::Pipeline::Instance &i) {
    i.writerID = this->getUniqueId();

    this->_parseRequest(i);
}

void HttpServerModule::_onHandleRequest(zany::Pipeline::Instance &i) {

    // Request path must be absolute and not contain "..".
    if (i.request.path.empty() || i.request.path[0] != '/'
        || i.request.path.find("..") != std::string::npos) {
        i.response.status = BAD_REQUEST;
        return ;
    }

    // If path ends without a file then add "index.html".
    if (i.request.path.find_first_of('.') == std::string::npos) {
        if (i.request.path[i.request.path.size() - 1] == '/') {
            i.request.path += "index.html";
        } else
            i.request.path += "/index.html";
    }

    // Get full path
    i.request.path = master->getConfig()["docRoot"].value<zany::String>() + i.request.path;

}


void HttpServerModule::_beforeHandleResponse(zany::Pipeline::Instance &i) {

    if (i.response.status == OK) {

        if (!boost::filesystem::is_regular_file(i.request.path)) {
            i.response.status = NOT_FOUND;
        } else if (i.request.method == zany::HttpRequest::RequestMethods::GET) {
            auto &fs = (i.properties["fs"] = zany::Property::make<std::ifstream>(i.request.path)).get<std::ifstream>();

            if (fs.bad()) {
                i.response.status = INTERNAL_SERVER_ERROR;
            }
        }
    }
}

void HttpServerModule::_onHandleResponse(zany::Pipeline::Instance &i) {

    if (i.writerID == this->getUniqueId()
        && i.response.status == OK
        && i.request.method == zany::HttpRequest::RequestMethods::GET) {
        auto 	&fs = i.properties["fs"].get<std::ifstream>();

        Utils::_writeReponseHeader(i);
        i.connection->stream() << "\r\n" << fs.rdbuf() << "\r\n";

    } else if (i.writerID == 0 || i.writerID == this->getUniqueId()) {
        Utils::writeResponse(i, reply::to_string(i.response.status));
    }
    Utils::printPipelineContent(i);
}

extern "C" ZANY_DLL
        zany::Loader::AbstractModule    *entrypoint() {
    return new HttpServerModule();
}