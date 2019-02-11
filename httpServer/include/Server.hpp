//
// Created by kahoul on 21/01/19.
//

#ifndef ZIA_SERVER_HPP
#define ZIA_SERVER_HPP

#include <boost/asio.hpp>
#include <string>
#include <thread>
#include "Connection.hpp"
#include "ConnectionManager.hpp"
#include "RequestHandler.hpp"

class Server {
public:
    Server(const Server &) = delete;

    Server &operator=(const Server &) = delete;

    explicit Server(const std::string &address, const std::string &port,
                    const std::string &docRoot);

    void run();

    int loadModule(const std::string &path);

private:
    void doAccept();

    void doAwaitStop();

    boost::asio::io_service _ioService;

    boost::asio::signal_set _signals;

    boost::asio::ip::tcp::acceptor _acceptor;

    ConnectionManager _connectionManager;

    boost::asio::ip::tcp::socket _socket;

    RequestHandler _requestHandler;
};

#endif //ZIA_SERVER_HPP
