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

Zia::Zia(zany::Context &_ctx)
        : zany::Orchestrator(_ctx) {}

auto Zia::getConfig() const -> const zany::Entity {
    return _config;
}

void Zia::_setConfigOnDefault() {
    std::cout << "Set Config To Default" << std::endl;
    this->_config = zany::makeObject{
            {"port",    "4242"},
            {"docRoot", "./../assets"},
            {"modules", zany::makeArray{
                    "../lib/libSslConnectionModule.so",
                    "../lib/libHttpServerModule.so",
                    "../lib/libParamsModule.so"
            }
            }
    };
}

bool Zia::checkConfig() const {

    if (this->_config.isNull()) {
        std::cerr << "Error: No config given." << std::endl;
        return false;

    } else if (_config["port"].isNull() || (!_config["port"].isString() && !_config["port"].isNumber())) {
        std::cerr << "Error: port not specified." << std::endl;
        return false;

    } else if (_config["docRoot"].isNull() || !_config["docRoot"].isString()) {
        std::cerr << "Error: docRoot not specified." << std::endl;
        return false;
    }
    return true;
}

void Zia::run(int ac, char **av) {
    zany::ThreadPool tp(8);
    bool parsed = false;

    this->_pline.linkThreadPool(tp);

    if (ac == 0) {
        this->_setConfigOnDefault();

    } else {
        this->loadModule("../lib/libConfigParserModule.so", [this, &parsed, &av](auto &module) {
            std::cout << "Module: " << module.name() << " loaded" << std::endl;

            if (module.isAParser() && !parsed) {
                this->_config = module.parse(av[1]);
                parsed = true;
            }

        }, [this](auto exception) {
            std::cout << "Error: " << exception.what() << std::endl;
        });
    }

    if (!parsed || this->_config.isNull()) {
        this->_setConfigOnDefault();
    }

    if (!this->checkConfig()) {
        return ;
    }

    if (this->_config["modules"].isNull()) {
        std::cout << "Warning: No modules to preload !" << std::endl;
        return ;

    } else if (!this->_config["modules"].isArray()) {
        std::cout << "Warning: Bad format of modules's list to preload !" << std::endl;
        return ;

    } else {

        std::vector <zany::Entity> modules = this->_config["modules"].getData<zany::Array>().get();

        for (auto &toLoad : modules) {

            this->loadModule(toLoad.value<zany::String>(), [this, &parsed](auto &module) {
                std::cout << "Module: " << module.name() << " loaded" << std::endl;

            }, [this](auto exception) {
                std::cout << "Error: " << exception.what() << std::endl;

            });
        }
    }

    _ctx.addTask([this] {
        std::string line;
        const std::string load("loadModule");

        while (std::getline(std::cin, line)) {

            if (line.substr(0, load.length()) == load) {
                this->loadModule(line.substr(load.length()), [this](auto &module) {

                    std::cout << "Module: " << module.name() << " loaded." << std::endl;

                }, [this](auto exception) {

                    std::cout << "Error: " << exception.what() << std::endl;
                });
            } else {
                std::cout << "Command not found: " << line << "." << std::endl;
            }
        }
    });

    std::vector <std::uint16_t> ports;

    ports.push_back(this->_config["port"].to<int>());

    if (getCore()) {
        getCore()->startListening(ports);
    } else {
        throw std::runtime_error("Critical error: No core module loaded !");
    }


    _ctx.addTask([] { std::cout << "Ready" << std::endl; });
    _ctx.run();
}

void Zia::onPipelineThrow(PipelineExecutionError const &exception) {
    std::cerr << "Error: " << exception.what() << std::endl;
}

