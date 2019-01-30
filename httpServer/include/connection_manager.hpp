//
// Created by kahoul on 21/01/19.
//

#ifndef ZIA_CONNECTION_MANAGER_HPP
#define ZIA_CONNECTION_MANAGER_HPP

#include <set>
#include "connection.hpp"

/// Manages open connections so that they may be cleanly stopped when the server
/// needs to shut down.
class connection_manager {
public:
    connection_manager(const connection_manager &) = delete;

    connection_manager &operator=(const connection_manager &) = delete;

    /// Construct a connection manager.
    connection_manager();

    /// Add the specified connection to the manager and start it.
    void start(connection_ptr c);

    /// Stop the specified connection.
    void stop(connection_ptr c);

    /// Stop all connections.
    void stop_all();

private:
    /// The managed connections.
    std::set <connection_ptr> connections_;
};


#endif //ZIA_CONNECTION_MANAGER_HPP
