//
// Created by JinHai on 2022/7/19.
//

#pragma once

#include "common/singleton.h"
#include "common/types/alias/concurrency.h"
#include "connection.h"
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace infinity {

struct StartupParameter {
    SharedPtr<String> config_path{};
};

class DBServer : public Singleton<DBServer> {
public:
    explicit DBServer(const StartupParameter &parameter);

    void Run();

    void Shutdown();

private:
    void CreateConnection();

    void StartConnection(SharedPtr<Connection> &connection);

    std::atomic_bool initialized{false};
    au64 running_connection_count_{0};
    boost::asio::io_service io_service_{};
    UniquePtr<boost::asio::ip::tcp::acceptor> acceptor_ptr_{};
    SharedPtr<String> config_path_{};
};

} // namespace infinity
