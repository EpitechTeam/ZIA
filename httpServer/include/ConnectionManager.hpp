//
// Created by kahoul on 21/01/19.
//

#ifndef ZIA_CONNECTION_MANAGER_HPP
#define ZIA_CONNECTION_MANAGER_HPP

#include <set>
#include "Connection.hpp"

class ConnectionManager {
public:
    ConnectionManager(const ConnectionManager &) = delete;

    ConnectionManager &operator=(const ConnectionManager &) = delete;

    ConnectionManager();

    void start(ConnectionPtr c);

    void stop(ConnectionPtr c);

    void stopAll();

private:
    std::set <ConnectionPtr> _connections;
};


#endif //ZIA_CONNECTION_MANAGER_HPP
