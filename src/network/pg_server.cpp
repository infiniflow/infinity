// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

#include <boost/bind.hpp>
#include <thread>

module pg_server;

import thrift_server;
import infinity_context;
import stl;
import boost;
import third_party;
import infinity_exception;

import connection;
import logger;

namespace infinity {

void PGServer::Run() {
    if (started_) {
        return;
    }

    started_ = true;

    u16 pg_port = InfinityContext::instance().config()->PostgresPort();
    const String &pg_listen_addr = InfinityContext::instance().config()->ServerAddress();

    boost::system::error_code error;
    boost::asio::ip::address address = boost::asio::ip::make_address(pg_listen_addr, error);
    if (error) {
        fmt::print("{} isn't a valid IPv4 address.\n", pg_listen_addr);
        infinity::InfinityContext::instance().UnInit();
        return ;
    }

    acceptor_ptr_ = MakeUnique<boost::asio::ip::tcp::acceptor>(io_service_, boost::asio::ip::tcp::endpoint(address, pg_port));
    CreateConnection();

    fmt::print("Run 'psql -h {} -p {}' to connect to the server (SQL is only for test).\n", pg_listen_addr, pg_port);

    io_service_.run();
}

void PGServer::Shutdown() {

    started_ = false;

    while (running_connection_count_ > 0) {
        // Running connection exists.
        std::this_thread::yield();
    }

    io_service_.stop();
    acceptor_ptr_->close();
}

void PGServer::CreateConnection() {
    SharedPtr<Connection> connection_ptr = MakeShared<Connection>(io_service_);
    acceptor_ptr_->async_accept(*(connection_ptr->socket()), boost::bind(&PGServer::StartConnection, this, connection_ptr));
}

void PGServer::StartConnection(SharedPtr<Connection> &connection) {
    Thread connection_thread([connection = connection, &num_running_connections = this->running_connection_count_, initialized = bool(this->started_)]() mutable {
        if(initialized) {
            ++num_running_connections;
            try {
                connection->Run();
            } catch (...) {
                String ip_address;
                u16 port;
                connection->GetClientInfo(ip_address, port);
                String msg = fmt::format("closed connection with {}:{} due to exception", ip_address, port);
                LOG_ERROR(msg);
            }

            // User disconnected
            connection.reset();
            --num_running_connections;
        }
    });

    connection_thread.detach();
    CreateConnection();
}

} // namespace infinity
