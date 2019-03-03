//
// Created by kahoul on 26/02/19.
//

#ifndef ZIA_LISTENER_HPP
#define ZIA_LISTENER_HPP

#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/bind.hpp>
#include "Zany/Connection.hpp"
#include "Zany/Pipeline.hpp"
#include "Zany/Event.hpp"
#include "Zany/Entity.hpp"

#define SSL_IDENTIFIER (22)


struct VirtualServersConfig {
    std::string hostname;
    std::uint16_t port;
    std::string certificateFile;
    std::string privateKeyFile;
    SSL_CTX *sslContext;

    void init() {
        const SSL_METHOD *method;
        SSL_CTX *ctx;

        method = ::SSLv23_server_method();

        ctx = ::SSL_CTX_new(method);
        if (!ctx) {
            throw std::runtime_error(std::string("OpenSSL: ") + ERR_error_string(ERR_get_error(), nullptr));
        }

        SSL_CTX_set_ecdh_auto(ctx, 1);

        if (::SSL_CTX_use_certificate_file(ctx, this->certificateFile.c_str(), SSL_FILETYPE_PEM) <= 0) {
            throw std::runtime_error(std::string("OpenSSL: ") + ERR_error_string(ERR_get_error(), nullptr));
        }

        if (::SSL_CTX_use_PrivateKey_file(ctx, this->privateKeyFile.c_str(), SSL_FILETYPE_PEM) <= 0) {
            throw std::runtime_error(std::string("OpenSSL: ") + ERR_error_string(ERR_get_error(), nullptr));
        }

        sslContext = ctx;
    }
};

class Listener {
public:
    class SslTcpIoStreamBidirectional : public boost::iostreams::device<boost::iostreams::bidirectional> {

    public:
        SslTcpIoStreamBidirectional(boost::asio::ip::tcp::socket *ssls)
                : stream(*ssls), disableSsl(false) {}

        std::streamsize write(const char *s, std::streamsize n) {
            if (disableSsl) {
                return boost::asio::write(stream, boost::asio::buffer(s, n));
            } else {
                return ::SSL_write(ssl->ssl, s, n);
            }
        }

        std::streamsize read(char *s, std::streamsize n) {
            if (disableSsl) {
                return stream.read_some(boost::asio::buffer(s, n));
            } else {
                return ::SSL_read(ssl->ssl, s, n);
            }
        }

        auto &getSocket() { return stream; }

        void setSslDisabled(bool status) { disableSsl = status; }

        auto isSslDisabled() { return disableSsl; }

        struct SslUtils {
            SSL *ssl;
            SSL_CTX *ref_ctx;
        };

        std::shared_ptr <SslUtils> ssl;
    private:
        boost::asio::ip::tcp::socket &stream;
        bool disableSsl;
    };

    class Connection : zany::Connection {
    public:
        template<typename ...Args>
        static auto make(Args &&...args) {
            return SharedInstance(reinterpret_cast<zany::Connection *>(
                                          new Connection(std::forward<Args>(args)...)
                                  ));
        }

        virtual std::iostream &stream() final { return _sslstream; }

        auto *parent() { return _parent; }

        auto &socket() { return _sock; }

        auto &sslStream() { return _sslstream; }

        static auto &fromZany(zany::Connection &self) { return reinterpret_cast<Connection &>(self); }

        auto peek() {
            char buffer = 0;
            ::recv(socket().native_handle(), &buffer, 1, MSG_PEEK);
            return buffer;
        }

        void doHandshake(zany::Pipeline::Instance &instance);

        auto onAccept() {
            auto first = peek();

            if (first != SSL_IDENTIFIER) {
                _sslstream->setSslDisabled(true);
            } else {
                _sslstream->ssl = std::make_shared<SslTcpIoStreamBidirectional::SslUtils>();
                _sslstream->ssl->ref_ctx = _parent->_baseCtx;
                _sslstream->ssl->ssl = ::SSL_new(_parent->_baseCtx);
            }
        }

    private:
        Connection(boost::asio::io_service &ios, Listener *parent) :
                zany::Connection(),
                _sock{ios},
                _sslstream(&_sock),
                _parent(parent) {}

        boost::asio::ip::tcp::socket _sock;
        boost::iostreams::stream <SslTcpIoStreamBidirectional>
                _sslstream;
        std::iostream *_stream;
        zany::evt::HdlCollector _collector;
        Listener *_parent;
    };


