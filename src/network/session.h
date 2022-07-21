//
// Created by JinHai on 2022/7/20.
//

#pragma once
#include "hv/TcpServer.h"
#include "pg_protocol_handler.h"

namespace infinity {

enum class SessionStatus : char {
    kIdle,
    kRunning,
    kSuspend,
    kTerminated
};

class Session: public std::enable_shared_from_this<Session> {
public:
    explicit Session(const hv::SocketChannelPtr& channel);
    void run(hv::Buffer* buffer);
    void start();
    void suspend();
    void resume();
    void stop();
    uint32_t id() const { return id_; }
    SessionStatus status() { return status_; }
private:
    void handle_connection();
    void handle_request();

    SessionStatus status_ = SessionStatus::kIdle;
    PGProtocolHandler pg_handler_;
    uint32_t id_;

    // mutex for protecting buffer of libhv
    std::mutex mutex_;

    // Condition variable for notify message received
    std::condition_variable waiting_cv_;
};

}


