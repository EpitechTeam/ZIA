//
// Created by kahoul on 03/03/19.
//

#ifndef ZIA_PARAMS_HPP
#define ZIA_PARAMS_HPP

#include "Zany/Loader.hpp"
#include "Zany/Pipeline.hpp"
#include "Zany/Orchestrator.hpp"
#include <boost/process.hpp>


class PhpCgiModule : public zany::Loader::AbstractModule {
public:
    PhpCgiModule() = default;

    ~PhpCgiModule() = default;

    virtual auto name() const -> const std::string & {
        static const std::string name("PhpCgi");
        return name;
    }

    virtual void init() final;


private:
    void _beforeHandleRequest(zany::Pipeline::Instance &i);

    void _onHandleResponse(zany::Pipeline::Instance &i);

    void _execPhpCgi(zany::Pipeline::Instance &i);

    boost::process::environment  buildEnv(zany::Pipeline::Instance &i);

    void exec1(zany::Pipeline::Instance &i, int pipe1[2]);

    void execPhp(zany::Pipeline::Instance &i);

};

#endif //ZIA_PARAMS_HPP
