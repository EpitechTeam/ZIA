//
// Created by kahoul on 21/01/19.
//

#include <iostream>
#include <string>
#include <boost/asio.hpp>

#include "Server.hpp"
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

        //Server server(av[1], av[2], av[3]);
        //server.loadModule("");

        Zia                 zia(context);

        zia.run(ac, av);

        /*
        std::thread moduleLoadingThread([&server]() {
            std::string line;

            while (std::getline(std::cin, line)) {
                server.loadModule(line);
            }
        });*/
        // Run the server until stopped.
        //server.run();

       // moduleLoadingThread.join();
    }
    catch (std::exception& e)
    {
        std::cerr << "exception: " << e.what() << "\n";
    }

    return 0;
}