//
// Created by JinHai on 2022/7/20.
//

#include "connection.h"
#include "main/query_context.h"
#include "main/logger.h"
#include "main/infinity.h"
#include "main/session.h"
#include "common/utility/infinity_assert.h"

#include <iostream>

namespace infinity {

Connection::Connection(boost::asio::io_service& io_service)
    : socket_(MakeShared<boost::asio::ip::tcp::socket>(io_service)),
      pg_handler_(MakeShared<PGProtocolHandler>(socket())),
      session_(MakeUnique<Session>()){}

void
Connection::Run() {
    // Disable Nagle's algorithm to reduce TCP latency, but will reduce the throughput.
    socket_->set_option(boost::asio::ip::tcp::no_delay(true));

    HandleConnection();

    session_->SetClientInfo(socket_->remote_endpoint().address().to_string(),
                            socket_->remote_endpoint().port());

    while(!terminate_connection_) {
        try {
            HandleRequest();
        } catch (const infinity::ClientException& e) {
            LOG_TRACE("Client is closed");
            return ;
        } catch (const std::exception& e) {
            HashMap<PGMessageType, String> error_message_map;
            error_message_map[PGMessageType::kHumanReadableError] = e.what();
            LOG_ERROR(e.what());
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

    UniquePtr<QueryContext> query_context_ptr
        = MakeUnique<QueryContext>(session_.get(),
                                   Infinity::instance().config(),
                                   Infinity::instance().scheduler(),
                                   Infinity::instance().storage());
    query_context_ptr->set_current_schema(session_->current_database());

    switch (cmd_type) {
        case PGMessageType::kBindCommand: {
            LOG_TRACE("BindCommand");
            break;
        }
        case PGMessageType::kDescribeCommand: {
            LOG_TRACE("DescribeCommand");
            break;
        }
        case PGMessageType::kExecuteCommand: {
            LOG_TRACE("ExecuteCommand");
            break;
        }
        case PGMessageType::kParseCommand: {
            LOG_TRACE("ParseCommand");
            break;
        }
        case PGMessageType::kSimpleQueryCommand: {
            HandlerSimpleQuery(query_context_ptr.get());
            break;
        }
        case PGMessageType::kSyncCommand: {
            LOG_TRACE("SyncCommand");
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
Connection::HandlerSimpleQuery(QueryContext* query_context) {
    const String& query = pg_handler_->read_command_body();
    LOG_TRACE("Query: {}", query);

    // Start to execute the query.
    QueryResult result = query_context->Query(query);

    // Response to the result message to client
    if(result.result_ == nullptr) {
        HashMap<PGMessageType, String> error_message_map;
        String response_message = "Error: " + query;
        error_message_map[PGMessageType::kHumanReadableError] = response_message;
        pg_handler_->send_error_response(error_message_map);
    } else {
        // Have result
        SendTableDescription(result.result_);
        SendQueryResponse(result);
    }

    pg_handler_->send_ready_for_query();

}

void
Connection::SendTableDescription(const SharedPtr<Table>& result_table) {
    u32 column_name_length_sum = 0;
    SizeT column_count = result_table->ColumnCount();
    for(SizeT idx = 0; idx < column_count; ++ idx) {
        column_name_length_sum += result_table->GetColumnNameById(idx).length();
    }

    // No output columns, no need to send table description, just return.
    if (column_name_length_sum == 0) return ;

    pg_handler_->SendDescriptionHeader(column_name_length_sum, column_count);

    for(SizeT idx = 0; idx < column_count; ++ idx) {
        SharedPtr<DataType> column_type = result_table->GetColumnTypeById(idx);

        u32 object_id = 0;
        i16 object_width = 0;

        switch(column_type->type()) {
            case LogicalType::kBoolean: {
                object_id = 16;
                object_width = 1;
                break;
            }
            case LogicalType::kSmallInt: {
                object_id = 21;
                object_width = 2;
                break;
            }
            case LogicalType::kTinyInt: {
                object_id = 18; // char
                object_width = 1;
                break;
            }
            case LogicalType::kInteger: {
                object_id = 23;
                object_width = 4;
                break;
            }
            case LogicalType::kBigInt: {
                object_id = 20;
                object_width = 8;
                break;
            }
            case LogicalType::kFloat: {
                object_id = 700;
                object_width = 4;
                break;
            }
            case LogicalType::kDouble: {
                object_id = 701;
                object_width = 8;
                break;
            }
            case LogicalType::kVarchar: {
                object_id = 25;
                object_width = -1;
                break;
            }
            case LogicalType::kDate: {
                object_id = 1082;
                object_width = 8;
                break;
            }
            case LogicalType::kTime: {
                object_id = 1266;
                object_width = 12;
                break;
            }
            case LogicalType::kInterval: {
                object_id = 1186;
                object_width = 16;
                break;
            }
            default: {
                TypeError("Unexpected type");
            }
        }

        pg_handler_->SendDescription(result_table->GetColumnNameById(idx), object_id, object_width);
    }
}

void
Connection::SendQueryResponse(const QueryResult& query_result) {

    const SharedPtr<Table>& result_table = query_result.result_;
    SizeT column_count = result_table->ColumnCount();
    auto values_as_strings = std::vector<std::optional<String>>(column_count);
    SizeT block_count = result_table->DataBlockCount();
    for(SizeT idx = 0; idx < block_count; ++ idx) {
        auto block = result_table->GetDataBlockById(idx);
        SizeT row_count = block->row_count();

        for(SizeT row_id = 0; row_id < row_count; ++ row_id) {
            SizeT string_length_sum = 0;

            // iterate each column_vector of the block
            for(SizeT column_id = 0; column_id < column_count; ++ column_id) {
                auto& column_vector = block->column_vectors[column_id];
                const String string_value = column_vector->ToString(row_id);
                values_as_strings[column_id] = string_value;
                string_length_sum += string_value.size();
            }
            pg_handler_->SendData(values_as_strings, string_length_sum);
        }
    }

    String message;
    switch (query_result.root_operator_type_) {
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
        case LogicalNodeType::kImport: {
            message = *query_result.result_->result_msg();
            break;
        }
        default: {
            message = "SELECT " + std::to_string(query_result.result_->row_count());
        }
    }

    pg_handler_->SendComplete(message);
}

}