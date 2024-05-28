// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

#include <boost/asio/ip/tcp.hpp>

module connection;

import pg_protocol_handler;
import boost;
import stl;
import session;
import infinity_exception;
import internal_types;
import pg_message;
import logger;
import query_context;
import infinity_context;
import third_party;
import data_table;

import logical_node_type;
import query_result;
import session_manager;
import type_info;
import logical_type;
import embedding_info;
import sparse_info;
import data_type;

namespace infinity {

Connection::Connection(boost::asio::io_service &io_service)
    : socket_(MakeShared<boost::asio::ip::tcp::socket>(io_service)), pg_handler_(MakeShared<PGProtocolHandler>(socket())) {}

Connection::~Connection() {
    if (session_ == nullptr) {
        // To avoid null ptr access
        return;
    }
    SessionManager *session_mgr = InfinityContext::instance().session_manager();
    session_mgr->RemoveSessionByID(session_->session_id());
}

void Connection::Run() {
    // Disable Nagle's algorithm to reduce TCP latency, but will reduce the throughput.
    socket_->set_option(boost::asio::ip::tcp::no_delay(true));

    SessionManager *session_manager = InfinityContext::instance().session_manager();
    session_ = session_manager->CreateRemoteSession();

    HandleConnection();

    session_->SetClientInfo(socket_->remote_endpoint().address().to_string(), socket_->remote_endpoint().port());

    while (!terminate_connection_) {
        try {
            HandleRequest();
        } catch (const infinity::RecoverableException &e) {
            LOG_TRACE(fmt::format("Recoverable exception: {}", e.what()));
            return ;
        } catch (const infinity::UnrecoverableException& e) {
            HashMap<PGMessageType, String> error_message_map;
            error_message_map[PGMessageType::kHumanReadableError] = e.what();
            LOG_ERROR(e.what());
            pg_handler_->send_error_response(error_message_map);
            pg_handler_->send_ready_for_query();
        } catch (const std::exception &e) {
            HashMap<PGMessageType, String> error_message_map;
            error_message_map[PGMessageType::kHumanReadableError] = e.what();
            LOG_ERROR(e.what());
            pg_handler_->send_error_response(error_message_map);
            pg_handler_->send_ready_for_query();
        }
    }
}

void Connection::HandleConnection() {
    const auto body_length = pg_handler_->read_startup_header();

    pg_handler_->read_startup_body(body_length);
    pg_handler_->send_authentication();
    pg_handler_->send_parameter("server_version", "14");
    pg_handler_->send_parameter("server_encoding", "UTF8");
    pg_handler_->send_parameter("client_encoding", "UTF8");
    pg_handler_->send_parameter("DateStyle", "IOS, DMY");
    pg_handler_->send_ready_for_query();
}

void Connection::HandleRequest() {
    const auto cmd_type = pg_handler_->read_command_type();

    // FIXME
    UniquePtr<QueryContext> query_context_ptr = MakeUnique<QueryContext>(session_.get());
    query_context_ptr->Init(InfinityContext::instance().config(),
                            InfinityContext::instance().task_scheduler(),
                            InfinityContext::instance().storage(),
                            InfinityContext::instance().resource_manager(),
                            InfinityContext::instance().session_manager());

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
            UnrecoverableError("Unknown PG command type");
        }
    }
}

void Connection::HandlerSimpleQuery(QueryContext *query_context) {
    const String &query = pg_handler_->read_command_body();
    LOG_TRACE(fmt::format("Query: {}", query));

    // Start to execute the query.
    QueryResult result = query_context->Query(query);

    // Response to the result message to client
    if (result.result_table_.get() == nullptr) {
        HashMap<PGMessageType, String> error_message_map;
        error_message_map[PGMessageType::kHumanReadableError] = result.status_.message();
        pg_handler_->send_error_response(error_message_map);
    } else {
        // Have result
        SendTableDescription(result.result_table_);
        SendQueryResponse(result);
    }

    pg_handler_->send_ready_for_query();
}