    template<typename PVERSION>
    Listener(PVERSION pv, boost::asio::io_service &ios, std::uint16_t port)
            : _acceptor(ios, boost::asio::ip::tcp::endpoint(std::forward<PVERSION>(pv), port)) {
        const SSL_METHOD *method;

        method = ::SSLv23_server_method();

        _baseCtx = ::SSL_CTX_new(method);
        if (!_baseCtx) {
            throw std::runtime_error(std::string("OpenSSL: ") + ERR_error_string(ERR_get_error(), nullptr));
        }
    }

    void startAccept() {
        auto nc = Connection::make(_acceptor.get_io_service(), this);

        _acceptor.async_accept(Connection::fromZany(*nc).socket(),
                               boost::bind(&Listener::handleAccept, this, nc,
                                           boost::asio::placeholders::error));
    }

    void handleAccept(zany::Connection::SharedInstance nConnection, const boost::system::error_code &error) {
        if (!error) {
            auto &&remoteEp = Connection::fromZany(*nConnection).socket().remote_endpoint();
            auto &&remoteAd = remoteEp.address();
            nConnection->info.ip = remoteAd.to_string();
            nConnection->info.port = remoteEp.port();

            std::cout << "New Connection from: " << '[' << nConnection->info.ip << "]:" << nConnection->info.port
                      << std::endl;
            Connection::fromZany(*nConnection).onAccept();
            onHandleAccept(nConnection);
            startAccept();
        }
    }

    void initVHostConfig(zany::Entity const &cfg) {
        try {
            auto &vHosts = cfg["server"].value<zany::Array>();

            for (auto &vhost: vHosts) {
                try {
                    auto it = vhost.value<zany::Object>().find("ssl");
                    if (it == vhost.value<zany::Object>().end())
                        continue;

                    auto &sslc = it->second;
                    auto targetPort = (std::uint16_t) vhost["port"].to<int>();
                    if (targetPort != _acceptor.local_endpoint().port())
                        continue;
                    auto vhit = vhostsConfigs.emplace(
                            std::pair<std::string, VirtualServersConfig>(
                                    vhost["host"].value<zany::String>(),
                                    VirtualServersConfig{
                                            vhost["host"].value<zany::String>(),
                                            targetPort,
                                            sslc["certificate"].value<zany::String>(),
                                            sslc["private-key"].value<zany::String>(),
                                            nullptr
                                    }
                            )
                    );
                    vhit->second.init();
                } catch (std::exception const &e) {
                    std::cerr << e.what() << std::endl;
                }
            }
        } catch (...) {}
    }

std::function<
    void (zany::Connection::SharedInstance
    nConnection)>
    onHandleAccept;

    boost::asio::ip::tcp::acceptor _acceptor;
    std::unordered_multimap <std::string, VirtualServersConfig>
            vhostsConfigs;
    SSL_CTX *_baseCtx;
};

void Listener::Connection::doHandshake(zany::Pipeline::Instance &pipeline) {
    if (!_sslstream->isSslDisabled()) {
        pipeline.request.port = _parent->_acceptor.local_endpoint().port();

        struct {
            Connection *co;
            zany::Pipeline::Instance *pipeline;
        } data = {
                this,
                &pipeline
        };

        typedef int (*callback_t)(SSL *, int *, decltype(data) *);
        static callback_t callback = [](SSL *ssl, int *, decltype(data) *cap) -> int {
            const char *hostname = SSL_get_servername(ssl, TLSEXT_NAMETYPE_host_name);

            auto vhit = cap->co->parent()->vhostsConfigs.find(hostname);
            if (vhit == cap->co->parent()->vhostsConfigs.end()) return 1;
            ::SSL_set_SSL_CTX(ssl, vhit->second.sslContext);
            return 0;
        };

        auto &sslData = *_sslstream->ssl;

        sslData.ssl = SSL_new(sslData.ref_ctx);
        ::SSL_set_fd(sslData.ssl, _sock.native_handle());

        ::SSL_CTX_set_tlsext_servername_arg(sslData.ref_ctx, &data);
        ::SSL_CTX_set_tlsext_servername_callback(sslData.ref_ctx, callback);

        if (::SSL_accept(sslData.ssl) <= 0) {
            throw std::runtime_error(std::string("OpenSSL: ") + ERR_error_string(ERR_get_error(), nullptr));
        }
    }
}


#endif //ZIA_LISTENER_HPP
