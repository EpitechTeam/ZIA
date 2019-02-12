//
// Created by kahoul on 21/01/19.
//

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "Server.hpp"

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 4)
        {
            std::cerr << "Usage: zia_server <address> <port> <doc_root>\n";
            std::cerr << "  For IPv4, try:\n";
            std::cerr << "    receiver 0.0.0.0 80 .\n";
            std::cerr << "  For IPv6, try:\n";
            std::cerr << "    receiver 0::0 80 .\n";
            return 1;
        }

        Server server(argv[1], argv[2], argv[3]);
        server.loadModule("");

        /*
        std::thread moduleLoadingThread([&server]() {
            std::string line;

            while (std::getline(std::cin, line)) {
                server.loadModule(line);
            }
        });*/
        // Run the server until stopped.
        server.run();

       // moduleLoadingThread.join();
    }
    catch (std::exception& e)
    {
        std::cerr << "exception: " << e.what() << "\n";
    }

    return 0;
}