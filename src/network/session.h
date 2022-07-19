//
// Created by JinHai on 2022/7/20.
//

#pragma once
#include "hv/TcpServer.h"

class Session {
public:
    explicit Session(const hv::SocketChannelPtr& channel);
    void run(hv::Buffer* buffer);
private:
    void handle_connection(hv::Buffer* buffer);
    void handle_request();

    bool terminated_ = false;
    const hv::SocketChannelPtr& channel_;
};


