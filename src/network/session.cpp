//
// Created by JinHai on 2022/7/20.
//

#include "session.h"

#include <iostream>

namespace infinity {

Session::Session(const hv::SocketChannelPtr &channel)
    : pg_handler_(channel) {}

void
Session::run(hv::Buffer* buffer) {
    pg_handler_.set_reader_buffer(buffer);
    handle_connection();
    while(!terminated_) {
        handle_request();
    }
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
            std::cout << "ExecuteCommand" << std::endl;
            break;
        }
        case PGMessageType::kCloseCommand : {
            std::cout << "ExecuteCommand" << std::endl;
            break;
        }
        case PGMessageType::kDescribeCommand : {
            std::cout << "ExecuteCommand" << std::endl;
            break;
        }
        case PGMessageType::kExecuteCommand : {
            std::cout << "ExecuteCommand" << std::endl;
            break;
        }
        case PGMessageType::kFlushCommand : {
            std::cout << "ExecuteCommand" << std::endl;
            break;
        }
        case PGMessageType::kParseCommand : {
            std::cout << "ExecuteCommand" << std::endl;
            break;
        }
        case PGMessageType::kSimpleQueryCommand : {
            std::cout << "ExecuteCommand" << std::endl;
            break;
        }
        case PGMessageType::kSyncCommand : {
            std::cout << "ExecuteCommand" << std::endl;
            break;
        }
        case PGMessageType::kTerminateCommand : {
            std::cout << "ExecuteCommand" << std::endl;
            terminated_ = true;
            break;
        }
        default: {
            Assert(false, "Unknown command type.");
        }
    }
}

}