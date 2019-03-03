//
// Created by kahoul on 03/03/19.
//

#include "Params.hpp"
#include "Utils.hpp"

void ParamsModule::init() {

    garbage << this->master->getPipeline().getHookSet<zany::Pipeline::Hooks::BEFORE_HANDLE_REQUEST>()
            .addTask<zany::Pipeline::Priority::MEDIUM>(
                    std::bind(&ParamsModule::_beforeHandleRequest, this, std::placeholders::_1));

    garbage << this->master->getPipeline().getHookSet<zany::Pipeline::Hooks::ON_DATA_READY>()
            .addTask<zany::Pipeline::Priority::MEDIUM>(
                    std::bind(&ParamsModule::_onDataReady, this, std::placeholders::_1));

}

void ParamsModule::_parseQuery(zany::Pipeline::Instance &i) {

    std::istringstream sstm(i.request.fullQuery);
    std::string param;

    while (std::getline(sstm, param, '&')) {
        std::string key;

        std::istringstream pstm(param);
        std::getline(pstm, key, '=');
        std::getline(pstm, i.request.query[key]);
    }
}

void ParamsModule::_beforeHandleRequest(zany::Pipeline::Instance &i) {

    std::size_t endPathPos = 0;
    char c;

    while (endPathPos < i.request.path.size() && (c = i.request.path.at(endPathPos)) && c != '?') ++endPathPos;

    std::string tmp = std::string(i.request.path.begin(), i.request.path.begin() + endPathPos);
    if (c == '?') {
        i.request.fullQuery = std::string(i.request.path.begin() + endPathPos + 1, i.request.path.end());
        this->_parseQuery(i);
    }
    i.request.path = tmp;
}

void ParamsModule::_onDataReady(zany::Pipeline::Instance &i) {

    if (i.request.method == zany::HttpRequest::RequestMethods::POST) {

        auto length = i.request.headers.find("content-length");

        if (length == i.request.headers.end() || !length->second.isNumber()) {
            i.response.status = BAD_REQUEST;
            return ;
        }

        char buffer[std::stoi(length->second.getData())];

        auto sread = i.connection->stream().readsome(buffer, length->second.getNumber());

        i.request.fullQuery.append(buffer, sread);
        this->_parseQuery(i);
    }
    Utils::printPipelineContent(i);
}

extern "C" ZANY_DLL
        zany::Loader::AbstractModule    *entrypoint() {
    return new ParamsModule();
}