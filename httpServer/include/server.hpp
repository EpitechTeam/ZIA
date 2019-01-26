//
// Created by kahoul on 21/01/19.
//

#ifndef ZIA_SERVER_HPP
#define ZIA_SERVER_HPP

#include <boost/asio.hpp>
#include <string>
#include <thread>
#include "connection.hpp"
#include "connection_manager.hpp"
#include "request_handler.hpp"

/// The top-level class of the HTTP server.
class Server {
public:
    Server(const Server &) = delete;

    Server &operator=(const Server &) = delete;

    /// Construct the server to listen on the specified TCP address and port, and
    /// serve up files from the given directory.
    explicit Server(const std::string &address, const std::string &port,
                    const std::string &doc_root);

    /// Run the server's io_service loop.
    void run();

    int loadModule(const std::string &path);

private:
    /// Perform an asynchronous accept operation.
    void do_accept();

    /// Wait for a request to stop the server.
    void do_await_stop();

    /// The io_service used to perform asynchronous operations.
    boost::asio::io_service io_service_;

    /// The signal_set is used to register for process termination notifications.
    boost::asio::signal_set signals_;

    /// Acceptor used to listen for incoming connections.
    boost::asio::ip::tcp::acceptor acceptor_;

    /// The connection manager which owns all live connections.
    connection_manager connection_manager_;

    /// The next socket to be accepted.
    boost::asio::ip::tcp::socket socket_;

    /// The handler for all incoming requests.
    request_handler request_handler_;
};

#endif //ZIA_SERVER_HPP
