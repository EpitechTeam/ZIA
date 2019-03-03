//
// Created by kahoul on 21/01/19.
//

#include <iostream>
#include <string>
#include <boost/asio.hpp>

#include "Zia.hpp"

int main(int ac, char* av[])
{

    try
    {
        if (ac != 4)
        {
            std::cerr << "Usage: zia_server <address> <port> <doc_root>\n";
            std::cerr << "  For IPv4, try:\n";
            std::cerr << "    receiver 0.0.0.0 80 .\n";
            std::cerr << "  For IPv6, try:\n";
            std::cerr << "    receiver 0::0 80 .\n";
            return 1;
        }

        zany::Context       context;

        Zia                 zia(context);

        zia.run(ac, av);

    }
    catch (std::exception& e)
    {
        std::cerr << "exception: " << e.what() << "\n";
    }

    return 0;
}