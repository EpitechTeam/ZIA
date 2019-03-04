//
// Created by kahoul on 26/02/19.
//

#include <fstream>
#include <thread>
#include <unordered_map>
#include <boost/filesystem.hpp>
#include "./Listener.hpp"
#include "Zany.hpp"

    class ConnectionModule : public zany::Loader::AbstractModule {
    public:
        ConnectionModule(): _signals(_ios) {
            _signals.add(SIGINT);
            _signals.add(SIGTERM);
#if defined(SIGQUIT)
            _signals.add(SIGQUIT);
#endif // defined(SIGQUIT)
            _signals.async_wait(boost::bind(&ConnectionModule::_onSignal, this));
        }

        ~ConnectionModule();

        virtual auto	name() const -> const std::string&
        { static const std::string name("Connection"); return name; }
        virtual void	init();
        virtual bool	isACoreModule() final { return true; }
        virtual void	startListening(std::vector<std::uint16_t> &ports) final;
    private:
        void	_onSignal();
        void	_listening(std::condition_variable &cv);
        void	_startPipeline(zany::Connection::SharedInstance c);

        std::vector<std::uint16_t>		_ports;
        boost::asio::io_context			_ios;
        std::unique_ptr<std::thread>	_t;
        boost::asio::signal_set			_signals;
    };

    void	ConnectionModule::init() {
        SSL_load_error_strings();
        OpenSSL_add_ssl_algorithms();
    }

ConnectionModule::~ConnectionModule() {
        _ios.stop();
        if (_t) {
            _t->join();
        }
        EVP_cleanup();
    }

    void	ConnectionModule::_onSignal() {
        std::cout << "Closing..." << std::endl;

        zany::evt::Manager::get()["onClose"]->fire();
        this->master->getContext().stop();
    }

    void	ConnectionModule::_listening(std::condition_variable &cv) {
        auto &ports = _ports;

        std::vector<Listener>	acceptors;

        acceptors.reserve(ports.size());
        for (auto port: ports) {
            auto 			&v6listener = acceptors.emplace_back(boost::asio::ip::tcp::v6(), _ios, port);

            v6listener.onHandleAccept = [this] (zany::Connection::SharedInstance c) {
                boost::asio::ip::v6_only option;

                Listener::Connection::fromZany(*c).socket().get_option(option);
                if (option.value() == true) {
                    boost::asio::ip::v6_only noption(false);

                    Listener::Connection::fromZany(*c).socket().set_option(noption);
                }
                this->master->getContext().addTask(std::bind(&ConnectionModule::_startPipeline, this, c));
            };

            v6listener.initVHostConfig(master->getConfig());
            v6listener.startAccept();
            std::cout << "Starting listener on port " << port << std::endl;
        }

        std::this_thread::yield();
        cv.notify_all();
        _ios.run();
    }

    void	ConnectionModule::_startPipeline(zany::Connection::SharedInstance c) {
        constexpr auto	sp = &zany::Orchestrator::startPipeline;
        auto			&connection = Listener::Connection::fromZany(*c);

        try {
            (this->master->*sp)(c, std::bind(&Listener::Connection::doHandshake, &connection, std::placeholders::_1));
        } catch (std::exception &e) {
            std::cerr
                    << "Connection rejected because of internal server error:\n\t"
                    << e.what() << std::endl;
        }
    }

    void	ConnectionModule::startListening(std::vector<std::uint16_t> &ports) {
        std::mutex						mtx;
        std::unique_lock<decltype(mtx)>	ulock(mtx);
        std::condition_variable			cv;

        _ports = ports;
        _t = std::make_unique<std::thread>(std::bind(&ConnectionModule::_listening, this, std::ref(cv)));
        cv.wait(ulock);
    }

extern "C" ZANY_DLL
        zany::Loader::AbstractModule	*entrypoint() {
    return new ConnectionModule();
}
