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

#include <boost/bind/bind.hpp>
#include <thread>

module infinity_core:pg_server.impl;

import :pg_server;
import :thrift_server;
import :infinity_context;
import :stl;
import :boost;
import third_party;
import :infinity_exception;

import :connection;
import :logger;

namespace infinity {

Thread PGServer::Run() {
    {
        auto expected = PGServerStatus::kStopped;
        if (!status_.compare_exchange_strong(expected, PGServerStatus::kStarting)) {
            UnrecoverableError(fmt::format("PGServer in unexpected state: {}", u8(expected)));
        }
    }
    u16 pg_port = InfinityContext::instance().config()->PostgresPort();
    const String &pg_listen_addr = InfinityContext::instance().config()->ServerAddress();

    boost::system::error_code error;
    boost::asio::ip::address address = boost::asio::ip::make_address(pg_listen_addr, error);
    if (error) {
        infinity::InfinityContext::instance().UnInit();
        String err_msg = fmt::format("{} isn't a valid IPv4 address.\n", pg_listen_addr);
        UnrecoverableError(err_msg);
    }

    running_connection_count_ = 0;
    io_context_ptr_ = MakeUnique<boost::asio::io_context>();
    acceptor_ptr_ = MakeUnique<boost::asio::ip::tcp::acceptor>(*io_context_ptr_, boost::asio::ip::tcp::endpoint(address, pg_port));
    CreateConnection();

    fmt::print("Run 'psql -h {} -p {}' to connect to the server (SQL is only for test).\n", pg_listen_addr, pg_port);
    status_.store(PGServerStatus::kRunning);
    return Thread([this] {
        io_context_ptr_->run();

        status_.store(PGServerStatus::kStopped);
        status_.notify_one();
    });
}

void PGServer::Shutdown() {
    {
        auto expected = PGServerStatus::kRunning;
        if (!status_.compare_exchange_strong(expected, PGServerStatus::kStopping)) {
            if (status_ == PGServerStatus::kStopped) {
                return;
            } else {
                UnrecoverableError(fmt::format("PGServer in unexpected state: {}", u8(expected)));
            }
        }
    }

    while (running_connection_count_ > 0) {
        // Running connection exists.
        std::this_thread::yield();
    }

    io_context_ptr_->stop();
    acceptor_ptr_->close();

    status_.wait(PGServerStatus::kStopping);
}

void PGServer::CreateConnection() {
    SharedPtr<Connection> connection_ptr = MakeShared<Connection>(*io_context_ptr_);
    acceptor_ptr_->async_accept(*(connection_ptr->socket()), boost::bind(&PGServer::StartConnection, this, connection_ptr));
}

void PGServer::StartConnection(SharedPtr<Connection> &connection) {
    bool started = status_ == PGServerStatus::kRunning;
    if (started) {
        Thread connection_thread([connection = connection, &num_running_connections = this->running_connection_count_]() mutable {
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
        });
        connection_thread.detach();
        CreateConnection();
    }
}

} // namespace infinity
