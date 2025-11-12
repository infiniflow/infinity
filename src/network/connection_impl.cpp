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

module infinity_core:connection.impl;

import :connection;
import :pg_protocol_handler;
import :boost;
import :infinity_exception;
import :pg_message;
import :infinity_context;
import :data_table;
import :logical_node_type;
import :session_manager;
import :query_context;

import std.compat;
import third_party;

import type_info;
import logical_type;
import embedding_info;
import sparse_info;
import data_type;
import global_resource_usage;
import internal_types;

namespace infinity {

Connection::Connection(boost::asio::io_context &io_context)
    : socket_(std::make_shared<boost::asio::ip::tcp::socket>(io_context)), pg_handler_(std::make_shared<PGProtocolHandler>(socket())) {}

Connection::~Connection() {
    if (session_ == nullptr) {
        // To avoid null ptr access
        return;
    }
    SessionManager *session_mgr = InfinityContext::instance().session_manager();
    session_mgr->RemoveSessionByID(session_->session_id());
}

void Connection::HandleError(const char *error_msg) {
    std::unordered_map<PGMessageType, std::string> error_message_map;
    error_message_map[PGMessageType::kHumanReadableError] = error_msg;
    LOG_ERROR(error_msg);
    pg_handler_->send_error_response(error_message_map);
    pg_handler_->send_ready_for_query();
}

void Connection::Run() {
    // Disable Nagle's algorithm to reduce TCP latency, but will reduce the throughput.
    socket_->set_option(boost::asio::ip::tcp::no_delay(true));

    SessionManager *session_manager = InfinityContext::instance().session_manager();
    std::shared_ptr<RemoteSession> remote_session = session_manager->CreateRemoteSession();
    if (remote_session == nullptr) {
        HandleError("Infinity is running under maintenance mode, only one connection is allowed.");
        return;
    }

    session_ = std::move(remote_session);

    HandleConnection();

    session_->SetClientInfo(socket_->remote_endpoint().address().to_string(), socket_->remote_endpoint().port());

    while (!terminate_connection_) {
        try {
            HandleRequest();
        } catch (const infinity::RecoverableException &e) {
            LOG_TRACE(fmt::format("Recoverable exception: {}", e.what()));
            return;
        } catch (const infinity::UnrecoverableException &e) {
            HandleError(e.what());
        } catch (const std::exception &e) {
            HandleError(e.what());
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
    std::unique_ptr<QueryContext> query_context_ptr = std::make_unique<QueryContext>(session_.get());
    query_context_ptr->Init(InfinityContext::instance().config(),
                            InfinityContext::instance().task_scheduler(),
                            InfinityContext::instance().storage(),
                            InfinityContext::instance().resource_manager(),
                            InfinityContext::instance().session_manager(),
                            InfinityContext::instance().persistence_manager());

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
    const std::string &query = pg_handler_->read_command_body();
    LOG_TRACE(fmt::format("Query: {}", query));

    // Start to execute the query.
    QueryResult result = query_context->Query(query);

    // Response to the result message to client
    if (result.result_table_.get() == nullptr) {
        std::unordered_map<PGMessageType, std::string> error_message_map;
        error_message_map[PGMessageType::kHumanReadableError] = result.status_.message();
        pg_handler_->send_error_response(error_message_map);
    } else {
        // Have result
        SendTableDescription(result.result_table_);
        SendQueryResponse(result);
    }

    pg_handler_->send_ready_for_query();
}

void Connection::SendTableDescription(const std::shared_ptr<DataTable> &result_table) {
    u32 column_name_length_sum = 0;
    size_t column_count = result_table->ColumnCount();
    for (size_t idx = 0; idx < column_count; ++idx) {
        column_name_length_sum += result_table->GetColumnNameById(idx).length();
    }

    // No output columns, no need to send table description, just return.
    if (column_name_length_sum == 0)
        return;

    pg_handler_->SendDescriptionHeader(column_name_length_sum, column_count);

    for (size_t idx = 0; idx < column_count; ++idx) {
        std::shared_ptr<DataType> column_type = result_table->GetColumnTypeById(idx);

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
            case LogicalType::kFloat16:
            case LogicalType::kBFloat16:
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
            case LogicalType::kArray: {
                object_id = 25;
                object_width = -1;
                break;
            }
            case LogicalType::kTensor:
            case LogicalType::kTensorArray:
            case LogicalType::kMultiVector:
            case LogicalType::kEmbedding: {
                if (column_type->type_info()->type() != TypeInfoType::kEmbedding) {
                    UnrecoverableError("Not embedding type");
                }

                auto *embedding_info = static_cast<EmbeddingInfo *>(column_type->type_info().get());
                switch (embedding_info->Type()) {

                    case EmbeddingDataType::kElemBit: {
                        object_id = 1000;
                        object_width = 1;
                        break;
                    }
                    case EmbeddingDataType::kElemUInt8:
                    case EmbeddingDataType::kElemInt8: {
                        object_id = 1002;
                        object_width = 1;
                        break;
                    }
                    case EmbeddingDataType::kElemInt16: {
                        object_id = 1005;
                        object_width = 2;
                        break;
                    }
                    case EmbeddingDataType::kElemInt32: {
                        object_id = 1007;
                        object_width = 4;
                        break;
                    }
                    case EmbeddingDataType::kElemInt64: {
                        object_id = 1016;
                        object_width = 8;
                        break;
                    }
                    case EmbeddingDataType::kElemFloat16:
                    case EmbeddingDataType::kElemBFloat16:
                    case EmbeddingDataType::kElemFloat: {
                        object_id = 1021;
                        object_width = 4;
                        break;
                    }
                    case EmbeddingDataType::kElemDouble: {
                        object_id = 1022;
                        object_width = 8;
                        break;
                    }
                    case EmbeddingDataType::kElemInvalid: {
                        UnrecoverableError("Invalid embedding data type");
                    }
                }
                break;
            }
            case LogicalType::kSparse: {
                if (column_type->type_info()->type() != TypeInfoType::kSparse) {
                    UnrecoverableError("Not sparse type");
                }
                switch (const auto *sparse_info = static_cast<SparseInfo *>(column_type->type_info().get()); sparse_info->DataType()) {
                    case EmbeddingDataType::kElemBit: {
                        object_id = 1000;
                        object_width = 1;
                        break;
                    }
                    case EmbeddingDataType::kElemUInt8:
                    case EmbeddingDataType::kElemInt8: {
                        object_id = 1002;
                        object_width = 1;
                        break;
                    }
                    case EmbeddingDataType::kElemInt16: {
                        object_id = 1005;
                        object_width = 2;
                        break;
                    }
                    case EmbeddingDataType::kElemInt32: {
                        object_id = 1007;
                        object_width = 4;
                        break;
                    }
                    case EmbeddingDataType::kElemInt64: {
                        object_id = 1016;
                        object_width = 8;
                        break;
                    }
                    case EmbeddingDataType::kElemFloat16:
                    case EmbeddingDataType::kElemBFloat16:
                    case EmbeddingDataType::kElemFloat: {
                        object_id = 1021;
                        object_width = 4;
                        break;
                    }
                    case EmbeddingDataType::kElemDouble: {
                        object_id = 1022;
                        object_width = 8;
                        break;
                    }
                    case EmbeddingDataType::kElemInvalid: {
                        UnrecoverableError("Should not reach here");
                    }
                }
                break;
            }
            default: {
                LOG_ERROR("Unexpected type");
                UnrecoverableError("Unexpected type");
            }
        }

        pg_handler_->SendDescription(result_table->GetColumnNameById(idx), object_id, object_width);
    }
}

void Connection::SendQueryResponse(const QueryResult &query_result) {

    const std::shared_ptr<DataTable> &result_table = query_result.result_table_;
    size_t column_count = result_table->ColumnCount();
    auto values_as_strings = std::vector<std::optional<std::string>>(column_count);
    size_t block_count = result_table->DataBlockCount();
    for (size_t idx = 0; idx < block_count; ++idx) {
        auto block = result_table->GetDataBlockById(idx);
        size_t row_count = block->row_count();

        for (size_t row_id = 0; row_id < row_count; ++row_id) {
            size_t string_length_sum = 0;

            // iterate each column_vector of the block
            for (size_t column_id = 0; column_id < column_count; ++column_id) {
                auto &column_vector = block->column_vectors_[column_id];
                const std::string string_value = column_vector->ToString(row_id);
                values_as_strings[column_id] = string_value;
                string_length_sum += string_value.size();
            }
            pg_handler_->SendData(values_as_strings, string_length_sum);
        }
    }

    std::string message;
    switch (query_result.root_operator_type_) {
        case LogicalNodeType::kInsert: {
            message = query_result.ToString();
            break;
        }
        case LogicalNodeType::kImport:
        case LogicalNodeType::kExport:
        case LogicalNodeType::kCompact: {
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