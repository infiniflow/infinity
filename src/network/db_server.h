//
// Created by JinHai on 2022/7/19.
//

#pragma once

#include <string>
#include "hv/TcpServer.h"
#include "session.h"

struct StartupParameter {
    std::string address;
    uint16_t    port;
};

class DBServer {
public:
    explicit DBServer(const StartupParameter& parameter);
    void run();
    void shutdown();

private:
    std::string address_;
    uint16_t    port_;
    hv::TcpServer hv_server_;
    std::map<uint32_t, std::shared_ptr<Session>> session_set_;
};