void Connection::SendTableDescription(const SharedPtr<DataTable> &result_table) {
    u32 column_name_length_sum = 0;
    SizeT column_count = result_table->ColumnCount();
    for (SizeT idx = 0; idx < column_count; ++idx) {
        column_name_length_sum += result_table->GetColumnNameById(idx).length();
    }

    // No output columns, no need to send table description, just return.
    if (column_name_length_sum == 0)
        return;

    pg_handler_->SendDescriptionHeader(column_name_length_sum, column_count);

    for (SizeT idx = 0; idx < column_count; ++idx) {
        SharedPtr<DataType> column_type = result_table->GetColumnTypeById(idx);

        u32 object_id = 0;
        i16 object_width = 0;

        switch (column_type->type()) {
            case LogicalType::kBoolean: {
                object_id = 16;
                object_width = 1;
                break;
            }
            case LogicalType::kTinyInt: {
                object_id = 18; // char
                object_width = 1;
                break;
            }
            case LogicalType::kSmallInt: {
                object_id = 21;
                object_width = 2;
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
                object_id = 1083;
                object_width = 8;
                break;
            }
            case LogicalType::kDateTime: {
                object_id = 1114;
                object_width = 8;
                break;
            }
            case LogicalType::kTimestamp: {
                object_id = 1114;
                object_width = 8;
                break;
            }
            case LogicalType::kInterval: {
                object_id = 1186;
                object_width = 16;
                break;
            }
            case LogicalType::kTensor:
            case LogicalType::kTensorArray:
            case LogicalType::kEmbedding: {
                if (column_type->type_info()->type() != TypeInfoType::kEmbedding) {
                    UnrecoverableError("Not embedding type");
                }

                EmbeddingInfo *embedding_info = static_cast<EmbeddingInfo *>(column_type->type_info().get());
                switch (embedding_info->Type()) {

                    case kElemBit: {
                        object_id = 1000;
                        object_width = 1;
                        break;
                    }
                    case kElemInt8: {
                        object_id = 1002;
                        object_width = 1;
                        break;
                    }
                    case kElemInt16: {
                        object_id = 1005;
                        object_width = 2;
                        break;
                    }
                    case kElemInt32: {
                        object_id = 1007;
                        object_width = 4;
                        break;
                    }
                    case kElemInt64: {
                        object_id = 1016;
                        object_width = 8;
                        break;
                    }
                    case kElemFloat: {
                        object_id = 1021;
                        object_width = 4;
                        break;
                    }
                    case kElemDouble: {
                        object_id = 1022;
                        object_width = 8;
                        break;
                    }
                    case kElemInvalid: {
                        UnrecoverableError("Invalid embedding data type");
                    }
                }
                break;
            }
            case LogicalType::kSparse: {
                if (column_type->type_info()->type() != TypeInfoType::kSparse) {
                    UnrecoverableError("Not sparse type");
                }
                const auto *sparse_info = static_cast<SparseInfo *>(column_type->type_info().get());
                switch (sparse_info->DataType()) {
                    case kElemBit: {
                        UnrecoverableError("Not implemented");
                    }
                    case kElemInt8: {
                        object_id = 1002;
                        object_width = 1;
                        break;
                    }
                    case kElemInt16: {
                        object_id = 1005;
                        object_width = 2;
                        break;
                    }
                    case kElemInt32: {
                        object_id = 1007;
                        object_width = 4;
                        break;
                    }
                    case kElemInt64: {
                        object_id = 1016;
                        object_width = 8;
                        break;
                    }
                    case kElemFloat: {
                        object_id = 1021;
                        object_width = 4;
                        break;
                    }
                    case kElemDouble: {
                        object_id = 1022;
                        object_width = 8;
                        break;
                    }
                    default: {
                        UnrecoverableError("Should not reach here");
                    }
                }
                break;
            }
            default: {
                UnrecoverableError("Unexpected type");
            }
        }

        pg_handler_->SendDescription(result_table->GetColumnNameById(idx), object_id, object_width);
    }
}

void Connection::SendQueryResponse(const QueryResult &query_result) {

    const SharedPtr<DataTable> &result_table = query_result.result_table_;
    SizeT column_count = result_table->ColumnCount();
    auto values_as_strings = Vector<Optional<String>>(column_count);
    SizeT block_count = result_table->DataBlockCount();
    for (SizeT idx = 0; idx < block_count; ++idx) {
        auto block = result_table->GetDataBlockById(idx);
        SizeT row_count = block->row_count();

        for (SizeT row_id = 0; row_id < row_count; ++row_id) {
            SizeT string_length_sum = 0;

            // iterate each column_vector of the block
            for (SizeT column_id = 0; column_id < column_count; ++column_id) {
                auto &column_vector = block->column_vectors[column_id];
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
            message = query_result.ToString();
            break;
        }
        case LogicalNodeType::kImport:
        case LogicalNodeType::kExport: {
            message = *query_result.result_table_->result_msg();
            break;
        }

        default: {
            message = fmt::format("SELECT {}", std::to_string(query_result.result_table_->row_count()));
        }
    }

    pg_handler_->SendComplete(message);
}

} // namespace infinity