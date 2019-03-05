//
// Created by kahoul on 21/01/19.
//

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "ConfigParser.hpp"
#include "Server.hpp"

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2 && argc != 4)
        {
            std::cerr << "Usage: zia_server <address> <port> <doc_root> "
                         "<config_file>\n";
            std::cerr << "  For IPv4, try:\n";
            std::cerr << "    receiver 0.0.0.0 80 .\n";
            std::cerr << "  For IPv6, try:\n";
            std::cerr << "    receiver 0::0 80 .\n";
            return 1;
        }
        ServerConfig	config;
	ConfigParser configParser("ConfigParser");
        if (argc == 2) {
		config = configParser.loadFromConfigFile(argv[1]);
        } else {
		config = configParser.loadDefaultConfig();
		config["address"] = argv[1];
		config["port"] = argv[2];
		config["docRoot"] = argv[3];
        }
        Server server(config["address"], config["port"], config["docRoot"]);
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