//
// Created by JinHai on 2022/7/20.
//

#pragma once
#include "hv/TcpServer.h"
#include "pg_protocol_handler.h"

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace infinity {

enum class SessionStatus : char {
    kIdle,
    kRunning,
    kSuspend,
    kTerminated
};

class Session {
public:
    explicit Session(boost::asio::io_service& io_service);
    void run();

    std::shared_ptr<boost::asio::ip::tcp::socket> socket() { return socket_; }
private:
    void handle_connection();

    void handle_request();

    void handle_simple_query();

    const std::shared_ptr<boost::asio::ip::tcp::socket> socket_;

    const std::shared_ptr<PGProtocolHandler> pg_handler_;

    bool terminate_session_ = false;
};

}


