// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

#include <cstring>
#include <string>
#include <vector>

module infinity_thrift_service;

import third_party;
import logger;
import query_options;
import infinity_thrift_types;
import infinity;
import stl;
import infinity_context;
import file_writer;
import value;
import infinity_exception;
import logical_type;
import internal_types;

import column_def;
import statement_common;
import data_type;
import status;
import embedding_info;
import sparse_info;
import in_expr;
import constant_expr;
import column_expr;
import function_expr;
import knn_expr;
import match_sparse_expr;
import match_tensor_expr;
import match_expr;
import fusion_expr;
import parsed_expr;
import update_statement;
import search_expr;
import explain_statement;
import create_index_info;
import command_statement;
import select_statement;
import data_block;
import table_def;
import extra_ddl_info;
import defer_op;

import column_vector;
import query_result;

namespace infinity {

ClientVersions::ClientVersions() {
    client_version_map_[1] = String("0.2.0.dev2");
    client_version_map_[2] = String("0.2.0.dev3");
    client_version_map_[3] = String("0.2.0.dev4");
    client_version_map_[4] = String("0.2.0.dev5");
    client_version_map_[5] = String("0.2.0.dev6");
    client_version_map_[6] = String("0.2.0.dev7");
    client_version_map_[7] = String("0.2.0.dev8");
    client_version_map_[8] = String("0.2.0");
    client_version_map_[9] = String("0.2.1.dev5");
    client_version_map_[10] = String("0.2.1");
    client_version_map_[11] = String("0.3.0.dev1");
    client_version_map_[12] = String("0.3.0.dev3");
    client_version_map_[13] = String("0.3.0.dev4");
    client_version_map_[14] = String("0.3.0.dev5");
    client_version_map_[15] = String("0.3.0.dev6");
    client_version_map_[16] = String("0.3.0.dev7");
    client_version_map_[17] = String("0.3.0");
    client_version_map_[18] = String("0.4.0.dev1");
    client_version_map_[19] = String("0.4.0.dev2");
    client_version_map_[20] = String("0.4.0.dev3");
    client_version_map_[21] = String("0.4.0.dev4");
}

Pair<const char *, Status> ClientVersions::GetVersionByIndex(i64 version_index) {
    auto iter = client_version_map_.find(version_index);
    if (iter == client_version_map_.end()) {
        return {nullptr, Status::UnsupportedVersionIndex(version_index)};
    }
    return {iter->second.c_str(), Status::OK()};
}

std::mutex InfinityThriftService::infinity_session_map_mutex_;
HashMap<u64, SharedPtr<Infinity>> InfinityThriftService::infinity_session_map_;
ClientVersions InfinityThriftService::client_version_;

void InfinityThriftService::Connect(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::ConnectRequest &request) {
    i64 request_client_version = request.client_version;
    if (request_client_version != current_version_index_) {
        auto [request_version_ptr, status1] = client_version_.GetVersionByIndex(request_client_version);
        if (!status1.ok()) {
            response.__set_error_code((i64)(status1.code()));
            response.__set_error_msg(status1.message());
            LOG_ERROR(status1.message());
            return;
        }

        auto [expected_version_ptr, status2] = client_version_.GetVersionByIndex(current_version_index_);
        if (!status2.ok()) {
            UnrecoverableError(status2.message());
        }

        Status status3 = Status::ClientVersionMismatch(expected_version_ptr, request_version_ptr);
        response.__set_error_code((i64)(status3.code()));
        response.__set_error_msg(status3.message());
        LOG_ERROR(status3.message());
        return;
    }

    auto infinity = Infinity::RemoteConnect();
    std::lock_guard<std::mutex> lock(infinity_session_map_mutex_);
    infinity_session_map_.emplace(infinity->GetSessionId(), infinity);
    response.__set_session_id(infinity->GetSessionId());
    response.__set_error_code((i64)(ErrorCode::kOk));
    LOG_TRACE(fmt::format("THRIFT: Connect success, new session {}", response.session_id));
}

void InfinityThriftService::Disconnect(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::CommonRequest &request) {
    auto status = GetAndRemoveSessionID(request.session_id);
    if (status.ok()) {
        response.__set_error_code((i64)(status.code()));
        LOG_TRACE(fmt::format("THRIFT: Disconnect session {} success", request.session_id));
    } else {
        response.__set_error_code((i64)(status.code()));
        response.__set_error_msg(status.message());
        LOG_TRACE(fmt::format("THRIFT: Disconnect session {} failed", request.session_id));
    }
}

void InfinityThriftService::CreateDatabase(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::CreateDatabaseRequest &request) {
    CreateDatabaseOptions create_database_opts;
    switch (request.create_option.conflict_type) {
        case infinity_thrift_rpc::CreateConflict::Ignore: {
            create_database_opts.conflict_type_ = ConflictType::kIgnore;
            break;
        }
        case infinity_thrift_rpc::CreateConflict::Error: {
            create_database_opts.conflict_type_ = ConflictType::kError;
            break;
        }
        case infinity_thrift_rpc::CreateConflict::Replace: {
            create_database_opts.conflict_type_ = ConflictType::kReplace;
            break;
        }
        default: {
            ProcessStatus(response, Status::InvalidConflictType());
            return;
        }
    }

    auto [infinity, status] = GetInfinityBySessionID(request.session_id);
    if (status.ok()) {
        auto result = infinity->CreateDatabase(request.db_name, create_database_opts);
        ProcessQueryResult(response, result);
    } else {
        ProcessStatus(response, status);
    }
}

void InfinityThriftService::DropDatabase(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::DropDatabaseRequest &request) {

    DropDatabaseOptions drop_database_opts;
    switch (request.drop_option.conflict_type) {
        case infinity_thrift_rpc::DropConflict::Ignore: {
            drop_database_opts.conflict_type_ = ConflictType::kIgnore;
            break;
        }
        case infinity_thrift_rpc::DropConflict::Error: {
            drop_database_opts.conflict_type_ = ConflictType::kError;
            break;
        }
        default: {
            ProcessStatus(response, Status::InvalidConflictType());
            return;
        }
    }

    auto [infinity, status] = GetInfinityBySessionID(request.session_id);
    if (status.ok()) {
        auto result = infinity->DropDatabase(request.db_name, drop_database_opts);
        ProcessQueryResult(response, result);
    } else {
        ProcessStatus(response, status);
    }
}

void InfinityThriftService::CreateTable(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::CreateTableRequest &request) {
    Vector<ColumnDef *> column_defs;

    for (auto &proto_column_def : request.column_defs) {
        auto [column_def, column_def_status] = GetColumnDefFromProto(proto_column_def);
        if (!column_def_status.ok()) {
            ProcessStatus(response, column_def_status);
            return;
        }
        column_defs.emplace_back(column_def);
    }

    CreateTableOptions create_table_opts;
    switch (request.create_option.conflict_type) {
        case infinity_thrift_rpc::CreateConflict::Ignore: {
            create_table_opts.conflict_type_ = ConflictType::kIgnore;
            break;
        }
        case infinity_thrift_rpc::CreateConflict::Error: {
            create_table_opts.conflict_type_ = ConflictType::kError;
            break;
        }
        case infinity_thrift_rpc::CreateConflict::Replace: {
            create_table_opts.conflict_type_ = ConflictType::kReplace;
            break;
        }
        default: {
            ProcessStatus(response, Status::InvalidConflictType());
            return;
        }
    }

    SizeT properties_count = request.create_option.properties.size();
    create_table_opts.properties_.reserve(properties_count);
    for (SizeT idx = 0; idx < properties_count; ++idx) {
        InitParameter *property = new InitParameter();
        property->param_name_ = request.create_option.properties[idx].key;
        property->param_value_ = request.create_option.properties[idx].value;
        create_table_opts.properties_.emplace_back(property);
    }

    auto [infinity, infinity_status] = GetInfinityBySessionID(request.session_id);
    if (!infinity_status.ok()) {
        ProcessStatus(response, infinity_status);
        return;
    }

    auto result = infinity->CreateTable(request.db_name, request.table_name, column_defs, Vector<TableConstraint *>(), create_table_opts);
    ProcessQueryResult(response, result);
}

void InfinityThriftService::DropTable(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::DropTableRequest &request) {
    auto [infinity, infinity_status] = GetInfinityBySessionID(request.session_id);
    if (!infinity_status.ok()) {
        ProcessStatus(response, infinity_status);
        return;
    }

    DropTableOptions drop_table_opts;
    switch (request.drop_option.conflict_type) {
        case infinity_thrift_rpc::DropConflict::Ignore: {
            drop_table_opts.conflict_type_ = ConflictType::kIgnore;
            break;
        }
        case infinity_thrift_rpc::DropConflict::Error: {
            drop_table_opts.conflict_type_ = ConflictType::kError;
            break;
        }
        default: {
            ProcessStatus(response, Status::InvalidConflictType());
            return;
        }
    }

    auto result = infinity->DropTable(request.db_name, request.table_name, drop_table_opts);
    ProcessQueryResult(response, result);
}

void InfinityThriftService::Insert(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::InsertRequest &request) {
    auto [infinity, infinity_status] = GetInfinityBySessionID(request.session_id);
    if (!infinity_status.ok()) {
        ProcessStatus(response, infinity_status);
        return;
    }

    if (request.fields.empty()) {
        ProcessStatus(response, Status::InsertWithoutValues());
        return;
    }

    auto columns = new Vector<String>();
    columns->reserve(request.column_names.size());
    for (auto &column : request.column_names) {
        columns->emplace_back(column);
    }

    Status constant_status;

    Vector<Vector<ParsedExpr *> *> *values = new Vector<Vector<ParsedExpr *> *>();
    values->reserve(request.fields.size());
    for (auto &value : request.fields) {
        auto value_list = new Vector<ParsedExpr *>();
        value_list->reserve(value.parse_exprs.size());
        for (auto &expr : value.parse_exprs) {
            auto parsed_expr = GetConstantFromProto(constant_status, *expr.type.constant_expr);
            if (!constant_status.ok()) {
                // Free values memory
                if (values != nullptr) {
                    for (auto &value_array : *values) {
                        for (auto &value_ptr : *value_array) {
                            delete value_ptr;
                            value_ptr = nullptr;
                        }
                        delete value_array;
                        value_array = nullptr;
                    }
                    delete values;
                    values = nullptr;
                }
                // Free current value list memory
                if (value_list != nullptr) {
                    for (auto &value_ptr : *value_list) {
                        delete value_ptr;
                        value_ptr = nullptr;
                    }
                    delete value_list;
                    value_list = nullptr;
                }

                if (parsed_expr != nullptr) {
                    delete parsed_expr;
                    parsed_expr = nullptr;
                }

                ProcessStatus(response, constant_status);
                return;
            }
            value_list->emplace_back(parsed_expr);
        }
        values->emplace_back(value_list);
    }

    auto result = infinity->Insert(request.db_name, request.table_name, columns, values);
    ProcessQueryResult(response, result);
}

Tuple<CopyFileType, Status> InfinityThriftService::GetCopyFileType(infinity_thrift_rpc::CopyFileType::type copy_file_type) {
    switch (copy_file_type) {
        case infinity_thrift_rpc::CopyFileType::CSV:
            return {CopyFileType::kCSV, Status::OK()};
        case infinity_thrift_rpc::CopyFileType::JSON:
            return {CopyFileType::kJSON, Status::OK()};
        case infinity_thrift_rpc::CopyFileType::JSONL:
            return {CopyFileType::kJSONL, Status::OK()};
        case infinity_thrift_rpc::CopyFileType::FVECS:
            return {CopyFileType::kFVECS, Status::OK()};
        case infinity_thrift_rpc::CopyFileType::CSR:
            return {CopyFileType::kCSR, Status::OK()};
        case infinity_thrift_rpc::CopyFileType::BVECS:
            return {CopyFileType::kBVECS, Status::OK()};
        default: {
            return {CopyFileType::kInvalid, Status::ImportFileFormatError("Not implemented yet")};
        }
    }
}

void InfinityThriftService::Import(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::ImportRequest &request) {
    auto [infinity, infinity_status] = GetInfinityBySessionID(request.session_id);
    if (!infinity_status.ok()) {
        ProcessStatus(response, infinity_status);
        return;
    }

    auto [copy_file_type, status] = GetCopyFileType(request.import_option.copy_file_type);
    if (!status.ok()) {
        ProcessStatus(response, status);
    }

    ImportOptions import_options;
    import_options.copy_file_type_ = copy_file_type;
    auto &delimiter_string = request.import_option.delimiter;
    if (import_options.copy_file_type_ == CopyFileType::kCSV && delimiter_string.size() != 1) {
        ProcessStatus(response, Status::SyntaxError("CSV file delimiter isn't a char."));
    }
    import_options.delimiter_ = delimiter_string[0];

    const QueryResult result = infinity->Import(request.db_name, request.table_name, request.file_name.c_str(), import_options);
    ProcessQueryResult(response, result);
}

void InfinityThriftService::Export(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::ExportRequest &request) {
    auto [infinity, infinity_status] = GetInfinityBySessionID(request.session_id);
    if (!infinity_status.ok()) {
        ProcessStatus(response, infinity_status);
        return;
    }

    auto [copy_file_type, status] = GetCopyFileType(request.export_option.copy_file_type);
    if (!status.ok()) {
        ProcessStatus(response, status);
    }

    Vector<ParsedExpr *> *export_columns = new Vector<ParsedExpr *>();
    export_columns->reserve(request.columns.size());

    for (String column_name : request.columns) {
        ToLower(column_name);
        if (column_name == "_row_id") {
            FunctionExpr *expr = new FunctionExpr();
            expr->func_name_ = "row_id";
            export_columns->emplace_back(expr);
        } else if (column_name == "_create_timestamp") {
            FunctionExpr *expr = new FunctionExpr();
            expr->func_name_ = "create_timestamp";
            export_columns->emplace_back(expr);
        } else if (column_name == "_delete_timestamp") {
            FunctionExpr *expr = new FunctionExpr();
            expr->func_name_ = "delete_timestamp";
            export_columns->emplace_back(expr);
        } else {
            ColumnExpr *expr = new ColumnExpr();
            expr->names_.emplace_back(column_name);
            export_columns->emplace_back(expr);
        }
    }

    ExportOptions export_options;
    export_options.copy_file_type_ = copy_file_type;
    auto &delimiter_string = request.export_option.delimiter;
    if (export_options.copy_file_type_ == CopyFileType::kCSV && delimiter_string.size() != 1) {
        ProcessStatus(response, Status::SyntaxError("CSV file delimiter isn't a char."));
    }
    export_options.delimiter_ = delimiter_string[0];
    export_options.offset_ = request.export_option.offset;
    export_options.limit_ = request.export_option.limit;
    export_options.row_limit_ = request.export_option.row_limit;

    const QueryResult result = infinity->Export(request.db_name, request.table_name, export_columns, request.file_name.c_str(), export_options);
    ProcessQueryResult(response, result);
}

void InfinityThriftService::Select(infinity_thrift_rpc::SelectResponse &response, const infinity_thrift_rpc::SelectRequest &request) {
    // ++count_;
    // auto start1 = std::chrono::steady_clock::now();

    auto [infinity, infinity_status] = GetInfinityBySessionID(request.session_id);
    if (!infinity_status.ok()) {
        ProcessStatus(response, infinity_status);
        return;
    }

    // auto end1 = std::chrono::steady_clock::now();
    //
    // phase_1_duration_ += end1 - start1;
    //
    // auto start2 = std::chrono::steady_clock::now();

    // select list
    if (request.__isset.select_list == false or request.select_list.empty()) {
        ProcessStatus(response, Status::EmptySelectFields());
        return;
    }

    Vector<ParsedExpr *> *output_columns = new Vector<ParsedExpr *>();
    DeferFn defer_fn1([&]() {
        if (output_columns != nullptr) {
            for (auto &expr_ptr : *output_columns) {
                delete expr_ptr;
                expr_ptr = nullptr;
            }
            delete output_columns;
            output_columns = nullptr;
        }
    });
    output_columns->reserve(request.select_list.size());

    Status parsed_expr_status;
    for (auto &expr : request.select_list) {
        auto parsed_expr = GetParsedExprFromProto(parsed_expr_status, expr);
        DeferFn defer_fn4([&]() {
            if (parsed_expr != nullptr) {
                delete parsed_expr;
                parsed_expr = nullptr;
            }
        });

        if (!parsed_expr_status.ok()) {
            ProcessStatus(response, parsed_expr_status);
            return;
        }
        output_columns->emplace_back(parsed_expr);
        parsed_expr = nullptr;
    }

    // highlight list
    Vector<ParsedExpr *> *highlight_columns = nullptr;
    DeferFn defer_fn11([&]() {
        if (highlight_columns != nullptr) {
            for (auto &expr_ptr : *highlight_columns) {
                delete expr_ptr;
                expr_ptr = nullptr;
            }
            delete highlight_columns;
            highlight_columns = nullptr;
        }
    });
    if (!request.highlight_list.empty()) {
        highlight_columns = new Vector<ParsedExpr *>();
        highlight_columns->reserve(request.highlight_list.size());
        for (auto &expr : request.highlight_list) {
            auto parsed_expr = GetParsedExprFromProto(parsed_expr_status, expr);
            DeferFn defer_fn4([&]() {
                if (parsed_expr != nullptr) {
                    delete parsed_expr;
                    parsed_expr = nullptr;
                }
            });

            if (!parsed_expr_status.ok()) {
                ProcessStatus(response, parsed_expr_status);
                return;
            }
            highlight_columns->emplace_back(parsed_expr);
            parsed_expr = nullptr;
        }
    }

    // search expr
    SearchExpr *search_expr = nullptr;
    DeferFn defer_fn5([&]() {
        if (search_expr != nullptr) {
            delete search_expr;
            search_expr = nullptr;
        }
    });
    if (request.__isset.search_expr) {

        auto search_expr_list = new Vector<ParsedExpr *>();
        DeferFn defer_fn2([&]() {
            if (search_expr_list != nullptr) {
                for (auto &expr_ptr : *search_expr_list) {
                    delete expr_ptr;
                    expr_ptr = nullptr;
                }
                delete search_expr_list;
                search_expr_list = nullptr;
            }
        });

        SizeT match_expr_count = request.search_expr.match_exprs.size();
        SizeT fusion_expr_count = request.search_expr.fusion_exprs.size();
        SizeT total_expr_count = match_expr_count + fusion_expr_count;
        search_expr_list->reserve(total_expr_count);
        for (SizeT idx = 0; idx < match_expr_count; ++idx) {
            Status status;
            ParsedExpr *match_expr = GetGenericMatchExprFromProto(status, request.search_expr.match_exprs[idx]);
            DeferFn defer_fn3([&]() {
                if (match_expr != nullptr) {
                    delete match_expr;
                    match_expr = nullptr;
                }
            });
            if (!status.ok()) {
                ProcessStatus(response, status);
                return;
            }
            search_expr_list->emplace_back(match_expr);
            match_expr = nullptr;
        }

        for (SizeT idx = 0; idx < fusion_expr_count; ++idx) {
            ParsedExpr *fusion_expr = GetFusionExprFromProto(request.search_expr.fusion_exprs[idx]);
            search_expr_list->emplace_back(fusion_expr);
        }

        search_expr = new SearchExpr();
        search_expr->SetExprs(search_expr_list);
        search_expr_list = nullptr;
    }

    // filter
    ParsedExpr *filter = nullptr;
    DeferFn defer_fn6([&]() {
        if (filter != nullptr) {
            delete filter;
            filter = nullptr;
        }
    });
    if (request.__isset.where_expr == true) {
        filter = GetParsedExprFromProto(parsed_expr_status, request.where_expr);
        if (!parsed_expr_status.ok()) {
            ProcessStatus(response, parsed_expr_status);
            return;
        }
    }

    // Limit
    ParsedExpr *limit = nullptr;
    DeferFn defer_fn7([&]() {
        if (limit != nullptr) {
            delete limit;
            limit = nullptr;
        }
    });
    if (request.__isset.limit_expr == true) {
        limit = GetParsedExprFromProto(parsed_expr_status, request.limit_expr);
        if (!parsed_expr_status.ok()) {
            ProcessStatus(response, parsed_expr_status);
            return;
        }
    }

    // Offset
    ParsedExpr *offset = nullptr;
    DeferFn defer_fn8([&]() {
        if (offset != nullptr) {
            delete offset;
            offset = nullptr;
        }
    });
    if (request.__isset.offset_expr == true) {
        offset = GetParsedExprFromProto(parsed_expr_status, request.offset_expr);
        if (!parsed_expr_status.ok()) {
            ProcessStatus(response, parsed_expr_status);
            return;
        }
    }

    // order by
    Vector<OrderByExpr *> *order_by_list = nullptr;
    DeferFn defer_fn9([&]() {
        if (order_by_list != nullptr) {
            for (auto &expr_ptr : *order_by_list) {
                delete expr_ptr;
                expr_ptr = nullptr;
            }
            delete order_by_list;
            order_by_list = nullptr;
        }
    });
    if (!request.order_by_list.empty()) {
        order_by_list = new Vector<OrderByExpr *>();
        order_by_list->reserve(request.order_by_list.size());

        Status order_by_status;
        for (auto &expr : request.order_by_list) {
            auto order_by_expr = GetOrderByExprFromProto(order_by_status, expr);
            DeferFn defer_fn4([&]() {
                if (order_by_expr != nullptr) {
                    delete order_by_expr;
                    order_by_expr = nullptr;
                }
            });

            if (!order_by_status.ok()) {
                ProcessStatus(response, order_by_status);
                return;
            }
            order_by_list->emplace_back(order_by_expr);
            order_by_expr = nullptr;
        }
    }

    // auto end2 = std::chrono::steady_clock::now();
    // phase_2_duration_ += end2 - start2;
    //
    // auto start3 = std::chrono::steady_clock::now();

    const QueryResult result = infinity->Search(request.db_name,
                                                request.table_name,
                                                search_expr,
                                                filter,
                                                limit,
                                                offset,
                                                output_columns,
                                                highlight_columns,
                                                order_by_list,
                                                nullptr);
    output_columns = nullptr;
    highlight_columns = nullptr;
    filter = nullptr;
    search_expr = nullptr;
    limit = nullptr;
    offset = nullptr;
    order_by_list = nullptr;
    // auto end3 = std::chrono::steady_clock::now();
    //
    // phase_3_duration_ += end3 - start3;
    //
    // auto start4 = std::chrono::steady_clock::now();

    if (result.IsOk()) {
        auto &columns = response.column_fields;
        columns.resize(result.result_table_->ColumnCount());
        ProcessDataBlocks(result, response, columns);
    } else {
        ProcessQueryResult(response, result);
    }

    // auto end4 = std::chrono::steady_clock::now();
    // phase_4_duration_ += end4 - start4;
    //
    // if (count_ % 10000 == 0) {
    //     LOG_ERROR(fmt::format("Phase 1: {} Phase 2: {} Phase 3: {} Phase 4: {}  Total: {} seconds",
    //                      phase_1_duration_.count(),
    //                      phase_2_duration_.count(),
    //                      phase_3_duration_.count(),
    //                      phase_4_duration_.count(),
    //                      (phase_1_duration_ + phase_2_duration_ + phase_3_duration_ + phase_4_duration_).count()));
    //     phase_1_duration_ = std::chrono::duration<double>();
    //     phase_2_duration_ = std::chrono::duration<double>();
    //     phase_3_duration_ = std::chrono::duration<double>();
    //     phase_4_duration_ = std::chrono::duration<double>();
    // } else if (count_ % 1000 == 0) {
    //     LOG_ERROR(fmt::format("Phase 1: {} Phase 2: {} Phase 3: {} Phase 4: {}  Total: {} seconds",
    //                      phase_1_duration_.count(),
    //                      phase_2_duration_.count(),
    //                      phase_3_duration_.count(),
    //                      phase_4_duration_.count(),
    //                      (phase_1_duration_ + phase_2_duration_ + phase_3_duration_ + phase_4_duration_).count()));
    // }
}

void InfinityThriftService::Explain(infinity_thrift_rpc::SelectResponse &response, const infinity_thrift_rpc::ExplainRequest &request) {
    auto [infinity, infinity_status] = GetInfinityBySessionID(request.session_id);
    if (!infinity_status.ok()) {
        ProcessStatus(response, infinity_status);
        return;
    }

    if (request.__isset.select_list == false) {
        ProcessStatus(response, Status::EmptySelectFields());
        return;
    }

    Vector<ParsedExpr *> *output_columns = new Vector<ParsedExpr *>();
    output_columns->reserve(request.select_list.size());
    DeferFn defer_fn1([&]() {
        if (output_columns != nullptr) {
            for (auto &expr_ptr : *output_columns) {
                delete expr_ptr;
                expr_ptr = nullptr;
            }
            delete output_columns;
            output_columns = nullptr;
        }
    });

    Status parsed_expr_status;
    for (auto &expr : request.select_list) {
        auto parsed_expr = GetParsedExprFromProto(parsed_expr_status, expr);
        if (!parsed_expr_status.ok()) {
            if (parsed_expr != nullptr) {
                delete parsed_expr;
                parsed_expr = nullptr;
            }

            ProcessStatus(response, parsed_expr_status);
            return;
        }
        output_columns->emplace_back(parsed_expr);
    }

    // highlight list
    Vector<ParsedExpr *> *highlight_columns = nullptr;
    DeferFn defer_fn11([&]() {
        if (highlight_columns != nullptr) {
            for (auto &expr_ptr : *highlight_columns) {
                delete expr_ptr;
                expr_ptr = nullptr;
            }
            delete highlight_columns;
            highlight_columns = nullptr;
        }
    });
    if (!request.highlight_list.empty()) {
        highlight_columns = new Vector<ParsedExpr *>();
        highlight_columns->reserve(request.highlight_list.size());

        for (auto &expr : request.highlight_list) {
            auto parsed_expr = GetParsedExprFromProto(parsed_expr_status, expr);
            DeferFn defer_fn4([&]() {
                if (parsed_expr != nullptr) {
                    delete parsed_expr;
                    parsed_expr = nullptr;
                }
            });

            if (!parsed_expr_status.ok()) {
                ProcessStatus(response, parsed_expr_status);
                return;
            }
            highlight_columns->emplace_back(parsed_expr);
            parsed_expr = nullptr;
        }
    }

    // search expr
    SearchExpr *search_expr = nullptr;
    DeferFn defer_fn5([&]() {
        if (search_expr != nullptr) {
            delete search_expr;
            search_expr = nullptr;
        }
    });
    if (request.__isset.search_expr) {
        auto search_expr_list = new Vector<ParsedExpr *>();
        DeferFn defer_fn2([&]() {
            if (search_expr_list != nullptr) {
                for (auto &expr_ptr : *search_expr_list) {
                    delete expr_ptr;
                    expr_ptr = nullptr;
                }
                delete search_expr_list;
                search_expr_list = nullptr;
            }
        });

        SizeT match_expr_count = request.search_expr.match_exprs.size();
        SizeT fusion_expr_count = request.search_expr.fusion_exprs.size();
        SizeT total_expr_count = match_expr_count + fusion_expr_count;
        search_expr_list->reserve(total_expr_count);
        for (SizeT idx = 0; idx < match_expr_count; ++idx) {
            Status status;
            ParsedExpr *match_expr = GetGenericMatchExprFromProto(status, request.search_expr.match_exprs[idx]);
            DeferFn defer_fn3([&]() {
                if (match_expr != nullptr) {
                    delete match_expr;
                    match_expr = nullptr;
                }
            });
            if (!status.ok()) {
                ProcessStatus(response, status);
                return;
            }
            search_expr_list->emplace_back(match_expr);
            match_expr = nullptr;
        }

        for (SizeT idx = 0; idx < fusion_expr_count; ++idx) {
            ParsedExpr *fusion_expr = GetFusionExprFromProto(request.search_expr.fusion_exprs[idx]);
            search_expr_list->emplace_back(fusion_expr);
        }

        search_expr = new SearchExpr();
        search_expr->SetExprs(search_expr_list);
        search_expr_list = nullptr;
    }

    // filter
    ParsedExpr *filter = nullptr;
    if (request.__isset.where_expr == true) {
        filter = GetParsedExprFromProto(parsed_expr_status, request.where_expr);
        if (!parsed_expr_status.ok()) {
            if (search_expr != nullptr) {
                delete search_expr;
                search_expr = nullptr;
            }

            if (filter != nullptr) {
                delete filter;
                filter = nullptr;
            }

            ProcessStatus(response, parsed_expr_status);
            return;
        }
    }

    // Limit
    ParsedExpr *limit = nullptr;
    DeferFn defer_fn7([&]() {
        if (limit != nullptr) {
            delete limit;
            limit = nullptr;
        }
    });
    if (request.__isset.limit_expr == true) {
        limit = GetParsedExprFromProto(parsed_expr_status, request.limit_expr);
        if (!parsed_expr_status.ok()) {
            ProcessStatus(response, parsed_expr_status);
            return;
        }
    }

    // Offset
    ParsedExpr *offset = nullptr;
    DeferFn defer_fn8([&]() {
        if (offset != nullptr) {
            delete offset;
            offset = nullptr;
        }
    });
    if (request.__isset.offset_expr == true) {
        offset = GetParsedExprFromProto(parsed_expr_status, request.offset_expr);
        if (!parsed_expr_status.ok()) {
            ProcessStatus(response, parsed_expr_status);
            return;
        }
    }

    // order by
    Vector<OrderByExpr *> *order_by_list = nullptr;
    DeferFn defer_fn9([&]() {
        if (order_by_list != nullptr) {
            for (auto &expr_ptr : *order_by_list) {
                delete expr_ptr;
                expr_ptr = nullptr;
            }
            delete order_by_list;
            order_by_list = nullptr;
        }
    });
    if (!request.order_by_list.empty()) {
        order_by_list = new Vector<OrderByExpr *>();
        order_by_list->reserve(request.order_by_list.size());

        Status order_by_status;
        for (auto &expr : request.order_by_list) {
            auto order_by_expr = GetOrderByExprFromProto(order_by_status, expr);
            DeferFn defer_fn4([&]() {
                if (order_by_expr != nullptr) {
                    delete order_by_expr;
                    order_by_expr = nullptr;
                }
            });

            if (!order_by_status.ok()) {
                ProcessStatus(response, order_by_status);
                return;
            }
            order_by_list->emplace_back(order_by_expr);
            order_by_expr = nullptr;
        }
    }

    // Explain type
    auto explain_type = GetExplainTypeFromProto(request.explain_type);
    const QueryResult result = infinity->Explain(request.db_name,
                                                 request.table_name,
                                                 explain_type,
                                                 search_expr,
                                                 filter,
                                                 limit,
                                                 offset,
                                                 output_columns,
                                                 highlight_columns,
                                                 order_by_list,
                                                 nullptr);
    output_columns = nullptr;
    highlight_columns = nullptr;
    search_expr = nullptr;
    filter = nullptr;
    limit = nullptr;
    offset = nullptr;
    order_by_list = nullptr;

    if (result.IsOk()) {
        auto &columns = response.column_fields;
        columns.resize(result.result_table_->ColumnCount());
        ProcessDataBlocks(result, response, columns);
    } else {
        ProcessQueryResult(response, result);
    }
}

void InfinityThriftService::Delete(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::DeleteRequest &request) {
    auto [infinity, infinity_status] = GetInfinityBySessionID(request.session_id);
    if (!infinity_status.ok()) {
        ProcessStatus(response, infinity_status);
        return;
    }

    ParsedExpr *filter = nullptr;
    if (request.__isset.where_expr == true) {
        Status parsed_expr_status;
        filter = GetParsedExprFromProto(parsed_expr_status, request.where_expr);
        if (!parsed_expr_status.ok()) {
            ProcessStatus(response, parsed_expr_status);
            return;
        }
    }

    const QueryResult result = infinity->Delete(request.db_name, request.table_name, filter);
    ProcessQueryResult(response, result);
};

void InfinityThriftService::Update(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::UpdateRequest &request) {
    auto [infinity, infinity_status] = GetInfinityBySessionID(request.session_id);
    if (!infinity_status.ok()) {
        ProcessStatus(response, infinity_status);
        return;
    }

    ParsedExpr *filter = nullptr;
    if (request.__isset.where_expr == true) {
        Status parsed_expr_status;
        filter = GetParsedExprFromProto(parsed_expr_status, request.where_expr);
        if (!parsed_expr_status.ok()) {
            ProcessStatus(response, parsed_expr_status);
            return;
        }
    }

    std::vector<UpdateExpr *> *update_expr_array{nullptr};
    if (request.__isset.update_expr_array == true) {
        update_expr_array = new std::vector<UpdateExpr *>();
        update_expr_array->reserve(request.update_expr_array.size());
        for (auto &update_expr : request.update_expr_array) {
            auto [parsed_expr, update_expr_status] = GetUpdateExprFromProto(update_expr);
            if (!update_expr_status.ok()) {

                if (update_expr_array != nullptr) {
                    for (auto update_expr : *update_expr_array) {
                        delete update_expr;
                    }

                    delete update_expr_array;
                    update_expr_array = nullptr;
                }

                if (parsed_expr != nullptr) {
                    delete parsed_expr;
                    parsed_expr = nullptr;
                }

                ProcessStatus(response, update_expr_status);
                return;
            }
            update_expr_array->emplace_back(parsed_expr);
        }
    }

    const QueryResult result = infinity->Update(request.db_name, request.table_name, filter, update_expr_array);
    ProcessQueryResult(response, result);
}

void InfinityThriftService::Optimize(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::OptimizeRequest &request) {
    auto [infinity, infinity_status] = GetInfinityBySessionID(request.session_id);
    if (!infinity_status.ok()) {
        ProcessStatus(response, infinity_status);
        return;
    }

    auto optimize_options = GetParsedOptimizeOptionFromProto(request.optimize_options);

    const QueryResult result = infinity->Optimize(request.db_name, request.table_name, std::move(optimize_options));
    ProcessQueryResult(response, result);
}

void InfinityThriftService::AddColumns(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::AddColumnsRequest &request) {
    Vector<SharedPtr<ColumnDef>> column_defs;
    for (auto &proto_column_def : request.column_defs) {
        auto [column_def, column_def_status] = GetColumnDefFromProto(proto_column_def);
        if (!column_def_status.ok()) {
            ProcessStatus(response, column_def_status);
            return;
        }
        column_defs.emplace_back(column_def);
    }
    auto [infinity, infinity_status] = GetInfinityBySessionID(request.session_id);
    if (!infinity_status.ok()) {
        ProcessStatus(response, infinity_status);
        return;
    }
    auto result = infinity->AddColumns(request.db_name, request.table_name, std::move(column_defs));
    ProcessQueryResult(response, result);
}

void InfinityThriftService::DropColumns(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::DropColumnsRequest &request) {
    Vector<String> column_names = request.column_names;
    auto [infinity, infinity_status] = GetInfinityBySessionID(request.session_id);
    if (!infinity_status.ok()) {
        ProcessStatus(response, infinity_status);
        return;
    }
    auto result = infinity->DropColumns(request.db_name, request.table_name, std::move(column_names));
    ProcessQueryResult(response, result);
}

void InfinityThriftService::Cleanup(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::CommonRequest &request) {
    auto [infinity, infinity_status] = GetInfinityBySessionID(request.session_id);
    if (!infinity_status.ok()) {
        ProcessStatus(response, infinity_status);
        return;
    }
    auto result = infinity->Cleanup();
    ProcessQueryResult(response, result);
}

void InfinityThriftService::ListDatabase(infinity_thrift_rpc::ListDatabaseResponse &response,
                                         const infinity_thrift_rpc::ListDatabaseRequest &request) {
    auto [infinity, infinity_status] = GetInfinityBySessionID(request.session_id);
    if (!infinity_status.ok()) {
        ProcessStatus(response, infinity_status);
        return;
    }

    auto result = infinity->ListDatabases();
    response.__set_error_code((i64)(result.ErrorCode()));
    if (result.IsOk()) {
        SharedPtr<DataBlock> data_block = result.result_table_->GetDataBlockById(0);
        auto row_count = data_block->row_count();
        for (int i = 0; i < row_count; ++i) {
            Value value = data_block->GetValue(0, i);
            const String &db_name = value.GetVarchar();
            response.db_names.emplace_back(db_name);
        }
    } else {
        ProcessQueryResult(response, result);
    }
}

void InfinityThriftService::ListTable(infinity_thrift_rpc::ListTableResponse &response, const infinity_thrift_rpc::ListTableRequest &request) {
    auto [infinity, infinity_status] = GetInfinityBySessionID(request.session_id);
    if (!infinity_status.ok()) {
        ProcessStatus(response, infinity_status);
        return;
    }

    auto result = infinity->ListTables(request.db_name);
    if (result.IsOk()) {
        SharedPtr<DataBlock> data_block = result.result_table_->GetDataBlockById(0);
        auto row_count = data_block->row_count();
        for (int i = 0; i < row_count; ++i) {
            Value value = data_block->GetValue(1, i);
            const String &table_name = value.GetVarchar();
            response.table_names.emplace_back(table_name);
        }
        response.__set_error_code((i64)(result.ErrorCode()));
    } else {
        ProcessQueryResult(response, result);
    }
}

void InfinityThriftService::ShowDatabase(infinity_thrift_rpc::ShowDatabaseResponse &response,
                                         const infinity_thrift_rpc::ShowDatabaseRequest &request) {
    auto [infinity, infinity_status] = GetInfinityBySessionID(request.session_id);
    if (!infinity_status.ok()) {
        ProcessStatus(response, infinity_status);
        return;
    }
    const QueryResult result = infinity->ShowDatabase(request.db_name);
    if (result.IsOk()) {
        SharedPtr<DataBlock> data_block = result.result_table_->GetDataBlockById(0);
        auto row_count = data_block->row_count();
        if (row_count != 3) {
            String error_message = "ShowDatabase: query result is invalid.";
            UnrecoverableError(error_message);
        }

        {
            Value value = data_block->GetValue(1, 0);
            response.database_name = value.GetVarchar();
        }

        {
            Value value = data_block->GetValue(1, 1);
            response.store_dir = value.GetVarchar();
        }

        {
            Value value = data_block->GetValue(1, 2);
            response.table_count = std::stol(value.GetVarchar());
        }

        response.__set_error_code((i64)(result.ErrorCode()));
    } else {
        ProcessQueryResult(response, result);
    }
}

void InfinityThriftService::ShowTable(infinity_thrift_rpc::ShowTableResponse &response, const infinity_thrift_rpc::ShowTableRequest &request) {
    auto [infinity, infinity_status] = GetInfinityBySessionID(request.session_id);
    if (!infinity_status.ok()) {
        ProcessStatus(response, infinity_status);
        return;
    }

    const QueryResult result = infinity->ShowTable(request.db_name, request.table_name);
    if (result.IsOk()) {
        SharedPtr<DataBlock> data_block = result.result_table_->GetDataBlockById(0);
        auto row_count = data_block->row_count();
        if (row_count != 6) {
            String error_message = "ShowTable: query result is invalid.";
            UnrecoverableError(error_message);
        }

        {
            Value value = data_block->GetValue(1, 0);
            response.database_name = value.GetVarchar();
        }

        {
            Value value = data_block->GetValue(1, 1);
            response.table_name = value.GetVarchar();
        }

        {
            Value value = data_block->GetValue(1, 2);
            response.store_dir = value.GetVarchar();
        }

        {
            Value value = data_block->GetValue(1, 3);
            response.column_count = std::stol(value.GetVarchar());
        }

        {
            Value value = data_block->GetValue(1, 4);
            response.segment_count = std::stol(value.GetVarchar());
        }

        {
            Value value = data_block->GetValue(1, 5);
            response.row_count = std::stol(value.GetVarchar());
        }

        response.__set_error_code((i64)(result.ErrorCode()));
    } else {
        ProcessQueryResult(response, result);
    }
}

void InfinityThriftService::ShowColumns(infinity_thrift_rpc::SelectResponse &response, const infinity_thrift_rpc::ShowColumnsRequest &request) {
    auto [infinity, infinity_status] = GetInfinityBySessionID(request.session_id);
    if (!infinity_status.ok()) {
        ProcessStatus(response, infinity_status);
        return;
    }

    const QueryResult result = infinity->ShowColumns(request.db_name, request.table_name);
    if (result.IsOk()) {
        auto &columns = response.column_fields;
        columns.resize(result.result_table_->ColumnCount());
        ProcessDataBlocks(result, response, columns);
    } else {
        ProcessQueryResult(response, result);
    }
}

void InfinityThriftService::ShowTables(infinity_thrift_rpc::SelectResponse &response, const infinity_thrift_rpc::ShowTablesRequest &request) {
    auto [infinity, infinity_status] = GetInfinityBySessionID(request.session_id);
    if (!infinity_status.ok()) {
        ProcessStatus(response, infinity_status);
        return;
    }

    const QueryResult result = infinity->ShowTables(request.db_name);
    if (result.IsOk()) {
        auto &columns = response.column_fields;
        columns.resize(result.result_table_->ColumnCount());
        ProcessDataBlocks(result, response, columns);
    } else {
        ProcessQueryResult(response, result);
    }
}

void InfinityThriftService::GetDatabase(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::GetDatabaseRequest &request) {
    auto [infinity, infinity_status] = GetInfinityBySessionID(request.session_id);
    if (!infinity_status.ok()) {
        ProcessStatus(response, infinity_status);
        return;
    }

    QueryResult result = infinity->GetDatabase(request.db_name);
    ProcessQueryResult(response, result);
}

void InfinityThriftService::GetTable(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::GetTableRequest &request) {
    auto [infinity, infinity_status] = GetInfinityBySessionID(request.session_id);
    if (!infinity_status.ok()) {
        ProcessStatus(response, infinity_status);
        return;
    }

    QueryResult result = infinity->GetTable(request.db_name, request.table_name);
    ProcessQueryResult(response, result);
}

void InfinityThriftService::CreateIndex(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::CreateIndexRequest &request) {
    CreateIndexOptions create_index_opts;
    switch (request.create_option.conflict_type) {
        case infinity_thrift_rpc::CreateConflict::Ignore: {
            create_index_opts.conflict_type_ = ConflictType::kIgnore;
            break;
        }
        case infinity_thrift_rpc::CreateConflict::Error: {
            create_index_opts.conflict_type_ = ConflictType::kError;
            break;
        }
        case infinity_thrift_rpc::CreateConflict::Replace: {
            create_index_opts.conflict_type_ = ConflictType::kReplace;
            break;
        }
        default: {
            ProcessStatus(response, Status::InvalidConflictType());
            return;
        }
    }

    auto [infinity, infinity_status] = GetInfinityBySessionID(request.session_id);
    if (!infinity_status.ok()) {
        ProcessStatus(response, infinity_status);
        return;
    }

    auto *index_info_to_use = new IndexInfo();

    index_info_to_use->index_type_ = GetIndexTypeFromProto(request.index_info.index_type);
    if (index_info_to_use->index_type_ == IndexType::kInvalid) {

        delete index_info_to_use;
        index_info_to_use = nullptr;

        ProcessStatus(response, Status::InvalidIndexType("Invalid index"));
        return;
    }

    index_info_to_use->column_name_ = request.index_info.column_name;

    auto *index_param_list = new Vector<InitParameter *>();
    for (auto &index_param : request.index_info.index_param_list) {
        auto init_parameter = new InitParameter();
        init_parameter->param_name_ = index_param.param_name;
        init_parameter->param_value_ = index_param.param_value;
        index_param_list->emplace_back(init_parameter);
    }
    index_info_to_use->index_param_list_ = index_param_list;

    QueryResult result = infinity->CreateIndex(request.db_name, request.table_name, request.index_name, index_info_to_use, create_index_opts);
    ProcessQueryResult(response, result);
}

void InfinityThriftService::DropIndex(infinity_thrift_rpc::CommonResponse &response, const infinity_thrift_rpc::DropIndexRequest &request) {
    DropIndexOptions drop_index_opts;
    switch (request.drop_option.conflict_type) {
        case infinity_thrift_rpc::DropConflict::type::Ignore: {
            drop_index_opts.conflict_type_ = ConflictType::kIgnore;
            break;
        }
        case infinity_thrift_rpc::DropConflict::type::Error: {
            drop_index_opts.conflict_type_ = ConflictType::kError;
            break;
        }
        default: {
            ProcessStatus(response, Status::InvalidConflictType());
            return;
        }
    }

    auto [infinity, infinity_status] = GetInfinityBySessionID(request.session_id);
    if (!infinity_status.ok()) {
        ProcessStatus(response, infinity_status);
        return;
    }

    QueryResult result = infinity->DropIndex(request.db_name, request.table_name, request.index_name, drop_index_opts);
    ProcessQueryResult(response, result);
}

void InfinityThriftService::ListIndex(infinity_thrift_rpc::ListIndexResponse &response, const infinity_thrift_rpc::ListIndexRequest &request) {
    auto [infinity, infinity_status] = GetInfinityBySessionID(request.session_id);
    if (!infinity_status.ok()) {
        ProcessStatus(response, infinity_status);
        return;
    }

    auto result = infinity->ListTableIndexes(request.db_name, request.table_name);
    if (result.IsOk()) {
        SharedPtr<DataBlock> data_block = result.result_table_->GetDataBlockById(0);
        auto row_count = data_block->row_count();
        for (int i = 0; i < row_count; ++i) {
            Value value = data_block->GetValue(0, i);
            const String &index_name = value.GetVarchar();
            response.index_names.emplace_back(index_name);
        }
        response.__set_error_code((i64)(result.ErrorCode()));
    } else {
        ProcessQueryResult(response, result);
    }
}

void InfinityThriftService::ShowIndex(infinity_thrift_rpc::ShowIndexResponse &response, const infinity_thrift_rpc::ShowIndexRequest &request) {
    auto [infinity, infinity_status] = GetInfinityBySessionID(request.session_id);
    if (!infinity_status.ok()) {
        ProcessStatus(response, infinity_status);
        return;
    }

    auto result = infinity->ShowIndex(request.db_name, request.table_name, request.index_name);

    if (result.IsOk()) {
        SharedPtr<DataBlock> data_block = result.result_table_->GetDataBlockById(0);
        auto row_count = data_block->row_count();
        if (row_count != 10) {
            String error_message = "ShowIndex: query result is invalid.";
            UnrecoverableError(error_message);
        }

        {
            Value value = data_block->GetValue(1, 0);
            response.db_name = value.GetVarchar();
        }

        {
            Value value = data_block->GetValue(1, 1);
            response.table_name = value.GetVarchar();
        }

        {
            Value value = data_block->GetValue(1, 2);
            response.index_name = value.GetVarchar();
        }

        {
            Value value = data_block->GetValue(1, 3);
            response.index_type = value.GetVarchar();
        }

        {
            Value value = data_block->GetValue(1, 4);
            response.index_column_names = value.GetVarchar();
        }

        {
            Value value = data_block->GetValue(1, 5);
            response.index_column_ids = value.GetVarchar();
        }

        {
            Value value = data_block->GetValue(1, 6);
            response.other_parameters = value.GetVarchar();
        }

        {
            Value value = data_block->GetValue(1, 7);
            response.store_dir = value.GetVarchar();
        }

        {
            Value value = data_block->GetValue(1, 8);
            response.store_size = value.GetVarchar();
        }

        {
            Value value = data_block->GetValue(1, 9);
            response.segment_index_count = value.GetVarchar();
        }

        response.__set_error_code((i64)(result.ErrorCode()));
    } else {
        ProcessQueryResult(response, result);
    }
}

void InfinityThriftService::ShowSegments(infinity_thrift_rpc::SelectResponse &response, const infinity_thrift_rpc::ShowSegmentsRequest &request) {
    auto [infinity, infinity_status] = GetInfinityBySessionID(request.session_id);
    if (!infinity_status.ok()) {
        ProcessStatus(response, infinity_status);
        return;
    }

    const QueryResult result = infinity->ShowSegments(request.db_name, request.table_name);
    if (result.IsOk()) {
        auto &columns = response.column_fields;
        columns.resize(result.result_table_->ColumnCount());
        ProcessDataBlocks(result, response, columns);
    } else {
        ProcessQueryResult(response, result);
    }
}

void InfinityThriftService::ShowSegment(infinity_thrift_rpc::ShowSegmentResponse &response, const infinity_thrift_rpc::ShowSegmentRequest &request) {
    auto [infinity, infinity_status] = GetInfinityBySessionID(request.session_id);
    if (!infinity_status.ok()) {
        ProcessStatus(response, infinity_status);
        return;
    }

    const QueryResult result = infinity->ShowSegment(request.db_name, request.table_name, request.segment_id);
    if (result.IsOk()) {
        SharedPtr<DataBlock> data_block = result.result_table_->GetDataBlockById(0);
        auto row_count = data_block->row_count();
        if (row_count != 1) {
            String error_message = "ShowSegment: query result is invalid.";
            UnrecoverableError(error_message);
        }

        {
            Value value = data_block->GetValue(0, 0);
            response.segment_id = value.value_.big_int;
        }

        {
            Value value = data_block->GetValue(0, 1);
            response.status = value.GetVarchar();
        }

        {
            Value value = data_block->GetValue(0, 2);
            response.path = value.GetVarchar();
        }

        {
            Value value = data_block->GetValue(0, 3);
            response.size = value.GetVarchar();
        }

        {
            Value value = data_block->GetValue(0, 4);
            response.block_count = value.value_.big_int;
        }

        {
            Value value = data_block->GetValue(0, 5);
            response.row_capacity = value.value_.big_int;
        }

        {
            Value value = data_block->GetValue(0, 6);
            response.row_count = value.value_.big_int;
        }

        {
            Value value = data_block->GetValue(0, 8);
            response.room = value.value_.big_int;
        }

        {
            Value value = data_block->GetValue(0, 9);
            response.column_count = value.value_.big_int;
        }

        response.__set_error_code((i64)(result.ErrorCode()));
    } else {
        ProcessQueryResult(response, result);
    }
}

void InfinityThriftService::ShowBlocks(infinity_thrift_rpc::SelectResponse &response, const infinity_thrift_rpc::ShowBlocksRequest &request) {
    auto [infinity, infinity_status] = GetInfinityBySessionID(request.session_id);
    if (!infinity_status.ok()) {
        ProcessStatus(response, infinity_status);
        return;
    }

    const QueryResult result = infinity->ShowBlocks(request.db_name, request.table_name, request.segment_id);
    if (result.IsOk()) {
        auto &columns = response.column_fields;
        columns.resize(result.result_table_->ColumnCount());
        ProcessDataBlocks(result, response, columns);
    } else {
        ProcessQueryResult(response, result);
    }
}

void InfinityThriftService::ShowBlock(infinity_thrift_rpc::ShowBlockResponse &response, const infinity_thrift_rpc::ShowBlockRequest &request) {
    auto [infinity, infinity_status] = GetInfinityBySessionID(request.session_id);
    if (!infinity_status.ok()) {
        ProcessStatus(response, infinity_status);
        return;
    }

    const QueryResult result = infinity->ShowBlock(request.db_name, request.table_name, request.segment_id, request.block_id);
    if (result.IsOk()) {
        SharedPtr<DataBlock> data_block = result.result_table_->GetDataBlockById(0);
        auto row_count = data_block->row_count();
        if (row_count != 1) {
            String error_message = "ShowBlock: query result is invalid.";
            UnrecoverableError(error_message);
        }

        {
            Value value = data_block->GetValue(0, 0);
            response.block_id = value.value_.big_int;
        }

        {
            Value value = data_block->GetValue(0, 1);
            response.path = value.GetVarchar();
        }

        {
            Value value = data_block->GetValue(0, 2);
            response.size = value.GetVarchar();
        }

        {
            Value value = data_block->GetValue(0, 3);
            response.size = value.GetVarchar();
        }

        {
            Value value = data_block->GetValue(0, 4);
            response.row_capacity = value.value_.big_int;
        }

        {
            Value value = data_block->GetValue(0, 5);
            response.row_count = value.value_.big_int;
        }

        {
            Value value = data_block->GetValue(0, 6);
            response.column_count = value.value_.big_int;
        }

        response.__set_error_code((i64)(result.ErrorCode()));
    } else {
        ProcessQueryResult(response, result);
    }
}

void InfinityThriftService::ShowBlockColumn(infinity_thrift_rpc::ShowBlockColumnResponse &response,
                                            const infinity_thrift_rpc::ShowBlockColumnRequest &request) {
    auto [infinity, infinity_status] = GetInfinityBySessionID(request.session_id);
    if (!infinity_status.ok()) {
        ProcessStatus(response, infinity_status);
        return;
    }

    auto result = infinity->ShowBlockColumn(request.db_name, request.table_name, request.segment_id, request.block_id, request.column_id);

    if (result.IsOk()) {
        SharedPtr<DataBlock> data_block = result.result_table_->GetDataBlockById(0);
        auto row_count = data_block->row_count();
        if (row_count != 6) {
            String error_message = "ShowBlockColumn: query result is invalid.";
            UnrecoverableError(error_message);
        }

        {
            Value value = data_block->GetValue(1, 0);
            response.column_name = value.GetVarchar();
        }

        {
            Value value = data_block->GetValue(1, 1);
            response.column_id = value.value_.big_int;
        }

        {
            Value value = data_block->GetValue(1, 2);
            response.data_type = value.GetVarchar();
        }

        {
            Value value = data_block->GetValue(1, 3);
            response.path = value.GetVarchar();
        }

        {
            Value value = data_block->GetValue(1, 4);
            response.extra_file_count = value.value_.big_int;
        }

        {
            Value value = data_block->GetValue(1, 5);
            response.extra_file_names = value.GetVarchar();
        }

        response.__set_error_code((i64)(result.ErrorCode()));
    } else {
        ProcessQueryResult(response, result);
    }
}

Tuple<Infinity *, Status> InfinityThriftService::GetInfinityBySessionID(i64 session_id) {
    std::lock_guard<std::mutex> lock(infinity_session_map_mutex_);
    auto iter = infinity_session_map_.find(session_id);
    if (iter == infinity_session_map_.end()) {
        return {nullptr, Status::SessionNotFound(session_id)};
    }
    return {iter->second.get(), Status::OK()};
}

Status InfinityThriftService::GetAndRemoveSessionID(i64 session_id) {
    std::lock_guard<std::mutex> lock(infinity_session_map_mutex_);
    auto iter = infinity_session_map_.find(session_id);
    if (iter == infinity_session_map_.end()) {
        return Status::SessionNotFound(session_id);
    }
    iter->second->RemoteDisconnect();
    infinity_session_map_.erase(session_id);
    LOG_TRACE(fmt::format("THRIFT: Remove session {}", session_id));
    return Status::OK();
}

Tuple<ColumnDef *, Status> InfinityThriftService::GetColumnDefFromProto(const infinity_thrift_rpc::ColumnDef &column_def) {
    auto column_def_data_type_ptr = GetColumnTypeFromProto(column_def.data_type);
    if (column_def_data_type_ptr->type() == infinity::LogicalType::kInvalid) {
        return {nullptr, Status::InvalidDataType()};
    }

    auto constraints = std::set<ConstraintType>();

    for (auto constraint : column_def.constraints) {
        auto type = GetConstraintTypeFromProto(constraint);
        if (type == ConstraintType::kInvalid) {
            return {nullptr, Status::InvalidConstraintType()};
        }
        constraints.insert(type);
    }

    Status status;
    auto const_expr = SharedPtr<ParsedExpr>(GetConstantFromProto(status, column_def.constant_expr));
    if (!status.ok()) {
        return {nullptr, status};
    }

    String comment = column_def.comment;
    auto col_def = new ColumnDef(column_def.id, column_def_data_type_ptr, column_def.name, constraints, std::move(comment), const_expr);
    return {col_def, Status::OK()};
}

SharedPtr<DataType> InfinityThriftService::GetColumnTypeFromProto(const infinity_thrift_rpc::DataType &type) {
    switch (type.logic_type) {
        case infinity_thrift_rpc::LogicType::Boolean:
            return MakeShared<infinity::DataType>(infinity::LogicalType::kBoolean);
        case infinity_thrift_rpc::LogicType::TinyInt:
            return MakeShared<infinity::DataType>(infinity::LogicalType::kTinyInt);
        case infinity_thrift_rpc::LogicType::SmallInt:
            return MakeShared<infinity::DataType>(infinity::LogicalType::kSmallInt);
        case infinity_thrift_rpc::LogicType::Integer:
            return MakeShared<infinity::DataType>(infinity::LogicalType::kInteger);
        case infinity_thrift_rpc::LogicType::BigInt:
            return MakeShared<infinity::DataType>(infinity::LogicalType::kBigInt);
        case infinity_thrift_rpc::LogicType::HugeInt:
            return MakeShared<infinity::DataType>(infinity::LogicalType::kHugeInt);
        case infinity_thrift_rpc::LogicType::Decimal:
            return MakeShared<infinity::DataType>(infinity::LogicalType::kDecimal);
        case infinity_thrift_rpc::LogicType::Float:
            return MakeShared<infinity::DataType>(infinity::LogicalType::kFloat);
        case infinity_thrift_rpc::LogicType::Double:
            return MakeShared<infinity::DataType>(infinity::LogicalType::kDouble);
        case infinity_thrift_rpc::LogicType::Float16:
            return MakeShared<infinity::DataType>(infinity::LogicalType::kFloat16);
        case infinity_thrift_rpc::LogicType::BFloat16:
            return MakeShared<infinity::DataType>(infinity::LogicalType::kBFloat16);
        case infinity_thrift_rpc::LogicType::Tensor:
        case infinity_thrift_rpc::LogicType::TensorArray:
        case infinity_thrift_rpc::LogicType::MultiVector:
        case infinity_thrift_rpc::LogicType::Embedding: {
            const auto embedding_type = GetEmbeddingDataTypeFromProto(type.physical_type.embedding_type.element_type);
            if (embedding_type == EmbeddingDataType::kElemInvalid) {
                return MakeShared<infinity::DataType>(infinity::LogicalType::kInvalid);
            }
            auto embedding_info = EmbeddingInfo::Make(embedding_type, type.physical_type.embedding_type.dimension);
            infinity::LogicalType dt = infinity::LogicalType::kInvalid;
            switch (type.logic_type) {
                case infinity_thrift_rpc::LogicType::Tensor: {
                    dt = infinity::LogicalType::kTensor;
                    break;
                }
                case infinity_thrift_rpc::LogicType::TensorArray: {
                    dt = infinity::LogicalType::kTensorArray;
                    break;
                }
                case infinity_thrift_rpc::LogicType::MultiVector: {
                    dt = infinity::LogicalType::kMultiVector;
                    break;
                }
                case infinity_thrift_rpc::LogicType::Embedding: {
                    dt = infinity::LogicalType::kEmbedding;
                    break;
                }
                default: {
                    UnrecoverableError("Unreachable code!");
                }
            }
            return MakeShared<infinity::DataType>(dt, std::move(embedding_info));
        }
        case infinity_thrift_rpc::LogicType::Varchar:
            return MakeShared<infinity::DataType>(infinity::LogicalType::kVarchar);
        case infinity_thrift_rpc::LogicType::Sparse: {
            auto embedding_type = GetEmbeddingDataTypeFromProto(type.physical_type.sparse_type.element_type);
            if (embedding_type == EmbeddingDataType::kElemInvalid) {
                return MakeShared<infinity::DataType>(infinity::LogicalType::kInvalid);
            }
            auto index_type = GetEmbeddingDataTypeFromProto(type.physical_type.sparse_type.index_type);
            if (index_type == EmbeddingDataType::kElemInvalid) {
                return MakeShared<infinity::DataType>(infinity::LogicalType::kInvalid);
            }
            auto sparse_info = SparseInfo::Make(embedding_type, index_type, type.physical_type.sparse_type.dimension, SparseStoreType::kSort);
            return MakeShared<infinity::DataType>(infinity::LogicalType::kSparse, sparse_info);
        }
        case infinity_thrift_rpc::LogicType::Date: {
            return MakeShared<infinity::DataType>(infinity::LogicalType::kDate);
        }
        case infinity_thrift_rpc::LogicType::Time: {
            return MakeShared<infinity::DataType>(infinity::LogicalType::kTime);
        }
        case infinity_thrift_rpc::LogicType::DateTime: {
            return MakeShared<infinity::DataType>(infinity::LogicalType::kDateTime);
        }
        case infinity_thrift_rpc::LogicType::Timestamp: {
            return MakeShared<infinity::DataType>(infinity::LogicalType::kTimestamp);
        }
        case infinity_thrift_rpc::LogicType::Interval: {
            return MakeShared<infinity::DataType>(infinity::LogicalType::kInterval);
        }
        case infinity_thrift_rpc::LogicType::Invalid: {
            return MakeShared<infinity::DataType>(infinity::LogicalType::kInvalid);
        }
    }
    return MakeShared<infinity::DataType>(infinity::LogicalType::kInvalid);
}

ConstraintType InfinityThriftService::GetConstraintTypeFromProto(infinity_thrift_rpc::Constraint::type constraint) {
    switch (constraint) {
        case infinity_thrift_rpc::Constraint::NotNull:
            return ConstraintType::kNotNull;
        case infinity_thrift_rpc::Constraint::Null:
            return ConstraintType::kNull;
        case infinity_thrift_rpc::Constraint::PrimaryKey:
            return ConstraintType::kPrimaryKey;
        case infinity_thrift_rpc::Constraint::Unique:
            return ConstraintType::kUnique;
        default:
            return ConstraintType::kInvalid;
    }
}

EmbeddingDataType InfinityThriftService::GetEmbeddingDataTypeFromProto(const infinity_thrift_rpc::ElementType::type &type) {
    switch (type) {
        case infinity_thrift_rpc::ElementType::ElementBit:
            return EmbeddingDataType::kElemBit;
        case infinity_thrift_rpc::ElementType::ElementUInt8:
            return EmbeddingDataType::kElemUInt8;
        case infinity_thrift_rpc::ElementType::ElementInt8:
            return EmbeddingDataType::kElemInt8;
        case infinity_thrift_rpc::ElementType::ElementInt16:
            return EmbeddingDataType::kElemInt16;
        case infinity_thrift_rpc::ElementType::ElementInt32:
            return EmbeddingDataType::kElemInt32;
        case infinity_thrift_rpc::ElementType::ElementInt64:
            return EmbeddingDataType::kElemInt64;
        case infinity_thrift_rpc::ElementType::ElementFloat32:
            return EmbeddingDataType::kElemFloat;
        case infinity_thrift_rpc::ElementType::ElementFloat64:
            return EmbeddingDataType::kElemDouble;
        case infinity_thrift_rpc::ElementType::ElementFloat16:
            return EmbeddingDataType::kElemFloat16;
        case infinity_thrift_rpc::ElementType::ElementBFloat16:
            return EmbeddingDataType::kElemBFloat16;
    }
    return EmbeddingDataType::kElemInvalid;
}

IndexType InfinityThriftService::GetIndexTypeFromProto(const infinity_thrift_rpc::IndexType::type &type) {
    switch (type) {
        case infinity_thrift_rpc::IndexType::IVF:
            return IndexType::kIVF;
        case infinity_thrift_rpc::IndexType::Hnsw:
            return IndexType::kHnsw;
        case infinity_thrift_rpc::IndexType::FullText:
            return IndexType::kFullText;
        case infinity_thrift_rpc::IndexType::Secondary:
            return IndexType::kSecondary;
        case infinity_thrift_rpc::IndexType::EMVB:
            return IndexType::kEMVB;
        case infinity_thrift_rpc::IndexType::BMP:
            return IndexType::kBMP;
        case infinity_thrift_rpc::IndexType::DiskAnn:
            return IndexType::kDiskAnn;
        default:
            return IndexType::kInvalid;
    }
    return IndexType::kInvalid;
}

ConstantExpr *InfinityThriftService::GetConstantFromProto(Status &status, const infinity_thrift_rpc::ConstantExpr &expr) {
    switch (expr.literal_type) {
        case infinity_thrift_rpc::LiteralType::Boolean: {
            auto parsed_expr = new ConstantExpr(LiteralType::kBoolean);
            parsed_expr->bool_value_ = expr.bool_value;
            return parsed_expr;
        }
        case infinity_thrift_rpc::LiteralType::Double: {
            auto parsed_expr = new ConstantExpr(LiteralType::kDouble);
            parsed_expr->double_value_ = expr.f64_value;
            return parsed_expr;
        }
        case infinity_thrift_rpc::LiteralType::String: {
            auto parsed_expr = new ConstantExpr(LiteralType::kString);
            parsed_expr->str_value_ = strdup(expr.str_value.c_str());
            return parsed_expr;
        }
        case infinity_thrift_rpc::LiteralType::Int64: {
            auto parsed_expr = new ConstantExpr(LiteralType::kInteger);
            parsed_expr->integer_value_ = expr.i64_value;
            return parsed_expr;
        }
        case infinity_thrift_rpc::LiteralType::Null: {
            auto parsed_expr = new ConstantExpr(LiteralType::kNull);
            return parsed_expr;
        }
        case infinity_thrift_rpc::LiteralType::IntegerArray: {
            auto parsed_expr = new ConstantExpr(LiteralType::kIntegerArray);
            parsed_expr->long_array_ = expr.i64_array_value;
            return parsed_expr;
        }
        case infinity_thrift_rpc::LiteralType::DoubleArray: {
            auto parsed_expr = new ConstantExpr(LiteralType::kDoubleArray);
            parsed_expr->double_array_ = expr.f64_array_value;
            return parsed_expr;
        }
        case infinity_thrift_rpc::LiteralType::IntegerTensor: {
            auto parsed_expr = new ConstantExpr(LiteralType::kSubArrayArray);
            parsed_expr->sub_array_array_.reserve(expr.i64_tensor_value.size());
            for (auto &value_2 : expr.i64_tensor_value) {
                auto parsed_expr_2 = MakeShared<ConstantExpr>(LiteralType::kIntegerArray);
                parsed_expr_2->long_array_ = value_2;
                parsed_expr->sub_array_array_.emplace_back(std::move(parsed_expr_2));
            }
            return parsed_expr;
        }
        case infinity_thrift_rpc::LiteralType::DoubleTensor: {
            auto parsed_expr = new ConstantExpr(LiteralType::kSubArrayArray);
            parsed_expr->sub_array_array_.reserve(expr.f64_tensor_value.size());
            for (auto &value_2 : expr.f64_tensor_value) {
                auto parsed_expr_2 = MakeShared<ConstantExpr>(LiteralType::kDoubleArray);
                parsed_expr_2->double_array_ = value_2;
                parsed_expr->sub_array_array_.emplace_back(std::move(parsed_expr_2));
            }
            return parsed_expr;
        }
        case infinity_thrift_rpc::LiteralType::IntegerTensorArray: {
            auto parsed_expr = new ConstantExpr(LiteralType::kSubArrayArray);
            parsed_expr->sub_array_array_.reserve(expr.i64_tensor_array_value.size());
            for (auto &value_1 : expr.i64_tensor_array_value) {
                auto parsed_expr_1 = MakeShared<ConstantExpr>(LiteralType::kSubArrayArray);
                parsed_expr_1->sub_array_array_.reserve(value_1.size());
                for (auto &value_2 : value_1) {
                    auto parsed_expr_2 = MakeShared<ConstantExpr>(LiteralType::kIntegerArray);
                    parsed_expr_2->long_array_ = value_2;
                    parsed_expr_1->sub_array_array_.emplace_back(std::move(parsed_expr_2));
                }
                parsed_expr->sub_array_array_.emplace_back(std::move(parsed_expr_1));
            }
            return parsed_expr;
        }
        case infinity_thrift_rpc::LiteralType::DoubleTensorArray: {
            auto parsed_expr = new ConstantExpr(LiteralType::kSubArrayArray);
            parsed_expr->sub_array_array_.reserve(expr.f64_tensor_array_value.size());
            for (auto &value_1 : expr.f64_tensor_array_value) {
                auto parsed_expr_1 = MakeShared<ConstantExpr>(LiteralType::kSubArrayArray);
                parsed_expr_1->sub_array_array_.reserve(value_1.size());
                for (auto &value_2 : value_1) {
                    auto parsed_expr_2 = MakeShared<ConstantExpr>(LiteralType::kDoubleArray);
                    parsed_expr_2->double_array_ = value_2;
                    parsed_expr_1->sub_array_array_.emplace_back(std::move(parsed_expr_2));
                }
                parsed_expr->sub_array_array_.emplace_back(std::move(parsed_expr_1));
            }
            return parsed_expr;
        }
        case infinity_thrift_rpc::LiteralType::SparseIntegerArray: {
            auto parsed_expr = new ConstantExpr(LiteralType::kLongSparseArray);
            parsed_expr->long_sparse_array_.first.reserve(expr.i64_array_idx.size());
            parsed_expr->long_sparse_array_.second.reserve(expr.i64_array_value.size());
            for (auto &value : expr.i64_array_idx) {
                parsed_expr->long_sparse_array_.first.emplace_back(value);
            }
            for (auto &value : expr.i64_array_value) {
                parsed_expr->long_sparse_array_.second.emplace_back(value);
            }
            return parsed_expr;
        }
        case infinity_thrift_rpc::LiteralType::SparseDoubleArray: {
            auto parsed_expr = new ConstantExpr(LiteralType::kDoubleSparseArray);
            parsed_expr->double_sparse_array_.first.reserve(expr.i64_array_idx.size());
            parsed_expr->double_sparse_array_.second.reserve(expr.f64_array_value.size());
            for (auto &value : expr.i64_array_idx) {
                parsed_expr->double_sparse_array_.first.emplace_back(value);
            }
            for (auto &value : expr.f64_array_value) {
                parsed_expr->double_sparse_array_.second.emplace_back(value);
            }
            return parsed_expr;
        }
        case infinity_thrift_rpc::LiteralType::Date: {
            auto parsed_expr = new ConstantExpr(LiteralType::kDate);
            parsed_expr->date_value_ = strdup(expr.str_value.c_str());
            return parsed_expr;
        }
        case infinity_thrift_rpc::LiteralType::Time: {
            auto parsed_expr = new ConstantExpr(LiteralType::kTime);
            parsed_expr->date_value_ = strdup(expr.str_value.c_str());
            return parsed_expr;
        }
        case infinity_thrift_rpc::LiteralType::DateTime: {
            auto parsed_expr = new ConstantExpr(LiteralType::kDateTime);
            parsed_expr->date_value_ = strdup(expr.str_value.c_str());
            return parsed_expr;
        }

        default: {
            status = Status::InvalidConstantType();
            return nullptr;
        }
    }
}

ColumnExpr *InfinityThriftService::GetColumnExprFromProto(const infinity_thrift_rpc::ColumnExpr &column_expr) {
    auto parsed_expr = new ColumnExpr();

    for (auto column_name : column_expr.column_name) {
        parsed_expr->names_.emplace_back(column_name);
    }

    parsed_expr->star_ = column_expr.star;
    return parsed_expr;
}

FunctionExpr *InfinityThriftService::GetFunctionExprFromProto(Status &status, const infinity_thrift_rpc::FunctionExpr &function_expr) {
    auto *parsed_expr = new FunctionExpr();
    parsed_expr->func_name_ = function_expr.function_name;
    Vector<ParsedExpr *> *arguments;
    arguments = new Vector<ParsedExpr *>();
    arguments->reserve(function_expr.arguments.size());

    for (auto &args : function_expr.arguments) {
        arguments->emplace_back(GetParsedExprFromProto(status, args));
        if (!status.ok()) {
            if (parsed_expr != nullptr) {
                delete parsed_expr;
                parsed_expr = nullptr;
            }
            if (arguments != nullptr) {
                for (auto &argument : *arguments) {
                    delete argument;
                    argument = nullptr;
                }
                delete arguments;
                arguments = nullptr;
            }
            return nullptr;
        }
    }

    parsed_expr->arguments_ = arguments;
    return parsed_expr;
}

KnnExpr *InfinityThriftService::GetKnnExprFromProto(Status &status, const infinity_thrift_rpc::KnnExpr &expr) {
    auto knn_expr = MakeUnique<KnnExpr>(false);
    knn_expr->column_expr_ = GetColumnExprFromProto(expr.column_expr);

    knn_expr->distance_type_ = GetDistanceTypeFormProto(expr.distance_type);
    if (knn_expr->distance_type_ == KnnDistanceType::kInvalid) {
        status = Status::InvalidKnnDistanceType();
        return nullptr;
    }
    knn_expr->embedding_data_type_ = GetEmbeddingDataTypeFromProto(expr.embedding_data_type);
    if (knn_expr->embedding_data_type_ == EmbeddingDataType::kElemInvalid) {
        status = Status::InvalidEmbeddingDataType("invalid");
        return nullptr;
    }

    auto [embedding_data_ptr, dimension, status2] = GetEmbeddingDataTypeDataPtrFromProto(expr.embedding_data);
    knn_expr->embedding_data_ptr_ = embedding_data_ptr;
    knn_expr->dimension_ = dimension;
    if (!status2.ok()) {
        status = status2;
        return nullptr;
    }

    knn_expr->topn_ = expr.topn;
    if (knn_expr->topn_ <= 0) {
        status = Status::InvalidParameterValue("topn", std::to_string(expr.topn), "topn should be greater than 0");
        return nullptr;
    }

    knn_expr->opt_params_ = new Vector<InitParameter *>();
    for (auto &param : expr.opt_params) {
        if (param.param_name == "index_name") {
            knn_expr->index_name_ = param.param_value;
        }
        if (param.param_name == "ignore_index" && param.param_value == "true") {
            knn_expr->ignore_index_ = true;
        }

        auto init_parameter = new InitParameter();
        init_parameter->param_name_ = param.param_name;
        init_parameter->param_value_ = param.param_value;
        knn_expr->opt_params_->emplace_back(init_parameter);
    }
    if (expr.__isset.filter_expr) {
        knn_expr->filter_expr_.reset(GetParsedExprFromProto(status, expr.filter_expr));
        if (!status.ok()) {
            return nullptr;
        }
    }
    status = Status::OK();
    return knn_expr.release();
}

MatchSparseExpr *InfinityThriftService::GetMatchSparseExprFromProto(Status &status, const infinity_thrift_rpc::MatchSparseExpr &expr) {
    auto match_sparse_expr = MakeUnique<MatchSparseExpr>(true);

    auto *column_expr = static_cast<ParsedExpr *>(GetColumnExprFromProto(expr.column_expr));
    DeferFn defer_fn2([&] {
        if (column_expr != nullptr) {
            delete column_expr;
            column_expr = nullptr;
        }
    });
    match_sparse_expr->SetSearchColumn(column_expr);
    column_expr = nullptr;

    auto *constant_expr = GetConstantFromProto(status, expr.query_sparse_expr);
    DeferFn defer_fn3([&] {
        if (constant_expr != nullptr) {
            delete constant_expr;
            constant_expr = nullptr;
        }
    });

    match_sparse_expr->SetQuerySparse(constant_expr);
    constant_expr = nullptr;

    match_sparse_expr->SetMetricType(expr.metric_type);

    auto *opt_params_ptr = new Vector<InitParameter *>();
    for (auto &param : expr.opt_params) {
        auto *init_parameter = new InitParameter();
        init_parameter->param_name_ = param.param_name;
        init_parameter->param_value_ = param.param_value;
        opt_params_ptr->emplace_back(init_parameter);
    }
    match_sparse_expr->SetOptParams(expr.topn, opt_params_ptr);

    if (expr.__isset.filter_expr) {
        match_sparse_expr->filter_expr_.reset(GetParsedExprFromProto(status, expr.filter_expr));
        if (!status.ok()) {
            return nullptr;
        }
    }
    status = Status::OK();

    return match_sparse_expr.release();
}

MatchTensorExpr *InfinityThriftService::GetMatchTensorExprFromProto(Status &status, const infinity_thrift_rpc::MatchTensorExpr &expr) {
    auto match_tensor_expr = MakeUnique<MatchTensorExpr>();
    match_tensor_expr->SetSearchMethodStr(expr.search_method);
    match_tensor_expr->column_expr_.reset(GetColumnExprFromProto(expr.column_expr));
    match_tensor_expr->embedding_data_type_ = GetEmbeddingDataTypeFromProto(expr.embedding_data_type);
    auto [embedding_data_ptr, dimension, status2] = GetEmbeddingDataTypeDataPtrFromProto(expr.embedding_data);
    if (!status2.ok()) {
        status = status2;
        return nullptr;
    }
    match_tensor_expr->dimension_ = dimension;
    const auto copy_bytes = EmbeddingT::EmbeddingSize(match_tensor_expr->embedding_data_type_, match_tensor_expr->dimension_);
    match_tensor_expr->query_tensor_data_ptr_ = MakeUniqueForOverwrite<char[]>(copy_bytes);
    std::memcpy(match_tensor_expr->query_tensor_data_ptr_.get(), embedding_data_ptr, copy_bytes);
    match_tensor_expr->options_text_ = expr.extra_options;
    if (expr.__isset.filter_expr) {
        match_tensor_expr->filter_expr_.reset(GetParsedExprFromProto(status, expr.filter_expr));
        if (!status.ok()) {
            return nullptr;
        }
    }
    status = Status::OK();
    return match_tensor_expr.release();
}

MatchExpr *InfinityThriftService::GetMatchExprFromProto(Status &status, const infinity_thrift_rpc::MatchExpr &expr) {
    auto match_expr = MakeUnique<MatchExpr>();
    match_expr->fields_ = expr.fields;
    match_expr->matching_text_ = expr.matching_text;
    match_expr->options_text_ = expr.options_text;
    if (expr.__isset.filter_expr) {
        match_expr->filter_expr_.reset(GetParsedExprFromProto(status, expr.filter_expr));
        if (!status.ok()) {
            return nullptr;
        }
    }
    return match_expr.release();
}

ParsedExpr *InfinityThriftService::GetGenericMatchExprFromProto(Status &status, const infinity_thrift_rpc::GenericMatchExpr &expr) {
    if (expr.__isset.match_vector_expr == true) {
        auto parsed_expr = GetKnnExprFromProto(status, *expr.match_vector_expr);
        return parsed_expr;
    } else if (expr.__isset.match_sparse_expr == true) {
        auto parsed_expr = GetMatchSparseExprFromProto(status, *expr.match_sparse_expr);
        return parsed_expr;
    } else if (expr.__isset.match_tensor_expr == true) {
        auto parsed_expr = GetMatchTensorExprFromProto(status, *expr.match_tensor_expr);
        return parsed_expr;
    } else if (expr.__isset.match_text_expr == true) {
        auto parsed_expr = GetMatchExprFromProto(status, *expr.match_text_expr);
        return parsed_expr;
    } else {
        status = Status::InvalidParsedExprType();
    }
    return nullptr;
}

FusionExpr *InfinityThriftService::GetFusionExprFromProto(const infinity_thrift_rpc::FusionExpr &expr) {
    auto fusion_expr = MakeUnique<FusionExpr>();
    fusion_expr->method_ = expr.method;
    fusion_expr->SetOptions(expr.options_text);
    if (expr.__isset.optional_match_tensor_expr) {
        Status status;
        const auto result_ptr = GetMatchTensorExprFromProto(status, expr.optional_match_tensor_expr);
        fusion_expr->match_tensor_expr_.reset(result_ptr);
        if (!status.ok()) {
            fusion_expr.reset();
        }
    }
    return fusion_expr.release();
}

InExpr *InfinityThriftService::GetInExprFromProto(Status &status, const infinity_thrift_rpc::InExpr &in_expr) {
    auto parsed_expr = MakeUnique<InExpr>();
    parsed_expr->left_ = GetParsedExprFromProto(status, in_expr.left_operand);
    if (!status.ok()) {
        return nullptr;
    }
    parsed_expr->arguments_ = new Vector<ParsedExpr *>();
    parsed_expr->arguments_->reserve(in_expr.arguments.size());
    for (auto &arg : in_expr.arguments) {
        parsed_expr->arguments_->emplace_back(GetParsedExprFromProto(status, arg));
        if (!status.ok()) {
            return nullptr;
        }
    }
    parsed_expr->in_ = in_expr.in_type;
    return parsed_expr.release();
}

ParsedExpr *InfinityThriftService::GetParsedExprFromProto(Status &status, const infinity_thrift_rpc::ParsedExpr &expr) {
    if (expr.type.__isset.column_expr == true) {
        auto parsed_expr = GetColumnExprFromProto(*expr.type.column_expr);
        return parsed_expr;
    } else if (expr.type.__isset.constant_expr == true) {
        auto parsed_expr = GetConstantFromProto(status, *expr.type.constant_expr);
        return parsed_expr;
    } else if (expr.type.__isset.function_expr == true) {
        auto parsed_expr = GetFunctionExprFromProto(status, *expr.type.function_expr);
        return parsed_expr;
    } else if (expr.type.__isset.knn_expr == true) {
        auto parsed_expr = GetKnnExprFromProto(status, *expr.type.knn_expr);
        return parsed_expr;
    } else if (expr.type.__isset.match_expr == true) {
        auto parsed_expr = GetMatchExprFromProto(status, *expr.type.match_expr);
        return parsed_expr;
    } else if (expr.type.__isset.fusion_expr == true) {
        auto parsed_expr = GetFusionExprFromProto(*expr.type.fusion_expr);
        return parsed_expr;
    } else if (expr.type.__isset.in_expr == true) {
        auto parsed_expr = GetInExprFromProto(status, *expr.type.in_expr);
        return parsed_expr;
    } else {
        status = Status::InvalidParsedExprType();
    }
    return nullptr;
}

OrderByExpr *InfinityThriftService::GetOrderByExprFromProto(Status &status, const infinity_thrift_rpc::OrderByExpr &expr) {
    auto order_by_expr = MakeUnique<OrderByExpr>();
    order_by_expr->expr_ = GetParsedExprFromProto(status, expr.expr);
    if (!status.ok()) {
        return nullptr;
    }
    if (expr.asc) {
        order_by_expr->type_ = OrderType::kAsc;
    } else {
        order_by_expr->type_ = OrderType::kDesc;
    }
    return order_by_expr.release();
}

KnnDistanceType InfinityThriftService::GetDistanceTypeFormProto(const infinity_thrift_rpc::KnnDistanceType::type &type) {
    switch (type) {
        case infinity_thrift_rpc::KnnDistanceType::L2:
            return KnnDistanceType::kL2;
        case infinity_thrift_rpc::KnnDistanceType::Cosine:
            return KnnDistanceType::kCosine;
        case infinity_thrift_rpc::KnnDistanceType::InnerProduct:
            return KnnDistanceType::kInnerProduct;
        case infinity_thrift_rpc::KnnDistanceType::Hamming:
            return KnnDistanceType::kHamming;
        default:
            return KnnDistanceType::kInvalid;
    }
}

ExplainType InfinityThriftService::GetExplainTypeFromProto(const infinity_thrift_rpc::ExplainType::type &type) {
    switch (type) {
        case infinity_thrift_rpc::ExplainType::Analyze:
            return ExplainType::kAnalyze;
        case infinity_thrift_rpc::ExplainType::Ast:
            return ExplainType::kAst;
        case infinity_thrift_rpc::ExplainType::Physical:
            return ExplainType ::kPhysical;
        case infinity_thrift_rpc::ExplainType::Pipeline:
            return ExplainType ::kPipeline;
        case infinity_thrift_rpc::ExplainType::UnOpt:
            return ExplainType ::kUnOpt;
        case infinity_thrift_rpc::ExplainType::Opt:
            return ExplainType ::kOpt;
        case infinity_thrift_rpc::ExplainType::Fragment:
            return ExplainType ::kFragment;
        default:
            return ExplainType::kInvalid;
    }
}

Tuple<void *, i64, Status> InfinityThriftService::GetEmbeddingDataTypeDataPtrFromProto(const infinity_thrift_rpc::EmbeddingData &embedding_data) {
    if (embedding_data.__isset.u8_array_value) {
        auto ptr_i16 = (int16_t *)(embedding_data.u8_array_value.data());
        auto ptr_u8 = (uint8_t *)(embedding_data.u8_array_value.data());
        for (size_t i = 0; i < embedding_data.u8_array_value.size(); ++i) {
            ptr_u8[i] = static_cast<uint8_t>(ptr_i16[i]);
        }
        return {(void *)embedding_data.u8_array_value.data(), embedding_data.u8_array_value.size(), Status::OK()};
    } else if (embedding_data.__isset.i8_array_value) {
        auto ptr_i16 = (int16_t *)(embedding_data.i8_array_value.data());
        auto ptr_i8 = (int8_t *)(embedding_data.i8_array_value.data());
        for (size_t i = 0; i < embedding_data.i8_array_value.size(); ++i) {
            ptr_i8[i] = static_cast<int8_t>(ptr_i16[i]);
        }
        return {(void *)embedding_data.i8_array_value.data(), embedding_data.i8_array_value.size(), Status::OK()};
    } else if (embedding_data.__isset.i16_array_value) {
        return {(void *)embedding_data.i16_array_value.data(), embedding_data.i16_array_value.size(), Status::OK()};
    } else if (embedding_data.__isset.i32_array_value) {
        return {(void *)embedding_data.i32_array_value.data(), embedding_data.i32_array_value.size(), Status::OK()};
    } else if (embedding_data.__isset.i64_array_value) {
        return {(void *)embedding_data.i64_array_value.data(), embedding_data.i64_array_value.size(), Status::OK()};
    } else if (embedding_data.__isset.f32_array_value) {
        auto ptr_double = (double *)(embedding_data.f32_array_value.data());
        auto ptr_float = (float *)(embedding_data.f32_array_value.data());
        for (size_t i = 0; i < embedding_data.f32_array_value.size(); ++i) {
            ptr_float[i] = float(ptr_double[i]);
        }
        return {(void *)embedding_data.f32_array_value.data(), embedding_data.f32_array_value.size(), Status::OK()};
    } else if (embedding_data.__isset.f64_array_value) {
        return {(void *)embedding_data.f64_array_value.data(), embedding_data.f64_array_value.size(), Status::OK()};
    } else if (embedding_data.__isset.f16_array_value) {
        auto ptr_double = (double *)(embedding_data.f16_array_value.data());
        auto ptr_float16 = (Float16T *)(embedding_data.f16_array_value.data());
        for (size_t i = 0; i < embedding_data.f16_array_value.size(); ++i) {
            ptr_float16[i] = float(ptr_double[i]);
        }
        return {(void *)embedding_data.f16_array_value.data(), embedding_data.f16_array_value.size(), Status::OK()};
    } else if (embedding_data.__isset.bf16_array_value) {
        auto ptr_double = (double *)(embedding_data.bf16_array_value.data());
        auto ptr_bfloat16 = (BFloat16T *)(embedding_data.bf16_array_value.data());
        for (size_t i = 0; i < embedding_data.bf16_array_value.size(); ++i) {
            ptr_bfloat16[i] = float(ptr_double[i]);
        }
        return {(void *)embedding_data.bf16_array_value.data(), embedding_data.bf16_array_value.size(), Status::OK()};
    } else {
        return {nullptr, 0, Status::InvalidEmbeddingDataType("unknown type")};
    }
}

Tuple<UpdateExpr *, Status> InfinityThriftService::GetUpdateExprFromProto(const infinity_thrift_rpc::UpdateExpr &update_expr) {
    Status status;
    auto up_expr = new UpdateExpr();
    up_expr->column_name = update_expr.column_name;
    up_expr->value = GetParsedExprFromProto(status, update_expr.value);
    return {up_expr, status};
}

OptimizeOptions InfinityThriftService::GetParsedOptimizeOptionFromProto(const infinity_thrift_rpc::OptimizeOptions &options) {
    OptimizeOptions opt;
    opt.index_name_ = options.index_name;
    for (const auto &param : options.opt_params) {
        auto *init_param = new InitParameter();
        init_param->param_name_ = param.param_name;
        init_param->param_value_ = param.param_value;
        opt.opt_params_.emplace_back(init_param);
    }
    return opt;
}

infinity_thrift_rpc::ColumnType::type InfinityThriftService::DataTypeToProtoColumnType(const SharedPtr<DataType> &data_type) {
    switch (data_type->type()) {
        case LogicalType::kBoolean:
            return infinity_thrift_rpc::ColumnType::ColumnBool;
        case LogicalType::kTinyInt:
            return infinity_thrift_rpc::ColumnType::ColumnInt8;
        case LogicalType::kSmallInt:
            return infinity_thrift_rpc::ColumnType::ColumnInt16;
        case LogicalType::kInteger:
            return infinity_thrift_rpc::ColumnType::ColumnInt32;
        case LogicalType::kBigInt:
            return infinity_thrift_rpc::ColumnType::ColumnInt64;
        case LogicalType::kFloat:
            return infinity_thrift_rpc::ColumnType::ColumnFloat32;
        case LogicalType::kDouble:
            return infinity_thrift_rpc::ColumnType::ColumnFloat64;
        case LogicalType::kFloat16:
            return infinity_thrift_rpc::ColumnType::ColumnFloat16;
        case LogicalType::kBFloat16:
            return infinity_thrift_rpc::ColumnType::ColumnBFloat16;
        case LogicalType::kVarchar:
            return infinity_thrift_rpc::ColumnType::ColumnVarchar;
        case LogicalType::kEmbedding:
            return infinity_thrift_rpc::ColumnType::ColumnEmbedding;
        case LogicalType::kMultiVector:
            return infinity_thrift_rpc::ColumnType::ColumnMultiVector;
        case LogicalType::kTensor:
            return infinity_thrift_rpc::ColumnType::ColumnTensor;
        case LogicalType::kTensorArray:
            return infinity_thrift_rpc::ColumnType::ColumnTensorArray;
        case LogicalType::kRowID:
            return infinity_thrift_rpc::ColumnType::ColumnRowID;
        case LogicalType::kSparse:
            return infinity_thrift_rpc::ColumnType::ColumnSparse;
        case LogicalType::kDate:
            return infinity_thrift_rpc::ColumnType::ColumnDate;
        case LogicalType::kTime:
            return infinity_thrift_rpc::ColumnType::ColumnTime;
        case LogicalType::kDateTime:
            return infinity_thrift_rpc::ColumnType::ColumnDateTime;
        case LogicalType::kTimestamp:
            return infinity_thrift_rpc::ColumnType::ColumnTimestamp;
        case LogicalType::kInterval:
            return infinity_thrift_rpc::ColumnType::ColumnInterval;
        default: {
            String error_message = fmt::format("Invalid logical data type: {}", data_type->ToString());
            UnrecoverableError(error_message);
        }
    }
    return infinity_thrift_rpc::ColumnType::ColumnInvalid;
}

UniquePtr<infinity_thrift_rpc::DataType> InfinityThriftService::DataTypeToProtoDataType(const SharedPtr<DataType> &data_type) {
    switch (data_type->type()) {
        case LogicalType::kBoolean: {
            auto data_type_proto = MakeUnique<infinity_thrift_rpc::DataType>();
            data_type_proto->__set_logic_type(infinity_thrift_rpc::LogicType::Boolean);
            return data_type_proto;
        }
        case LogicalType::kTinyInt: {
            auto data_type_proto = MakeUnique<infinity_thrift_rpc::DataType>();
            data_type_proto->__set_logic_type(infinity_thrift_rpc::LogicType::TinyInt);
            return data_type_proto;
        }
        case LogicalType::kSmallInt: {
            auto data_type_proto = MakeUnique<infinity_thrift_rpc::DataType>();
            data_type_proto->__set_logic_type(infinity_thrift_rpc::LogicType::SmallInt);
            return data_type_proto;
        }
        case LogicalType::kInteger: {
            auto data_type_proto = MakeUnique<infinity_thrift_rpc::DataType>();
            data_type_proto->__set_logic_type(infinity_thrift_rpc::LogicType::Integer);
            return data_type_proto;
        }
        case LogicalType::kBigInt: {
            auto data_type_proto = MakeUnique<infinity_thrift_rpc::DataType>();
            data_type_proto->__set_logic_type(infinity_thrift_rpc::LogicType::BigInt);
            return data_type_proto;
        }
        case LogicalType::kFloat: {
            auto data_type_proto = MakeUnique<infinity_thrift_rpc::DataType>();
            data_type_proto->__set_logic_type(infinity_thrift_rpc::LogicType::Float);
            return data_type_proto;
        }
        case LogicalType::kDouble: {
            auto data_type_proto = MakeUnique<infinity_thrift_rpc::DataType>();
            data_type_proto->__set_logic_type(infinity_thrift_rpc::LogicType::Double);
            return data_type_proto;
        }
        case LogicalType::kFloat16: {
            auto data_type_proto = MakeUnique<infinity_thrift_rpc::DataType>();
            data_type_proto->__set_logic_type(infinity_thrift_rpc::LogicType::Float16);
            return data_type_proto;
        }
        case LogicalType::kBFloat16: {
            auto data_type_proto = MakeUnique<infinity_thrift_rpc::DataType>();
            data_type_proto->__set_logic_type(infinity_thrift_rpc::LogicType::BFloat16);
            return data_type_proto;
        }
        case LogicalType::kVarchar: {
            auto data_type_proto = MakeUnique<infinity_thrift_rpc::DataType>();
            infinity_thrift_rpc::VarcharType varchar_type;
            data_type_proto->__set_logic_type(infinity_thrift_rpc::LogicType::Varchar);
            infinity_thrift_rpc::PhysicalType physical_type;
            physical_type.__set_varchar_type(varchar_type);
            data_type_proto->__set_physical_type(physical_type);
            return data_type_proto;
        }
        case LogicalType::kTensor:
        case LogicalType::kTensorArray:
        case LogicalType::kMultiVector:
        case LogicalType::kEmbedding: {
            auto data_type_proto = MakeUnique<infinity_thrift_rpc::DataType>();
            infinity_thrift_rpc::EmbeddingType embedding_type;
            auto embedding_info = static_cast<EmbeddingInfo *>(data_type->type_info().get());
            embedding_type.__set_dimension(embedding_info->Dimension());
            embedding_type.__set_element_type(EmbeddingDataTypeToProtoElementType(embedding_info->Type()));
            switch (data_type->type()) {
                case LogicalType::kTensor: {
                    data_type_proto->__set_logic_type(infinity_thrift_rpc::LogicType::Tensor);
                    break;
                }
                case LogicalType::kTensorArray: {
                    data_type_proto->__set_logic_type(infinity_thrift_rpc::LogicType::TensorArray);
                    break;
                }
                case LogicalType::kMultiVector: {
                    data_type_proto->__set_logic_type(infinity_thrift_rpc::LogicType::MultiVector);
                    break;
                }
                case LogicalType::kEmbedding: {
                    data_type_proto->__set_logic_type(infinity_thrift_rpc::LogicType::Embedding);
                    break;
                }
                default: {
                    UnrecoverableError("Invalid data type");
                }
            }
            infinity_thrift_rpc::PhysicalType physical_type;
            physical_type.__set_embedding_type(embedding_type);
            data_type_proto->__set_physical_type(physical_type);
            return data_type_proto;
        }
        case LogicalType::kSparse: {
            auto data_type_proto = MakeUnique<infinity_thrift_rpc::DataType>();
            data_type_proto->__set_logic_type(infinity_thrift_rpc::LogicType::Sparse);

            infinity_thrift_rpc::SparseType sparse_type;
            auto *sparse_info = static_cast<SparseInfo *>(data_type->type_info().get());
            sparse_type.__set_dimension(sparse_info->Dimension());
            sparse_type.__set_element_type(EmbeddingDataTypeToProtoElementType(sparse_info->DataType()));
            sparse_type.__set_index_type(EmbeddingDataTypeToProtoElementType(sparse_info->IndexType()));

            infinity_thrift_rpc::PhysicalType physical_type;
            physical_type.__set_sparse_type(sparse_type);
            data_type_proto->__set_physical_type(physical_type);
            return data_type_proto;
        }
        case LogicalType::kTime: {
            auto data_type_proto = MakeUnique<infinity_thrift_rpc::DataType>();
            data_type_proto->__set_logic_type(infinity_thrift_rpc::LogicType::Time);
            return data_type_proto;
        }
        case LogicalType::kDate: {
            auto data_type_proto = MakeUnique<infinity_thrift_rpc::DataType>();
            data_type_proto->__set_logic_type(infinity_thrift_rpc::LogicType::Date);
            return data_type_proto;
        }
        case LogicalType::kDateTime: {
            auto data_type_proto = MakeUnique<infinity_thrift_rpc::DataType>();
            data_type_proto->__set_logic_type(infinity_thrift_rpc::LogicType::DateTime);
            return data_type_proto;
        }
        case LogicalType::kTimestamp: {
            auto data_type_proto = MakeUnique<infinity_thrift_rpc::DataType>();
            data_type_proto->__set_logic_type(infinity_thrift_rpc::LogicType::Timestamp);
            return data_type_proto;
        }
        case LogicalType::kInterval: {
            auto data_type_proto = MakeUnique<infinity_thrift_rpc::DataType>();
            data_type_proto->__set_logic_type(infinity_thrift_rpc::LogicType::Interval);
            return data_type_proto;
        }
        default: {
            String error_message = fmt::format("Invalid logical data type: {}", data_type->ToString());
            UnrecoverableError(error_message);
        }
    }
    return nullptr;
}

infinity_thrift_rpc::ElementType::type InfinityThriftService::EmbeddingDataTypeToProtoElementType(const EmbeddingDataType &embedding_data_type) {
    switch (embedding_data_type) {
        case EmbeddingDataType::kElemBit:
            return infinity_thrift_rpc::ElementType::ElementBit;
        case EmbeddingDataType::kElemUInt8:
            return infinity_thrift_rpc::ElementType::ElementUInt8;
        case EmbeddingDataType::kElemInt8:
            return infinity_thrift_rpc::ElementType::ElementInt8;
        case EmbeddingDataType::kElemInt16:
            return infinity_thrift_rpc::ElementType::ElementInt16;
        case EmbeddingDataType::kElemInt32:
            return infinity_thrift_rpc::ElementType::ElementInt32;
        case EmbeddingDataType::kElemInt64:
            return infinity_thrift_rpc::ElementType::ElementInt64;
        case EmbeddingDataType::kElemFloat:
            return infinity_thrift_rpc::ElementType::ElementFloat32;
        case EmbeddingDataType::kElemDouble:
            return infinity_thrift_rpc::ElementType::ElementFloat64;
        case EmbeddingDataType::kElemFloat16:
            return infinity_thrift_rpc::ElementType::ElementFloat16;
        case EmbeddingDataType::kElemBFloat16:
            return infinity_thrift_rpc::ElementType::ElementBFloat16;
        case EmbeddingDataType::kElemInvalid: {
            String error_message = fmt::format("Invalid embedding element data type: {}", static_cast<i8>(embedding_data_type));
            UnrecoverableError(error_message);
        }
    }
    return infinity_thrift_rpc::ElementType::ElementFloat32;
}

void InfinityThriftService::ProcessDataBlocks(const QueryResult &result,
                                              infinity_thrift_rpc::SelectResponse &response,
                                              Vector<infinity_thrift_rpc::ColumnField> &columns) {
    SizeT blocks_count = result.result_table_->DataBlockCount();
    for (SizeT block_idx = 0; block_idx < blocks_count; ++block_idx) {
        auto data_block = result.result_table_->GetDataBlockById(block_idx);
        Status status = ProcessColumns(data_block, result.result_table_->ColumnCount(), columns);
        if (!status.ok()) {
            ProcessStatus(response, status);
            return;
        }
    }
    HandleColumnDef(response, result.result_table_->ColumnCount(), result.result_table_->definition_ptr_, columns);
}

Status
InfinityThriftService::ProcessColumns(const SharedPtr<DataBlock> &data_block, SizeT column_count, Vector<infinity_thrift_rpc::ColumnField> &columns) {
    auto row_count = data_block->row_count();
    for (SizeT col_index = 0; col_index < column_count; ++col_index) {
        auto &result_column_vector = data_block->column_vectors[col_index];
        infinity_thrift_rpc::ColumnField &output_column_field = columns[col_index];
        output_column_field.__set_column_type(DataTypeToProtoColumnType(result_column_vector->data_type()));
        Status status = ProcessColumnFieldType(output_column_field, row_count, result_column_vector);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

void InfinityThriftService::HandleColumnDef(infinity_thrift_rpc::SelectResponse &response,
                                            SizeT column_count,
                                            SharedPtr<TableDef> table_def,
                                            Vector<infinity_thrift_rpc::ColumnField> &all_column_vectors) {
    if (column_count != all_column_vectors.size()) {
        ProcessStatus(response, Status::ColumnCountMismatch(fmt::format("expect: {}, actual: {}", column_count, all_column_vectors.size())));
        return;
    }
    for (SizeT col_index = 0; col_index < column_count; ++col_index) {
        auto column_def = table_def->columns()[col_index];
        infinity_thrift_rpc::ColumnDef proto_column_def;
        proto_column_def.__set_id(column_def->id());
        proto_column_def.__set_name(column_def->name());

        infinity_thrift_rpc::DataType proto_data_type;
        proto_column_def.__set_data_type(*DataTypeToProtoDataType(column_def->type()));

        response.column_defs.emplace_back(proto_column_def);
    }
    response.__set_error_code((i64)(ErrorCode::kOk));
}

Status InfinityThriftService::ProcessColumnFieldType(infinity_thrift_rpc::ColumnField &output_column_field,
                                                     SizeT row_count,
                                                     const SharedPtr<ColumnVector> &column_vector) {
    switch (column_vector->data_type()->type()) {
        case LogicalType::kBoolean: {
            HandleBoolType(output_column_field, row_count, column_vector);
            break;
        }
        case LogicalType::kTinyInt:
        case LogicalType::kSmallInt:
        case LogicalType::kInteger:
        case LogicalType::kBigInt:
        case LogicalType::kHugeInt:
        case LogicalType::kFloat16:
        case LogicalType::kBFloat16:
        case LogicalType::kFloat:
        case LogicalType::kDouble: {
            HandlePodType(output_column_field, row_count, column_vector);
            break;
        }
        case LogicalType::kVarchar: {
            HandleVarcharType(output_column_field, row_count, column_vector);
            break;
        }
        case LogicalType::kEmbedding: {
            HandleEmbeddingType(output_column_field, row_count, column_vector);
            break;
        }
        case LogicalType::kMultiVector: {
            HandleMultiVectorType(output_column_field, row_count, column_vector);
            break;
        }
        case LogicalType::kTensor: {
            HandleTensorType(output_column_field, row_count, column_vector);
            break;
        }
        case LogicalType::kTensorArray: {
            HandleTensorArrayType(output_column_field, row_count, column_vector);
            break;
        }
        case LogicalType::kSparse: {
            HandleSparseType(output_column_field, row_count, column_vector);
            break;
        }
        case LogicalType::kRowID: {
            HandleRowIDType(output_column_field, row_count, column_vector);
            break;
        }
        case LogicalType::kDate:
        case LogicalType::kTime:
        case LogicalType::kDateTime:
        case LogicalType::kTimestamp:
        case LogicalType::kInterval: {
            HandleTimeRelatedTypes(output_column_field, row_count, column_vector);
            break;
        }
        default: {
            return Status::InvalidDataType();
        }
    }
    return Status::OK();
}

void InfinityThriftService::HandleTimeRelatedTypes(infinity_thrift_rpc::ColumnField &output_column_field,
                                                   SizeT row_count,
                                                   const SharedPtr<ColumnVector> &column_vector) {
    auto size = column_vector->data_type()->Size() * row_count;
    String dst;
    dst.resize(size);
    std::memcpy(dst.data(), column_vector->data(), size);
    output_column_field.column_vectors.emplace_back(std::move(dst));
}

void InfinityThriftService::HandleBoolType(infinity_thrift_rpc::ColumnField &output_column_field,
                                           SizeT row_count,
                                           const SharedPtr<ColumnVector> &column_vector) {
    String dst;
    dst.reserve(row_count);
    for (SizeT index = 0; index < row_count; ++index) {
        const char c = column_vector->buffer_->GetCompactBit(index) ? 1 : 0;
        dst.push_back(c);
    }
    output_column_field.column_vectors.emplace_back(std::move(dst));
}

void InfinityThriftService::HandlePodType(infinity_thrift_rpc::ColumnField &output_column_field,
                                          SizeT row_count,
                                          const SharedPtr<ColumnVector> &column_vector) {
    auto size = column_vector->data_type()->Size() * row_count;
    String dst;
    dst.resize(size);
    std::memcpy(dst.data(), column_vector->data(), size);
    output_column_field.column_vectors.emplace_back(std::move(dst));
}

void InfinityThriftService::HandleVarcharType(infinity_thrift_rpc::ColumnField &output_column_field,
                                              SizeT row_count,
                                              const SharedPtr<ColumnVector> &column_vector) {
    SizeT all_size = 0;
    Vector<Pair<const char *, SizeT>> raw_data;
    for (SizeT index = 0; index < row_count; ++index) {
        Span<const char> data = column_vector->GetVarchar(index);
        all_size += sizeof(i32) + data.size();
        raw_data.emplace_back(data.data(), data.size());
    }
    String dst;
    dst.resize(all_size);
    SizeT current_offset = 0;
    for (const auto [data_ptr, data_size] : raw_data) {
        i32 length = data_size;
        std::memcpy(dst.data() + current_offset, &length, sizeof(i32));
        std::memcpy(dst.data() + current_offset + sizeof(i32), data_ptr, data_size);
        current_offset += sizeof(i32) + length;
    }

    output_column_field.column_vectors.emplace_back(std::move(dst));
    output_column_field.__set_column_type(DataTypeToProtoColumnType(column_vector->data_type()));
}

void InfinityThriftService::HandleEmbeddingType(infinity_thrift_rpc::ColumnField &output_column_field,
                                                SizeT row_count,
                                                const SharedPtr<ColumnVector> &column_vector) {
    auto size = column_vector->data_type()->Size() * row_count;
    String dst;
    dst.resize(size);
    std::memcpy(dst.data(), column_vector->data(), size);
    output_column_field.column_vectors.emplace_back(std::move(dst));
    output_column_field.__set_column_type(DataTypeToProtoColumnType(column_vector->data_type()));
}

void InfinityThriftService::HandleMultiVectorType(infinity_thrift_rpc::ColumnField &output_column_field,
                                                  SizeT row_count,
                                                  const SharedPtr<ColumnVector> &column_vector) {
    SizeT all_size = 0;
    Vector<Pair<const char *, SizeT>> multi_vector_data(row_count);
    for (SizeT index = 0; index < row_count; ++index) {
        Span<const char> raw_data = column_vector->GetMultiVectorRaw(index).first;
        all_size += sizeof(i32) + raw_data.size();
        multi_vector_data[index] = {raw_data.data(), raw_data.size()};
    }
    String dst;
    dst.resize(all_size);

    i32 current_offset = 0;
    for (SizeT index = 0; index < row_count; ++index) {
        const auto &[data, length] = multi_vector_data[index];
        std::memcpy(dst.data() + current_offset, &length, sizeof(i32));
        std::memcpy(dst.data() + current_offset + sizeof(i32), data, length);
        current_offset += sizeof(i32) + length;
    }

    output_column_field.column_vectors.emplace_back(std::move(dst));
    output_column_field.__set_column_type(DataTypeToProtoColumnType(column_vector->data_type()));
}

void InfinityThriftService::HandleTensorType(infinity_thrift_rpc::ColumnField &output_column_field,
                                             SizeT row_count,
                                             const SharedPtr<ColumnVector> &column_vector) {
    SizeT all_size = 0;
    Vector<Pair<const char *, SizeT>> tensor_data(row_count);
    for (SizeT index = 0; index < row_count; ++index) {
        Span<const char> raw_data = column_vector->GetTensorRaw(index).first;
        all_size += sizeof(i32) + raw_data.size();
        tensor_data[index] = {raw_data.data(), raw_data.size()};
    }
    String dst;
    dst.resize(all_size);

    i32 current_offset = 0;
    for (SizeT index = 0; index < row_count; ++index) {
        const auto &[data, length] = tensor_data[index];
        std::memcpy(dst.data() + current_offset, &length, sizeof(i32));
        std::memcpy(dst.data() + current_offset + sizeof(i32), data, length);
        current_offset += sizeof(i32) + length;
    }

    output_column_field.column_vectors.emplace_back(std::move(dst));
    output_column_field.__set_column_type(DataTypeToProtoColumnType(column_vector->data_type()));
}

void InfinityThriftService::HandleTensorArrayType(infinity_thrift_rpc::ColumnField &output_column_field,
                                                  SizeT row_count,
                                                  const SharedPtr<ColumnVector> &column_vector) {
    SizeT all_size = 0;
    Vector<Vector<Pair<const char *, SizeT>>> tensor_array_data;
    for (SizeT index = 0; index < row_count; ++index) {
        all_size += sizeof(i32);
        Vector<Pair<const char *, SizeT>> tensor_data;
        Vector<Pair<Span<const char>, SizeT>> array_data = column_vector->GetTensorArrayRaw(index);
        for (const auto [raw_data, embedding_num] : array_data) {
            all_size += sizeof(i32) + raw_data.size();
            tensor_data.emplace_back(raw_data.data(), raw_data.size());
        }
        tensor_array_data.push_back(std::move(tensor_data));
    }
    String dst;
    dst.resize(all_size);

    i32 current_offset = 0;
    for (const auto &tensor_data : tensor_array_data) {
        i32 tensor_num = tensor_data.size();
        std::memcpy(dst.data() + current_offset, &tensor_num, sizeof(i32));
        current_offset += sizeof(i32);
        for (const auto [raw_data, size] : tensor_data) {
            std::memcpy(dst.data() + current_offset, &size, sizeof(i32));
            std::memcpy(dst.data() + current_offset + sizeof(i32), raw_data, size);
            current_offset += sizeof(i32) + size;
        }
    }

    output_column_field.column_vectors.emplace_back(std::move(dst));
    output_column_field.__set_column_type(DataTypeToProtoColumnType(column_vector->data_type()));
}

void InfinityThriftService::HandleSparseType(infinity_thrift_rpc::ColumnField &output_column_field,
                                             SizeT row_count,
                                             const SharedPtr<ColumnVector> &column_vector) {
    SizeT all_size = 0;
    Vector<Tuple<Span<const char>, Span<const char>, SizeT>> raw_data;
    for (SizeT index = 0; index < row_count; ++index) {
        auto [data_span, index_span, nnz_size_t] = column_vector->GetSparseRaw(index);
        all_size += sizeof(i32) + data_span.size() + index_span.size();
        raw_data.emplace_back(data_span, index_span, nnz_size_t);
    }
    String dst;
    dst.resize(all_size);
    SizeT current_offset = 0;
    for (const auto [data_span, index_span, nnz_size_t] : raw_data) {
        i32 nnz = nnz_size_t;
        std::memcpy(dst.data() + current_offset, &nnz, sizeof(i32));
        current_offset += sizeof(i32);
        std::memcpy(dst.data() + current_offset, index_span.data(), index_span.size());
        current_offset += index_span.size();
        std::memcpy(dst.data() + current_offset, data_span.data(), data_span.size());
        current_offset += data_span.size();
    }

    output_column_field.column_vectors.emplace_back(std::move(dst));
    output_column_field.__set_column_type(DataTypeToProtoColumnType(column_vector->data_type()));
}

void InfinityThriftService::HandleRowIDType(infinity_thrift_rpc::ColumnField &output_column_field,
                                            SizeT row_count,
                                            const SharedPtr<ColumnVector> &column_vector) {
    auto size = column_vector->data_type()->Size() * row_count;
    String dst;
    dst.resize(size);
    std::memcpy(dst.data(), column_vector->data(), size);
    output_column_field.column_vectors.emplace_back(std::move(dst));
    output_column_field.__set_column_type(DataTypeToProtoColumnType(column_vector->data_type()));
}

void InfinityThriftService::ProcessStatus(infinity_thrift_rpc::CommonResponse &response, const Status &status, const std::string_view error_header) {
    response.__set_error_code((i64)(status.code()));
    if (!status.ok()) {
        response.__set_error_msg(status.message());
        LOG_ERROR(fmt::format("{}: {}", error_header, status.message()));
    }
}

void InfinityThriftService::ProcessStatus(infinity_thrift_rpc::ShowDatabaseResponse &response,
                                          const Status &status,
                                          const std::string_view error_header) {
    response.__set_error_code((i64)(status.code()));
    if (!status.ok()) {
        response.__set_error_msg(status.message());
        LOG_ERROR(fmt::format("{}: {}", error_header, status.message()));
    }
}

void InfinityThriftService::ProcessStatus(infinity_thrift_rpc::ShowTableResponse &response,
                                          const Status &status,
                                          const std::string_view error_header) {
    response.__set_error_code((i64)(status.code()));
    if (!status.ok()) {
        response.__set_error_msg(status.message());
        LOG_ERROR(fmt::format("{}: {}", error_header, status.message()));
    }
}

void InfinityThriftService::ProcessStatus(infinity_thrift_rpc::ShowIndexResponse &response,
                                          const Status &status,
                                          const std::string_view error_header) {
    response.__set_error_code((i64)(status.code()));
    if (!status.ok()) {
        response.__set_error_msg(status.message());
        LOG_ERROR(fmt::format("{}: {}", error_header, status.message()));
    }
}

void InfinityThriftService::ProcessStatus(infinity_thrift_rpc::SelectResponse &response, const Status &status, const std::string_view error_header) {
    response.__set_error_code((i64)(status.code()));
    if (!status.ok()) {
        response.__set_error_msg(status.message());
        LOG_ERROR(fmt::format("{}: {}", error_header, status.message()));
    }
}

void InfinityThriftService::ProcessStatus(infinity_thrift_rpc::ListDatabaseResponse &response,
                                          const Status &status,
                                          const std::string_view error_header) {
    response.__set_error_code((i64)(status.code()));
    if (!status.ok()) {
        response.__set_error_msg(status.message());
        LOG_ERROR(fmt::format("{}: {}", error_header, status.message()));
    }
}

void InfinityThriftService::ProcessStatus(infinity_thrift_rpc::ListTableResponse &response,
                                          const Status &status,
                                          const std::string_view error_header) {
    response.__set_error_code((i64)(status.code()));
    if (!status.ok()) {
        response.__set_error_msg(status.message());
        LOG_ERROR(fmt::format("{}: {}", error_header, status.message()));
    }
}

void InfinityThriftService::ProcessStatus(infinity_thrift_rpc::ListIndexResponse &response,
                                          const Status &status,
                                          const std::string_view error_header) {
    response.__set_error_code((i64)(status.code()));
    if (!status.ok()) {
        response.__set_error_msg(status.message());
        LOG_ERROR(fmt::format("{}: {}", error_header, status.message()));
    }
}

void InfinityThriftService::ProcessStatus(infinity_thrift_rpc::ShowSegmentResponse &response,
                                          const Status &status,
                                          const std::string_view error_header) {
    response.__set_error_code((i64)(status.code()));
    if (!status.ok()) {
        response.__set_error_msg(status.message());
        LOG_ERROR(fmt::format("{}: {}", error_header, status.message()));
    }
}

void InfinityThriftService::ProcessStatus(infinity_thrift_rpc::ShowBlockResponse &response,
                                          const Status &status,
                                          const std::string_view error_header) {
    response.__set_error_code((i64)(status.code()));
    if (!status.ok()) {
        response.__set_error_msg(status.message());
        LOG_ERROR(fmt::format("{}: {}", error_header, status.message()));
    }
}

void InfinityThriftService::ProcessStatus(infinity_thrift_rpc::ShowBlockColumnResponse &response,
                                          const Status &status,
                                          const std::string_view error_header) {
    response.__set_error_code((i64)(status.code()));
    if (!status.ok()) {
        response.__set_error_msg(status.message());
        LOG_ERROR(fmt::format("{}: {}", error_header, status.message()));
    }
}

void InfinityThriftService::ProcessQueryResult(infinity_thrift_rpc::CommonResponse &response,
                                               const QueryResult &result,
                                               const std::string_view error_header) {
    response.__set_error_code((i64)(result.ErrorCode()));
    if (!result.IsOk()) {
        response.__set_error_msg(result.ErrorStr());
        LOG_ERROR(fmt::format("{}: {}", error_header, result.ErrorStr()));
    }
}

void InfinityThriftService::ProcessQueryResult(infinity_thrift_rpc::SelectResponse &response,
                                               const QueryResult &result,
                                               const std::string_view error_header) {
    response.__set_error_code((i64)(result.ErrorCode()));
    if (!result.IsOk()) {
        response.__set_error_msg(result.ErrorStr());
        LOG_ERROR(fmt::format("{}: {}", error_header, result.ErrorStr()));
    }
}

void InfinityThriftService::ProcessQueryResult(infinity_thrift_rpc::ListDatabaseResponse &response,
                                               const QueryResult &result,
                                               const std::string_view error_header) {
    response.__set_error_code((i64)(result.ErrorCode()));
    if (!result.IsOk()) {
        response.__set_error_msg(result.ErrorStr());
        LOG_ERROR(fmt::format("{}: {}", error_header, result.ErrorStr()));
    }
}

void InfinityThriftService::ProcessQueryResult(infinity_thrift_rpc::ListTableResponse &response,
                                               const QueryResult &result,
                                               const std::string_view error_header) {
    response.__set_error_code((i64)(result.ErrorCode()));
    if (!result.IsOk()) {
        response.__set_error_msg(result.ErrorStr());
        LOG_ERROR(fmt::format("{}: {}", error_header, result.ErrorStr()));
    }
}

void InfinityThriftService::ProcessQueryResult(infinity_thrift_rpc::ListIndexResponse &response,
                                               const QueryResult &result,
                                               const std::string_view error_header) {
    response.__set_error_code((i64)(result.ErrorCode()));
    if (!result.IsOk()) {
        response.__set_error_msg(result.ErrorStr());
        LOG_ERROR(fmt::format("{}: {}", error_header, result.ErrorStr()));
    }
}

void InfinityThriftService::ProcessQueryResult(infinity_thrift_rpc::ShowDatabaseResponse &response,
                                               const QueryResult &result,
                                               const std::string_view error_header) {
    response.__set_error_code((i64)(result.ErrorCode()));
    if (!result.IsOk()) {
        response.__set_error_msg(result.ErrorStr());
        LOG_ERROR(fmt::format("{}: {}", error_header, result.ErrorStr()));
    }
}

void InfinityThriftService::ProcessQueryResult(infinity_thrift_rpc::ShowTableResponse &response,
                                               const QueryResult &result,
                                               const std::string_view error_header) {
    response.__set_error_code((i64)(result.ErrorCode()));
    if (!result.IsOk()) {
        response.__set_error_msg(result.ErrorStr());
        LOG_ERROR(fmt::format("{}: {}", error_header, result.ErrorStr()));
    }
}

void InfinityThriftService::ProcessQueryResult(infinity_thrift_rpc::ShowIndexResponse &response,
                                               const QueryResult &result,
                                               const std::string_view error_header) {
    response.__set_error_code((i64)(result.ErrorCode()));
    if (!result.IsOk()) {
        response.__set_error_msg(result.ErrorStr());
        LOG_ERROR(fmt::format("{}: {}", error_header, result.ErrorStr()));
    }
}

void InfinityThriftService::ProcessQueryResult(infinity_thrift_rpc::ShowSegmentResponse &response,
                                               const QueryResult &result,
                                               const std::string_view error_header) {
    response.__set_error_code((i64)(result.ErrorCode()));
    if (!result.IsOk()) {
        response.__set_error_msg(result.ErrorStr());
        LOG_ERROR(fmt::format("{}: {}", error_header, result.ErrorStr()));
    }
}

void InfinityThriftService::ProcessQueryResult(infinity_thrift_rpc::ShowBlockResponse &response,
                                               const QueryResult &result,
                                               const std::string_view error_header) {
    response.__set_error_code((i64)(result.ErrorCode()));
    if (!result.IsOk()) {
        response.__set_error_msg(result.ErrorStr());
        LOG_ERROR(fmt::format("{}: {}", error_header, result.ErrorStr()));
    }
}

void InfinityThriftService::ProcessQueryResult(infinity_thrift_rpc::ShowBlockColumnResponse &response,
                                               const QueryResult &result,
                                               const std::string_view error_header) {
    response.__set_error_code((i64)(result.ErrorCode()));
    if (!result.IsOk()) {
        response.__set_error_msg(result.ErrorStr());
        LOG_ERROR(fmt::format("{}: {}", error_header, result.ErrorStr()));
    }
}

} // namespace infinity
