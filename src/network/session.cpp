//
// Created by JinHai on 2022/7/20.
//

#include "session.h"

#include <iostream>

namespace infinity {

Session::Session(const hv::SocketChannelPtr &channel)
    : pg_handler_(channel), id_(channel->id()) {}

void
Session::run(hv::Buffer* buffer) {
    start();
    pg_handler_.set_session(shared_from_this());
    pg_handler_.set_reader_buffer(buffer);
    handle_connection();
    while(status_ == SessionStatus::kRunning) {
        handle_request();
    }
}

void
Session::start() {
    std::unique_lock<std::mutex> lock(mutex_);
    status_ = SessionStatus::kRunning;
}

void
Session::stop() {
    std::unique_lock<std::mutex> lock(mutex_);
    status_ = SessionStatus::kTerminated;
}

void
Session::suspend() {
    std::unique_lock<std::mutex> lock(mutex_);
    status_ = SessionStatus::kSuspend;
    waiting_cv_.wait(lock, [&] { return status_ == SessionStatus::kRunning; });
}

void
Session::resume() {
    std::unique_lock<std::mutex> lock(mutex_);
    status_ = SessionStatus::kRunning;
    waiting_cv_.notify_one();
}

void
Session::handle_connection() {
    // TODO: SSL not supported in header
    const auto body_length = pg_handler_.read_startup_header();

    // TODO: no message is ingested from startup message
    pg_handler_.read_startup_body(body_length);

    // TODO: always authenticate user successfully
    pg_handler_.send_authentication();

    pg_handler_.send_parameter("server_version", "14");
    pg_handler_.send_parameter("server_encoding", "UTF8");
    pg_handler_.send_parameter("client_encoding", "UTF8");
    pg_handler_.send_parameter("DataStyle", "ISO, DMY");

    pg_handler_.send_ready_for_query();
}

void
Session::handle_request() {
    const PGMessageType message_type = pg_handler_.read_message_type();
    switch (message_type) {
        case PGMessageType::kBindCommand : {
            std::cout << "BindCommand" << std::endl;
            break;
        }
        case PGMessageType::kCloseCommand : {
            std::cout << "CloseCommand" << std::endl;
            break;
        }
        case PGMessageType::kDescribeCommand : {
            std::cout << "DescribeCommand" << std::endl;
            break;
        }
        case PGMessageType::kExecuteCommand : {
            std::cout << "ExecuteCommand" << std::endl;
            break;
        }
        case PGMessageType::kFlushCommand : {
            std::cout << "FlushCommand" << std::endl;
            break;
        }
        case PGMessageType::kParseCommand : {
            std::cout << "ParseCommand" << std::endl;
            break;
        }
        case PGMessageType::kSimpleQueryCommand : {
            std::cout << "SimpleQueryCommand" << std::endl;
            break;
        }
        case PGMessageType::kSyncCommand : {
            std::cout << "SyncCommand" << std::endl;
            break;
        }
        case PGMessageType::kTerminateCommand : {
            std::cout << "TerminateCommand" << std::endl;
            status_ = SessionStatus::kTerminated;
            break;
        }
        default: {
            Assert(false, "Unknown command type.");
        }
    }
}

}