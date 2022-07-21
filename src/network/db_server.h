//
// Created by JinHai on 2022/7/19.
//

#pragma once

#include <string>
#include "hv/TcpServer.h"
#include "session.h"
#include "common/singleton.h"

namespace infinity {

struct StartupParameter {
    std::string address;
    uint16_t    port;
};

class DBServer : public Singleton<DBServer> {
public:
    explicit DBServer(const StartupParameter& parameter);
    void run();
    void shutdown();

private:
    void create_session(const hv::SocketChannelPtr& channel);
    void start_session(std::shared_ptr<Session> session, hv::Buffer* buf);
    std::shared_ptr<Session> get_session_by_id(uint32_t id);
    void destroy_session(uint32_t id);

    std::string address_;
    uint16_t    port_;
    hv::TcpServer hv_server_;

    std::map<uint32_t, std::shared_ptr<Session>> session_set_;
    std::mutex session_mutex_;

    uint64_t running_session_count = 0;
};

}


