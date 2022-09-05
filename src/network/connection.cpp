//
// Created by JinHai on 2022/7/20.
//

#include "connection.h"
#include "main/query_handler.h"

#include <iostream>

namespace infinity {

Connection::Connection(boost::asio::io_service& io_service)
    : socket_(std::make_shared<boost::asio::ip::tcp::socket>(io_service)),
      pg_handler_(std::make_shared<PGProtocolHandler>(socket())){}

void
Connection::Run() {
    // Disable Nagle's algorithm to reduce TCP latency, but will reduce the throughput.
    socket_->set_option(boost::asio::ip::tcp::no_delay(true));
    HandleConnection();
    while(!terminate_connection_) {
        try {
            HandleRequest();
        } catch (const std::exception& e) {
            std::map<PGMessageType, std::string> error_message_map;
            error_message_map[PGMessageType::kHumanReadableError] = e.what();
            std::cout << e.what() << std::endl;
            pg_handler_->send_error_response(error_message_map);
            pg_handler_->send_ready_for_query();
        }

    }
}

void
Connection::HandleConnection() {
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
Connection::HandleRequest() {
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
            HandlerSimpleQuery();
            break;
        }
        case PGMessageType::kSyncCommand: {
            std::cout << "SyncCommand" << std::endl;
            break;
        }
        case PGMessageType::kTerminateCommand: {
            terminate_connection_ = true;
            break;
        }
        default: {
            NetworkAssert(false, "Unknown command type");
        }
    }
}

void
Connection::HandlerSimpleQuery() {
    const std::string& query = pg_handler_->read_command_body();
    std::cout << "Query: " << query << std::endl;

    // Start to execute the query.
    QueryResult result = QueryHandler::ExecuteQuery(query);

    // Response to the result message to client
    if(result.result_ == nullptr) {
        std::map<PGMessageType, std::string> error_message_map;
        std::string response_message = "Error: " + query;
        error_message_map[PGMessageType::kHumanReadableError] = response_message;
        pg_handler_->send_error_response(error_message_map);
    } else {
        // Have result
        SendTableDescription(result.result_);
        SendQueryResponse(result.result_);
        uint64_t row_count = result.result_->row_count();
        SendComplete(result.root_operator_type_, row_count);
    }

    pg_handler_->send_ready_for_query();

}

void
Connection::SendTableDescription(const std::shared_ptr<Table>& result_table) {
    uint32_t column_name_length_sum = 0;
    for(auto& column: result_table->table_def()->columns()) {
        column_name_length_sum += column.name().size();
    }

    // No output columns, no need to send table description, just return.
    if (column_name_length_sum == 0) return ;

    pg_handler_->SendDescriptionHeader(column_name_length_sum, result_table->table_def()->columns().size());

    for(auto& column: result_table->table_def()->columns()) {
        uint32_t object_id = 0;
        int16_t  object_width = 0;

        switch(column.logical_type().GetTypeId()) {
            case LogicalTypeId::kBoolean:
                object_id = 16;
                object_width = 1;
                break;
            case LogicalTypeId::kSmallInt:
                object_id = 21;
                object_width = 2;
                break;
            case LogicalTypeId::kTinyInt:
                object_id = 18; // char
                object_width = 1;
                break;
            case LogicalTypeId::kInteger:
                object_id = 23;
                object_width = 4;
                break;
            case LogicalTypeId::kBigInt:
                object_id = 20;
                object_width = 8;
                break;
            case LogicalTypeId::kFloat:
                object_id = 700;
                object_width = 4;
                break;
            case LogicalTypeId::kDouble:
                object_id = 701;
                object_width = 8;
                break;
            case LogicalTypeId::kText:
                object_id = 25;
                object_width = -1;
                break;
            case LogicalTypeId::kVarchar:
                object_id = 1043;
                object_width = -1;
                break;
            case LogicalTypeId::kDate:
                object_id = 1082;
                object_width = 8;
                break;
            case LogicalTypeId::kTime:
                object_id = 1266;
                object_width = 12;
                break;
            case LogicalTypeId::kDecimal:
                object_id = 1700;
                object_width = -1;
                break;
            case LogicalTypeId::kInterval:
                object_id = 1186;
                object_width = 16;
                break;
            default:
                NetworkError("Not supported type.");
        }

        pg_handler_->SendDescription(column.name(), object_id, object_width);
    }
}

void
Connection::SendQueryResponse(const std::shared_ptr<Table>& result_table) {
    uint64_t column_count = result_table->table_def()->column_count();
    auto values_as_strings = std::vector<std::optional<std::string>>(column_count);
    uint64_t row_group_count = result_table->block_count();
    for(uint64_t idx = 0; idx < row_group_count; ++ idx) {
        // pg_handler_->SendData(values_as_strings, string_length_sum)
    }
}

void
Connection::SendComplete(LogicalNodeType root_operator_type, uint64_t row_count) {
    std::string message;
    switch (root_operator_type) {
        case LogicalNodeType::kInsert: {
            message = "INSERT 0 1";
            break;
        }
        case LogicalNodeType::kUpdate: {
            message = "UPDATE -1";
            break;
        }
        case LogicalNodeType::kDelete: {
            message = "DELETE -1";
            break;
        }
        default: {
            message = "SELECT " + std::to_string(row_count);
        }
    }

    pg_handler_->SendComplete(message);
}

}