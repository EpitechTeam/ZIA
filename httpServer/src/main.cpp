//
// Created by kahoul on 21/01/19.
//

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "server.hpp"

int main(int argc, char* argv[])
{
    try
    {
        // Check command line arguments.
        if (argc != 4)
        {
            std::cerr << "Usage: http_server <address> <port> <doc_root>\n";
            std::cerr << "  For IPv4, try:\n";
            std::cerr << "    receiver 0.0.0.0 80 .\n";
            std::cerr << "  For IPv6, try:\n";
            std::cerr << "    receiver 0::0 80 .\n";
            return 1;
        }

        // Initialise the server.
        Server server(argv[1], argv[2], argv[3]);

        std::thread moduleLoadingThread([&server]() {
            std::string line;

            while (std::getline(std::cin, line)) {
                server.loadModule(line);
            }
        });
        // Run the server until stopped.
        server.run();

        moduleLoadingThread.join();
    }
    catch (std::exception& e)
    {
        std::cerr << "exception: " << e.what() << "\n";
    }

    return 0;
}