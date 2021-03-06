//
// Created by kahoul on 26/02/19.
//

#ifndef ZIA_ZIA_HPP
#define ZIA_ZIA_HPP

#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <boost/bind.hpp>

#include "Zany/Orchestrator.hpp"

class Zia: public zany::Orchestrator {
public:
    Zia(zany::Context	&_ctx);
    ~Zia();

    virtual auto	getConfig() const -> const zany::Entity final;
    void			run(int ac, char **av);
    void			startPipeline(zany::Connection::SharedInstance c);

protected:
    bool            checkConfig() const;
    virtual void	onPipelineThrow(PipelineExecutionError const &exception) final;
    void            _setConfigOnDefault();

private:

    zany::Entity    _config;
    std::thread     *_loadModuleThread = nullptr;
};


#endif //ZIA_ZIA_HPP
