//
// Created by JinHai on 2022/7/20.
//

#include "session.h"
#include "main/query_handler.h"

#include <iostream>

namespace infinity {

Session::Session(boost::asio::io_service& io_service)
    : socket_(std::make_shared<boost::asio::ip::tcp::socket>(io_service)),
      pg_handler_(std::make_shared<PGProtocolHandler>(socket())){}

void
Session::run() {
    // Disable Nagle's algorithm to reduce TCP latency, but will reduce the throughput.
    socket_->set_option(boost::asio::ip::tcp::no_delay(true));
    handle_connection();
    while(!terminate_session_) {
        try {
            handle_request();
        } catch (const std::exception& e) {
            std::map<PGMessageType, std::string> error_message_map;
            error_message_map[PGMessageType::kHumanReadableError] = e.what();
            std::cout << "Error: " << e.what() << std::endl;
            pg_handler_->send_error_response(error_message_map);
            pg_handler_->send_ready_for_query();
        }

    }
}

void
Session::handle_connection() {
    const auto body_length = pg_handler_->read_startup_header();

    pg_handler_->read_startup_body(body_length);
    pg_handler_->send_authentication();
    pg_handler_->send_parameter("server_version", "14");
    pg_handler_->send_parameter("server_encoding", "UTF8");
    pg_handler_->send_parameter("client_encoding", "UTF8");
    pg_handler_->send_parameter("DateStyle", "IOS, DMY");
    pg_handler_->send_ready_for_query();
}

void
Session::handle_request() {
    const auto cmd_type = pg_handler_->read_command_type();

    switch (cmd_type) {
        case PGMessageType::kBindCommand: {
            std::cout << "BindCommand" << std::endl;
            break;
        }
        case PGMessageType::kDescribeCommand: {
            std::cout << "DescribeCommand" << std::endl;
            break;
        }
        case PGMessageType::kExecuteCommand: {
            std::cout << "ExecuteCommand" << std::endl;
            break;
        }
        case PGMessageType::kParseCommand: {
            std::cout << "ParseCommand" << std::endl;
            break;
        }
        case PGMessageType::kSimpleQueryCommand: {
            handle_simple_query();
            break;
        }
        case PGMessageType::kSyncCommand: {
            std::cout << "SyncCommand" << std::endl;
            break;
        }
        case PGMessageType::kTerminateCommand: {
            terminate_session_ = true;
            break;
        }
        default: {
            Assert(false, "Unknown command type");
        }
    }
}

void
Session::handle_simple_query() {
    const std::string& query = pg_handler_->read_command_body();
    std::cout << "Query: " << query << std::endl;

    QueryHandler::execute_query(query);

    std::map<PGMessageType, std::string> error_message_map;
    std::string response_message = "SimpleQuery: " + query;
    error_message_map[PGMessageType::kHumanReadableError] = response_message;
    pg_handler_->send_error_response(error_message_map);
    pg_handler_->send_ready_for_query();

}

}