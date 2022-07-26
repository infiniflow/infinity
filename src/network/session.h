//
// Created by JinHai on 2022/7/20.
//

#pragma once
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
    void Run();

    std::shared_ptr<boost::asio::ip::tcp::socket> socket() { return socket_; }
private:
    void HandleConnection();

    void HandleRequest();

    void HandlerSimpleQuery();

    const std::shared_ptr<boost::asio::ip::tcp::socket> socket_;

    const std::shared_ptr<PGProtocolHandler> pg_handler_;

    bool terminate_session_ = false;
};

}


