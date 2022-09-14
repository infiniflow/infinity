//
// Created by JinHai on 2022/7/19.
//

#include <boost/bind/bind.hpp>
#include <thread>
#include "db_server.h"
#include "main/infinity.h"

namespace infinity {

DBServer::DBServer(const StartupParameter &parameter)
    : acceptor_(io_service_, boost::asio::ip::tcp::endpoint(parameter.address, parameter.port)) { }

void DBServer::Run() {
    initialized = true;
    Infinity::instance().Init();
    CreateConnection();
    io_service_.run();
}

void DBServer::Shutdown() {
    while(running_connection_count_ > 0) {
        // Running connection exists.
        std::this_thread::yield();
    }
    io_service_.stop();

    acceptor_.close();
}

void
DBServer::CreateConnection() {
    std::shared_ptr<Connection> connection_ptr = std::make_shared<Connection>(io_service_);
    acceptor_.async_accept(*(connection_ptr->socket()), boost::bind(&DBServer::StartConnection, this, connection_ptr));
}


void
DBServer::StartConnection(std::shared_ptr<Connection>& connection) {
    std::thread connection_thread ([connection = connection, &num_running_connections = this->running_connection_count_]() mutable {
        ++ num_running_connections;
        connection->Run();

        // User disconnected
        connection.reset();
        -- num_running_connections;
    });

    connection_thread.detach();
    CreateConnection();
}

}
