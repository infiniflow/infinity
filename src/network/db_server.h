//
// Created by JinHai on 2022/7/19.
//

#pragma once

#include <string>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include "connection.h"
#include "common/singleton.h"

namespace infinity {

struct StartupParameter {
    boost::asio::ip::address address;
    uint16_t    port;
};

class DBServer : public Singleton<DBServer> {
public:
    explicit DBServer(const StartupParameter& parameter);
    void Run();
    void Shutdown();

private:
    void CreateConnection();
    void StartConnection(std::shared_ptr<Connection>& connection);

    std::atomic_bool initialized{false};
    std::atomic_uint64_t running_connection_count_{0};
    boost::asio::io_service io_service_;
    boost::asio::ip::tcp::acceptor acceptor_;
};

}


