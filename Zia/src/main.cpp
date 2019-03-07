//
// Created by kahoul on 21/01/19.
//

#include <iostream>
#include <string>
#include <boost/asio.hpp>

#include "Zia.hpp"

int usage() {

    std::cerr << "Usage: zia    [configFile]" << std::endl;
    std::cerr << "  configFile  Relative path of config file." << std::endl;
    return 1;
}

int main(int ac, char **av)
{

    try
    {

        if (ac == 2 && (std::string(av[1]) == "--help" || std::string(av[1]) == "-h")) {
            return usage();
        }

        zany::Context       context;
        Zia                 zia(context);

        zia.run(ac, av);

    }
    catch (std::exception& e)
    {
        std::cerr << "exception: " << e.what() << "\n";
        return 1;
    }

    return 0;
}