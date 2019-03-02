//
// Created by kahoul on 02/03/19.
//

#include "HttpServer.hpp"
#include "mime_types.hpp"
#include "reply.hpp"

void HttpServerModule::init() {

    garbage << this->master->getPipeline().getHookSet<zany::Pipeline::Hooks::BEFORE_HANDLE_REQUEST>()
            .addTask<zany::Pipeline::Priority::HIGH>(std::bind(&HttpServerModule::_beforeHandleRequest, this, std::placeholders::_1));

    garbage << this->master->getPipeline().getHookSet<zany::Pipeline::Hooks::ON_HANDLE_REQUEST>()
            .addTask<zany::Pipeline::Priority::HIGH>(std::bind(&HttpServerModule::_onHandleRequest, this, std::placeholders::_1));

    garbage << this->master->getPipeline().getHookSet<zany::Pipeline::Hooks::BEFORE_HANDLE_RESPONSE>()
            .addTask<zany::Pipeline::Priority::HIGH>(std::bind(&HttpServerModule::_beforeHandleResponse, this, std::placeholders::_1));

    garbage << this->master->getPipeline().getHookSet<zany::Pipeline::Hooks::ON_HANDLE_RESPONSE>()
            .addTask<zany::Pipeline::Priority::HIGH>(std::bind(&HttpServerModule::_onHandleResponse, this, std::placeholders::_1));

}

void HttpServerModule::_beforeHandleRequest(zany::Pipeline::Instance &i) {
    i.writerID = this->getUniqueId();
    std::string line;
    std::string request;

    std::cout << "Request:" << std::endl;
    std::getline(i.connection->stream(), line);
    request.append(line + "\r\n");

    while (line != "\r") {
        std::getline(i.connection->stream(), line);
        request.append(line + "\r\n");
    }

    std::cout << request << std::endl;
}

void HttpServerModule::_onHandleRequest(zany::Pipeline::Instance &i) {

}

void HttpServerModule::_beforeHandleResponse(zany::Pipeline::Instance &i) {

}

void HttpServerModule::_onHandleResponse(zany::Pipeline::Instance &i) {
    reply rep;

    rep.status = reply::ok;
    char buf[512];
    rep.content = "Zia A+++";
    rep.headers.resize(2);
    rep.headers[0].name = "Content-Length";
    rep.headers[0].value = std::to_string(rep.content.size());
    rep.headers[1].name = "Content-Type";
    rep.headers[1].value = mime_types::extensionToType("html");

    /*for (auto &it : rep.toBuffers()) {
        std::cout << "\r\n" << it.data() << "\r\n";
        i.connection->stream() << "\r\n" << it.data() << "\r\n";
    }*/
}

extern "C" ZANY_DLL
        zany::Loader::AbstractModule	*entrypoint() {
    return new HttpServerModule();
}