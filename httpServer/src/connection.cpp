//
// Created by kahoul on 21/01/19.
//

#include "Connection.hpp"
#include <utility>
#include <vector>
#include "ConnectionManager.hpp"
#include "RequestHandler.hpp"

Connection::Connection(boost::asio::ip::tcp::socket socket,
                       ConnectionManager &manager, RequestHandler &handler)
        : _socket(std::move(socket)),
          _connectionManager(manager),
          _requestHandler(handler) {
}

void Connection::start() {
    this->doRead();
}

void Connection::stop() {
    this->_socket.close();
}

void Connection::doRead() {
    auto self(shared_from_this());
    this->_socket.async_read_some(boost::asio::buffer(this->_buffer),
                            [this, self](boost::system::error_code ec, std::size_t bytesTransferred) {
                                if (!ec) {
                                    RequestParser::resultType result;
                                    std::tie(result, std::ignore) = this->_requestParser.parse(
                                            this->_request, this->_buffer.data(), this->_buffer.data() + bytesTransferred);

                                    if (result == RequestParser::good) {
                                        this->_requestHandler.handleRequest(this->_request, this->_reply, shared_from_this());
                                        this->doWrite();
                                    } else if (result == RequestParser::bad) {
                                        this->_reply = reply::stockReply(reply::bad_request);
                                        this->doWrite();
                                    } else {
                                        this->doRead();
                                    }
                                } else if (ec != boost::asio::error::operation_aborted) {
                                    this->_connectionManager.stop(shared_from_this());
                                }
                            });
}

void Connection::doWrite() {
    auto self(shared_from_this());
    boost::asio::async_write(this->_socket, this->_reply.toBuffers(),
                             [this, self](boost::system::error_code ec, std::size_t) {
                                 if (!ec) {
                                     boost::system::error_code ignoredEc;
                                     this->_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both,
                                                      ignoredEc);
                                 }

                                 if (ec != boost::asio::error::operation_aborted) {
                                     this->_connectionManager.stop(shared_from_this());
                                 }
                             });
}
