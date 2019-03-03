//
// Created by kahoul on 03/03/19.
//

#ifndef ZIA_PARAMS_HPP
#define ZIA_PARAMS_HPP

#include "Zany/Loader.hpp"
#include "Zany/Pipeline.hpp"
#include "Zany/Orchestrator.hpp"

class ParamsModule : public zany::Loader::AbstractModule {
public:
    ParamsModule() = default;

    ~ParamsModule() = default;

    virtual auto name() const -> const std::string & {
        static const std::string name("Params");
        return name;
    }

    virtual void init() final;


private:
    void _beforeHandleRequest(zany::Pipeline::Instance &i);

    void _onDataReady(zany::Pipeline::Instance &i);

    void _parseQuery(zany::Pipeline::Instance &i);
};

#endif //ZIA_PARAMS_HPP
