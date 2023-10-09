//
// Created by JinHai on 2022/7/19.
//

#include "db_server.h"
#include "common/utility/infinity_assert.h"
#include "main/infinity.h"
#include <boost/bind/bind.hpp>
#include <thread>

namespace infinity {

DBServer::DBServer(const StartupParameter &parameter) : config_path_(parameter.config_path) {}

void DBServer::Run() {
    if (initialized) {
        return;
    }

    initialized = true;
    Infinity::instance().Init(config_path_);
    u16 pg_port = Infinity::instance().config()->pg_port();
    const String &listen_address_ref = Infinity::instance().config()->listen_address();

    boost::system::error_code error;
    boost::asio::ip::address address = boost::asio::ip::make_address(listen_address_ref, error);
    if (error) {
        GeneralError(fmt::format("Not a valid IPv4 address: {}", listen_address_ref));
    }

    acceptor_ptr_ = MakeUnique<boost::asio::ip::tcp::acceptor>(io_service_, boost::asio::ip::tcp::endpoint(address, pg_port));
    CreateConnection();

    if (config_path_) {
        LOG_INFO("Startup database server, at: {} and port: {}, config: {}", listen_address_ref, pg_port, *config_path_);
    } else {
        LOG_INFO("Startup database server, at: {} and port: {}", listen_address_ref, pg_port);
    }

    Infinity::instance().config()->PrintAll();

    LOG_INFO("Run 'psql -h {} -p {}' to connect to the server.", listen_address_ref, pg_port);

    io_service_.run();
}

void DBServer::Shutdown() {
    while (running_connection_count_ > 0) {
        // Running connection exists.
        std::this_thread::yield();
    }

    io_service_.stop();
    initialized = false;
    acceptor_ptr_->close();
}

void DBServer::CreateConnection() {
    SharedPtr<Connection> connection_ptr = MakeShared<Connection>(io_service_);
    acceptor_ptr_->async_accept(*(connection_ptr->socket()), boost::bind(&DBServer::StartConnection, this, connection_ptr));
}

void DBServer::StartConnection(SharedPtr<Connection> &connection) {
    std::thread connection_thread([connection = connection, &num_running_connections = this->running_connection_count_]() mutable {
        ++num_running_connections;
        connection->Run();

        // User disconnected
        connection.reset();
        --num_running_connections;
    });

    connection_thread.detach();
    CreateConnection();
}

} // namespace infinity
