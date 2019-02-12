//
// Created by kahoul on 21/01/19.
//

#include "ConnectionManager.hpp"

ConnectionManager::ConnectionManager() {
}

void ConnectionManager::start(ConnectionPtr connection) {
    this->_connections.insert(connection);
    connection->start();
}

void ConnectionManager::stop(ConnectionPtr connection) {
    this->_connections.erase(connection);
    connection->stop();
}

void ConnectionManager::stopAll() {
    for (auto connection: this->_connections)
        connection->stop();
    this->_connections.clear();
}
