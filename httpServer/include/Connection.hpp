//
// Created by kahoul on 21/01/19.
//

#ifndef ZIA_CONNECTION_HPP
#define ZIA_CONNECTION_HPP

#include <array>
#include <memory>
#include <boost/asio.hpp>
#include "reply.hpp"
#include "request.hpp"
#include "RequestHandler.hpp"
#include "RequestParser.hpp"

class ConnectionManager;

class Connection
        : public std::enable_shared_from_this<Connection> {
public:
    Connection(const Connection &) = delete;

    Connection &operator=(const Connection &) = delete;

    explicit Connection(boost::asio::ip::tcp::socket socket,
                        ConnectionManager &manager, RequestHandler &handler);

    void start();

    void stop();

private:
    void doRead();

    void doWrite();

    boost::asio::ip::tcp::socket _socket;

    ConnectionManager &_connectionManager;

    RequestHandler &_requestHandler;

    std::array<char, 8192> _buffer;

    request _request;

    RequestParser _requestParser;

    reply _reply;
};

typedef std::shared_ptr <Connection> ConnectionPtr;


#endif //ZIA_CONNECTION_HPP
