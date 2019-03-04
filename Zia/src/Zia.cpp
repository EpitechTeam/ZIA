//
// Created by kahoul on 26/02/19.
//

#include <thread>
#include <iostream>
#include <vector>
#include <functional>
#include <type_traits>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>
#include "Zia.hpp"
#include "Zany/Connection.hpp"
#include "Zany/Event.hpp"

Zia::Zia(zany::Context	&_ctx)
    :  zany::Orchestrator(_ctx) {}

auto    Zia::getConfig() const -> const zany::Entity {
    return _config;
}

void    Zia::run(int ac, char **av) {

    zany::ThreadPool tp(8);

    this->_pline.linkThreadPool(tp);

    this->_config = zany::makeObject{
            { "host", av[1] },
            { "port", av[2] },
            { "docRoot", av[3]}
    };

    std::vector<std::string> modules = {
            "../lib/libConfigParserModule.so",
            "../lib/libConnectionModule.so",
            "../lib/libHttpServerModule.so",
            "../lib/libParamsModule.so"
     };

    bool	parsed = false;

    for (auto &module : modules) {

        this->loadModule(module, [this, &parsed] (auto &module) {
            std::cout << "Module: " << module.name() << " loaded" << std::endl;

            if (module.isAParser() && !parsed) {
                module.parse("");
                parsed = true;
            }

        }, [this] (auto exception) {
            std::cout << "Error: " << exception.what() << std::endl;

        });
    }

    /*std::thread moduleLoadingThread([this]() {
        std::string line;

        while (std::getline(std::cin, line)) {

            this->loadModule(line, [this] (auto &module) {
                std::cout << "Module: " << module.name() << " loaded" << std::endl;
            }, [this] (auto exception) {
                std::cout << "Error: " << exception.what() << std::endl;

            });
        }
    });*/

    //this->_server = new Server(av[1], av[2], av[3]);

    std::vector<std::uint16_t>	ports;

    ports.push_back(this->_config["port"].to<int>());

    if (getCore()) {
        getCore()->startListening(ports);
    } else {
        throw std::runtime_error("Critical error: No core module loaded !");
    }


    _ctx.addTask([] { std::cout << "Ready" << std::endl; });
    _ctx.run();
    //this->_server->run();
    //moduleLoadingThread.join();
}

void	Zia::onPipelineThrow(PipelineExecutionError const &exception) {
    std::cerr << "Error: " << exception.what() << std::endl;
}

