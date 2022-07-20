//
// Created by JinHai on 2022/7/20.
//

#pragma once
#include "hv/TcpServer.h"
#include "pg_protocol_handler.h"

namespace infinity {

class Session {
public:
    explicit Session(const hv::SocketChannelPtr& channel);
    void run(hv::Buffer* buffer);
private:
    void handle_connection();
    void handle_request();

    bool terminated_ = false;
    PGProtocolHandler pg_handler_;
};

}


