//
// Created by kahoul on 21/01/19.
//

#include "Server.hpp"
#include <signal.h>
#include <utility>

Server::Server(const std::string &address, const std::string &port,
               const std::string &docRoot)
        : _ioService(),
          _signals(_ioService),
          _acceptor(_ioService),
          _connectionManager(),
          _socket(_ioService),
          _requestHandler(docRoot) {
    this->_signals.add(SIGINT);
    this->_signals.add(SIGTERM);
#if defined(SIGQUIT)
    this->_signals.add(SIGQUIT);
#endif // defined(SIGQUIT)

    this->doAwaitStop();

    boost::asio::ip::tcp::resolver resolver(this->_ioService);
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve({address, port});
    this->_acceptor.open(endpoint.protocol());
    this->_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    this->_acceptor.bind(endpoint);
    this->_acceptor.listen();

    this->doAccept();
}

void Server::run() {
    this->_ioService.run();
}

void Server::doAccept() {
    this->_acceptor.async_accept(this->_socket,
                           [this](boost::system::error_code ec) {
                               if (!this->_acceptor.is_open()) {
                                   return;
                               }

                               if (!ec) {
                                   this->_connectionManager.start(std::make_shared<Connection>(
                                           std::move(this->_socket), this->_connectionManager, this->_requestHandler));
                               }

                               this->doAccept();
                           });
}

void Server::doAwaitStop() {
    this->_signals.async_wait(
            [this](boost::system::error_code /*ec*/, int /*signo*/) {
                this->_acceptor.close();
                this->_connectionManager.stopAll();
            });
}

int Server::loadModule(const std::string &path) {
    return this->_requestHandler.loadModule(path);
}