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

#include <arrow/type_fwd.h>
#include <cstring>
#include <iostream>
#include <ranges>
#include <vector>

module http_server;

import infinity;
import stl;
import status;
import third_party;
import defer_op;
import data_block;
import data_table;
import data_type;
import value;
import infinity_exception;
import logger;
import query_result;
import query_options;
import column_vector;
import infinity_context;
import query_context;
import column_def;
import internal_types;
import parsed_expr;
import constant_expr;
import expr_parser;
import expression_parser_result;
import create_index_info;
import statement_common;
import extra_ddl_info;
import update_statement;
import http_search;
import knn_expr;
import function_expr;
import column_expr;
import type_info;
import logical_type;
import embedding_info;
import sparse_info;
import decimal_info;
import status;
import constant_expr;
import command_statement;
import physical_import;

namespace {

using namespace infinity;

int ParseColumnDefs(const nlohmann::json &fields, Vector<ColumnDef *> &column_definitions, nlohmann::json &json_response) {
    SizeT column_count = fields.size();
    for (SizeT column_id = 0; column_id < column_count; ++column_id) {
        auto &field_element = fields[column_id];

        if (!field_element.contains("name") && !field_element["name"].is_string()) {
            infinity::Status status = infinity::Status::InvalidColumnDefinition("Name field is missing or not string");
            json_response["error_code"] = status.code();
            json_response["error_message"] = status.message();
            return 1;
        }
        String column_name = field_element["name"];

        if (!field_element.contains("type") && !field_element["type"].is_string()) {
            infinity::Status status = infinity::Status::InvalidColumnDefinition("Type field is missing or not string");
            json_response["error_code"] = status.code();
            json_response["error_message"] = status.message();
            return 1;
        }
        String value_type = field_element["type"];
        ToLower(value_type);

        std::vector<std::string> tokens;
        tokens = SplitStrByComma(value_type);

        SharedPtr<DataType> column_type{nullptr};
        SharedPtr<TypeInfo> type_info{nullptr};
        try{
            if (tokens[0] == "vector" || tokens[0] == "multivector" || tokens[0] == "tensor" || tokens[0] == "tensorarray") {
                int dimension = std::stoi(tokens[1]);
                String etype = tokens[2];
                EmbeddingDataType e_data_type;
                if (etype == "int" || etype == "integer" || etype == "int32") {
                    e_data_type = EmbeddingDataType::kElemInt32;
                } else if (etype == "uint8") {
                    e_data_type = EmbeddingDataType::kElemUInt8;
                } else if (etype == "int8" || etype == "tinyint") {
                    e_data_type = EmbeddingDataType::kElemInt8;
                } else if (etype == "float" || etype == "float32") {
                    e_data_type = EmbeddingDataType::kElemFloat;
                } else if (etype == "double" || etype == "float64") {
                    e_data_type = EmbeddingDataType::kElemDouble;
                } else if (etype == "float16") {
                    e_data_type = EmbeddingDataType::kElemFloat16;
                } else if (etype == "bfloat16") {
                    e_data_type = EmbeddingDataType::kElemBFloat16;
                } else {
                    infinity::Status status = infinity::Status::InvalidEmbeddingDataType(etype);
                    json_response["error_code"] = status.code();
                    json_response["error_message"] = status.message();
                    return 1;
                }
                type_info = EmbeddingInfo::Make(e_data_type, size_t(dimension));
                LogicalType logical_type_v = LogicalType::kInvalid;
                if (tokens[0] == "vector") {
                    logical_type_v = LogicalType::kEmbedding;
                } else if (tokens[0] == "multivector") {
                    logical_type_v = LogicalType::kMultiVector;
                } else if (tokens[0] == "tensor") {
                    logical_type_v = LogicalType::kTensor;
                } else if (tokens[0] == "tensorarray") {
                    logical_type_v = LogicalType::kTensorArray;
                }
                column_type = std::make_shared<DataType>(logical_type_v, type_info);
            } else if (tokens[0] == "sparse") {
                int dimension = std::stoi(tokens[1]);
                String dtype = tokens[2];
                String itype = tokens[3];
                EmbeddingDataType d_data_type;
                EmbeddingDataType i_data_type;
                if (dtype == "integer" || dtype == "int" || dtype == "int32") {
                    d_data_type = EmbeddingDataType::kElemInt32;
                } else if (dtype == "float" || dtype == "float32") {
                    d_data_type = EmbeddingDataType::kElemFloat;
                } else if (dtype == "double" || dtype == "float64") {
                    d_data_type = EmbeddingDataType::kElemDouble;
                } else {
                    infinity::Status status = infinity::Status::InvalidEmbeddingDataType(dtype);
                    json_response["error_code"] = status.code();
                    json_response["error_message"] = status.message();
                    return 1;
                }

                if (itype == "tinyint" || itype == "int8") {
                    i_data_type = EmbeddingDataType::kElemInt8;
                } else if (itype == "smallint" || itype == "int16") {
                    i_data_type = EmbeddingDataType::kElemInt16;
                } else if (itype == "integer" || itype == "int" || itype == "int32") {
                    i_data_type = EmbeddingDataType::kElemInt32;
                } else if (itype == "bigint" || itype == "int64") {
                    i_data_type = EmbeddingDataType::kElemInt64;
                } else {
                    infinity::Status status = infinity::Status::InvalidEmbeddingDataType(itype);
                    json_response["error_code"] = status.code();
                    json_response["error_message"] = status.message();
                    return 1;
                }
                type_info = SparseInfo::Make(d_data_type, i_data_type, size_t(dimension), SparseStoreType::kSort);
                column_type = std::make_shared<DataType>(LogicalType::kSparse, type_info);
            } else if (tokens[0] == "decimal") {
                type_info = DecimalInfo::Make(field_element["precision"], field_element["scale"]);
                column_type = std::make_shared<DataType>(LogicalType::kDecimal, type_info);
            } else if (tokens[0] == "array") {
                infinity::Status::ParserError("Array isn't implemented here.");
                type_info = nullptr;
            } else if (tokens.size() == 1) {
                column_type = DataType::StringDeserialize(tokens[0]);
            } else {
                infinity::Status status = infinity::Status::ParserError("type syntax error");
                json_response["error_code"] = status.code();
                json_response["error_message"] = status.message();
                return 1;
            }
        } catch(std::exception &e) {
            infinity::Status status = infinity::Status::ParserError("type syntax error");
            json_response["error_code"] = status.code();
            json_response["error_message"] = status.message();
            return 1;
        }

        if (column_type) {
            std::set<ConstraintType> constraints;
            if (field_element.contains("constraints"))
                for (auto &constraint_json : field_element["constraints"]) {
                    String constraint = constraint_json;
                    ToLower(constraint);
                    constraints.insert(StringToConstraintType(constraint));
                }
            SharedPtr<ParsedExpr> default_expr{nullptr};
            if (field_element.contains("default")) {
                switch (column_type->type()) {
                    case LogicalType::kSparse: {
                        default_expr = BuildConstantSparseExprFromJson(field_element["default"],
                                                                        dynamic_cast<const SparseInfo *>(column_type->type_info().get()));
                        break;
                    }
                    default: {
                        default_expr = BuildConstantExprFromJson(field_element["default"]);
                        break;
                    }
                }
            }
            ColumnDef *col_def = new ColumnDef(column_id, column_type, column_name, constraints, default_expr);
            column_definitions.emplace_back(col_def);
        } else {
            infinity::Status status = infinity::Status::NotSupport(fmt::format("{} type is not supported yet.", field_element["type"]));
            json_response["error_code"] = status.code();
            json_response["error_message"] = status.message();
            return 1;
        }
    }
    return 0;
}

class ListDatabaseHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto result = infinity->ListDatabases();
        nlohmann::json json_response;
        HTTPStatus http_status;
        if (result.IsOk()) {
            SizeT block_rows = result.result_table_->DataBlockCount();
            for (SizeT block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                for (int i = 0; i < row_count; ++i) {
                    Value value = data_block->GetValue(0, i);
                    const String &db_name = value.GetVarchar();
                    json_response["databases"].push_back(db_name);
                }
            }
            json_response["error_code"] = 0;
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class CreateDatabaseHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        // get database name
        String database_name = request->getPathVariable("database_name");

        // get create option
        String body_info = request->readBodyToString();
        nlohmann::json body_info_json = nlohmann::json::parse(body_info);
        String option = body_info_json["create_option"];

        HTTPStatus http_status;
        nlohmann::json json_response;

        CreateDatabaseOptions options;
        if (body_info_json.contains("create_option")) {
            auto create_option = body_info_json["create_option"];
            if (create_option.is_string()) {
                String option = create_option;
                if (option == "ignore_if_exists") {
                    options.conflict_type_ = ConflictType::kIgnore;
                } else if (option == "error") {
                    options.conflict_type_ = ConflictType::kError;
                } else if (option == "replace_if_exists") {
                    options.conflict_type_ = ConflictType::kReplace;
                } else {
                    json_response["error_code"] = 3074;
                    json_response["error_message"] = fmt::format("Invalid create option: {}", option);
                    http_status = HTTPStatus::CODE_500;
                    return ResponseFactory::createResponse(http_status, json_response.dump());
                }
            } else {
                json_response["error_code"] = 3067;
                json_response["error_message"] = "'CREATE OPTION' field value should be string type";
                http_status = HTTPStatus::CODE_500;
                return ResponseFactory::createResponse(http_status, json_response.dump());
            }
        }

        // create database
        auto result = infinity->CreateDatabase(database_name, options);

        if (result.IsOk()) {
            json_response["error_code"] = 0;
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class DropDatabaseHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        // get database name
        String database_name = request->getPathVariable("database_name");

        // get drop option
        HTTPStatus http_status;
        nlohmann::json json_response;

        String body_info = request->readBodyToString();
        nlohmann::json body_info_json = nlohmann::json::parse(body_info);
        String option = body_info_json["drop_option"];
        DropDatabaseOptions options;
        if (body_info_json.contains("drop_option")) {
            auto drop_option = body_info_json["drop_option"];
            if (drop_option.is_string()) {
                String option = drop_option;
                if (option == "ignore_if_not_exists") {
                    options.conflict_type_ = ConflictType::kIgnore;
                } else if (option == "error") {
                    options.conflict_type_ = ConflictType::kError;
                } else {
                    json_response["error_code"] = 3075;
                    json_response["error_message"] = fmt::format("Invalid drop option: {}", option);
                    http_status = HTTPStatus::CODE_500;
                    return ResponseFactory::createResponse(http_status, json_response.dump());
                }
            } else {
                json_response["error_code"] = 3067;
                json_response["error_message"] = "'DROP OPTION' field value should be string type";
                http_status = HTTPStatus::CODE_500;
                return ResponseFactory::createResponse(http_status, json_response.dump());
            }
        }

        auto result = infinity->DropDatabase(database_name, options);

        if (result.IsOk()) {
            json_response["error_code"] = 0;
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class ShowDatabaseHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto database_name = request->getPathVariable("database_name");
        auto result = infinity->ShowDatabase(database_name);

        nlohmann::json json_response;
        nlohmann::json json_res;
        HTTPStatus http_status;

        if (result.IsOk()) {
            SizeT block_rows = result.result_table_->DataBlockCount();
            for (SizeT block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                auto column_cnt = result.result_table_->ColumnCount();

                for (int row = 0; row < row_count; ++row) {
                    nlohmann::json json_database;
                    for (SizeT col = 0; col < column_cnt; ++col) {
                        Value value = data_block->GetValue(col, row);
                        const String &column_name = result.result_table_->GetColumnNameById(col);
                        const String &column_value = value.ToString();
                        json_database[column_name] = column_value;
                    }
                    json_res["res"].push_back(json_database);
                }
                for (auto &element : json_res["res"]) {
                    json_response[element["name"]] = element["value"];
                }
            }

            json_response["error_code"] = 0;
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class CreateTableHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        String database_name = request->getPathVariable("database_name");
        String table_name = request->getPathVariable("table_name");

        String body_info = request->readBodyToString();
        nlohmann::json body_info_json = nlohmann::json::parse(body_info);

        const auto &fields = body_info_json["fields"];
        auto properties = body_info_json["properties"];

        nlohmann::json json_response;
        json_response["error_code"] = 0;
        HTTPStatus http_status;
        http_status = HTTPStatus::CODE_200;

        if (!fields.is_array()) {
            infinity::Status status = infinity::Status::InvalidColumnDefinition("Expect json array in column definitions");
            json_response["error_code"] = status.code();
            json_response["error_message"] = status.message();
            HTTPStatus http_status;
            http_status = HTTPStatus::CODE_500;
            return ResponseFactory::createResponse(http_status, json_response.dump());
        }

        Vector<ColumnDef *> column_definitions;
        Vector<TableConstraint *> table_constraint;
        DeferFn defer_fn_column_def([&]() {
            for (auto &column_def : column_definitions) {
                delete column_def;
                column_def = nullptr;
            }
            for (auto &constraint : table_constraint) {
                delete constraint;
                constraint = nullptr;
            }
        });
        if (int res = ParseColumnDefs(fields, column_definitions, json_response); res) {
            HTTPStatus http_status;
            http_status = HTTPStatus::CODE_500;
            return ResponseFactory::createResponse(http_status, json_response.dump());
        }

        CreateTableOptions options;
        if (body_info_json.contains("create_option")) {
            auto create_option = body_info_json["create_option"];
            if (create_option.is_string()) {
                String option = create_option;
                if (option == "ignore_if_exists") {
                    options.conflict_type_ = ConflictType::kIgnore;
                } else if (option == "error") {
                    options.conflict_type_ = ConflictType::kError;
                } else if (option == "replace_if_exists") {
                    options.conflict_type_ = ConflictType::kReplace;
                } else {
                    json_response["error_code"] = 3074;
                    json_response["error_message"] = fmt::format("Invalid create option: {}", option);
                    http_status = HTTPStatus::CODE_500;
                }
            } else {
                json_response["error_code"] = 3067;
                json_response["error_message"] = "'CREATE OPTION' field value should be string type";
                http_status = HTTPStatus::CODE_500;
            }
        }

        if(json_response["error_code"] == 0) {
            auto result = infinity->CreateTable(database_name, table_name, column_definitions, table_constraint, options);
            if (result.IsOk()) {
                json_response["error_code"] = 0;
                http_status = HTTPStatus::CODE_200;
            } else {
                json_response["error_code"] = result.ErrorCode();
                json_response["error_message"] = result.ErrorMsg();
                http_status = HTTPStatus::CODE_500;
            }
            column_definitions.clear();
            table_constraint.clear();
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class DropTableHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        String database_name = request->getPathVariable("database_name");
        String table_name = request->getPathVariable("table_name");
        String body_info = request->readBodyToString();

        nlohmann::json body_info_json = nlohmann::json::parse(body_info);

        HTTPStatus http_status;
        http_status = HTTPStatus::CODE_200;
        nlohmann::json json_response;
        json_response["error_code"] = 0;

        DropTableOptions options;
        if (body_info_json.contains("drop_option")) {
            auto drop_option = body_info_json["drop_option"];
            if (drop_option.is_string()) {
                String option = drop_option;
                if (option == "ignore_if_not_exists") {
                    options.conflict_type_ = ConflictType::kIgnore;
                } else if (option == "error") {
                    options.conflict_type_ = ConflictType::kError;
                } else {
                    json_response["error_code"] = 3075;
                    json_response["error_message"] = fmt::format("Invalid drop option: {}", option);
                    http_status = HTTPStatus::CODE_500;
                }
            } else {
                json_response["error_code"] = 3067;
                json_response["error_message"] = "'DROP OPTION' field value should be string type";
                http_status = HTTPStatus::CODE_500;
            }
        }

        if (json_response["error_code"] == 0) {
            auto result = infinity->DropTable(database_name, table_name, options);
            if (result.IsOk()) {
                json_response["error_code"] = 0;
                http_status = HTTPStatus::CODE_200;
            } else {
                json_response["error_code"] = result.ErrorCode();
                json_response["error_message"] = result.ErrorMsg();
                http_status = HTTPStatus::CODE_500;
            }
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class ListTableHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        String database_name = request->getPathVariable("database_name");
        auto result = infinity->ShowTables(database_name);
        nlohmann::json json_response;
        HTTPStatus http_status;
        if (result.IsOk()) {
            SizeT block_rows = result.result_table_->DataBlockCount();
            for (SizeT block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                for (int row = 0; row < row_count; ++row) {
                    // Column 1: table name
                    Value value = data_block->GetValue(1, row);
                    const String &column_value = value.ToString();
                    json_response["tables"].push_back(column_value);
                }
            }

            json_response["error_code"] = 0;
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class ShowTableHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        String database_name = request->getPathVariable("database_name");
        String table_name = request->getPathVariable("table_name");

        auto result = infinity->ShowTable(database_name, table_name);
        nlohmann::json json_response;
        nlohmann::json json_res;
        HTTPStatus http_status;
        if (result.IsOk()) {
            SizeT block_rows = result.result_table_->DataBlockCount();
            for (SizeT block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                auto column_cnt = result.result_table_->ColumnCount();
                for (int row = 0; row < row_count; ++row) {
                    nlohmann::json json_table;
                    for (SizeT col = 0; col < column_cnt; ++col) {
                        const String &column_name = result.result_table_->GetColumnNameById(col);
                        Value value = data_block->GetValue(col, row);
                        const String &column_value = value.ToString();
                        json_table[column_name] = column_value;
                    }
                    json_res["tables"].push_back(json_table);
                }
                for (auto &element : json_res["tables"]) {
                    json_response[element["name"]] = element["value"];
                }
            }
            json_response["error_code"] = 0;
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class ExportTableHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        String database_name = request->getPathVariable("database_name");
        String table_name = request->getPathVariable("table_name");

        nlohmann::json json_response;
        HTTPStatus http_status = HTTPStatus::CODE_500;

        String data_body = request->readBodyToString();
        try {
            nlohmann::json http_body_json = nlohmann::json::parse(data_body);
            ExportOptions export_options;

            String file_type_str = http_body_json["file_type"];
            ToLower(file_type_str);
            if (file_type_str == "csv") {
                export_options.copy_file_type_ = CopyFileType::kCSV;
            } else if (file_type_str == "jsonl") {
                export_options.copy_file_type_ = CopyFileType::kJSONL;
            } else if (file_type_str == "fvecs"){
                export_options.copy_file_type_ = CopyFileType::kFVECS;
            }
            else {
                json_response["error_code"] = ErrorCode::kNotSupported;
                json_response["error_message"] = fmt::format("Not supported file type {}", file_type_str);
                return ResponseFactory::createResponse(http_status, json_response.dump());
            }

            if (json_response["error_code"] != ErrorCode::kNotSupported) {
                if (http_body_json.contains("header")) {
                    export_options.header_ = http_body_json["header"];
                }
                if (http_body_json.contains("offset")) {
                    export_options.offset_ = http_body_json["offset"];
                }
                if (http_body_json.contains("limit")) {
                    export_options.limit_ = http_body_json["limit"];
                }
                if (http_body_json.contains("row_limit")) {
                    export_options.row_limit_ = http_body_json["row_limit"];
                }
                if (http_body_json.contains("delimiter")) {
                    String delimiter = http_body_json["delimiter"];
                    if (delimiter.size() != 1) {
                        json_response["error_code"] = ErrorCode::kNotSupported;
                        json_response["error_message"] = fmt::format("Not supported delimiter: {}", delimiter);
                        return ResponseFactory::createResponse(http_status, json_response.dump());
                    }
                    export_options.delimiter_ = delimiter[0];
                } else {
                    export_options.delimiter_ = ',';
                }
            }
            String file_path = http_body_json["file_path"];
            Vector<ParsedExpr *> *export_columns{nullptr};
            DeferFn defer_fn([&]() {
                if (export_columns != nullptr) {
                    for (auto &column_expr : *export_columns) {
                        delete column_expr;
                        column_expr = nullptr;
                    }
                    delete export_columns;
                    export_columns = nullptr;
                }
            });

            if (http_body_json.contains("columns")) {
                export_columns = new Vector<ParsedExpr *>();

                for(const auto& column: http_body_json["columns"]) {
                    if(column.is_string()) {
                        String column_name = column;
                        ToLower(column_name);
                        if(column_name == "_row_id") {
                            FunctionExpr* expr = new FunctionExpr();
                            expr->func_name_ = "row_id";
                            export_columns->emplace_back(expr);
                        } else if(column_name == "_create_timestamp") {
                            FunctionExpr* expr = new FunctionExpr();
                            expr->func_name_ = "create_timestamp";
                            export_columns->emplace_back(expr);
                        } else if(column_name == "_delete_timestamp") {
                            FunctionExpr* expr = new FunctionExpr();
                            expr->func_name_ = "delete_timestamp";
                            export_columns->emplace_back(expr);
                        } else {
                            ColumnExpr* expr = new ColumnExpr();
                            expr->names_.emplace_back(column_name);
                            export_columns->emplace_back(expr);
                        }
                    } else {
                        json_response["error_code"] = ErrorCode::kInvalidJsonFormat;
                        json_response["error_message"] = "Export data isn't a column";
                    }
                }
            }

            auto result = infinity->Export(database_name, table_name, export_columns, file_path, export_options);

            export_columns = nullptr;
            if (result.IsOk()) {
                json_response["error_code"] = 0;
                http_status = HTTPStatus::CODE_200;
            } else {
                json_response["error_code"] = result.ErrorCode();
                json_response["error_message"] = result.ErrorMsg();
                http_status = HTTPStatus::CODE_500;
            }
        } catch (nlohmann::json::exception &e) {
            json_response["error_code"] = ErrorCode::kInvalidJsonFormat;
            json_response["error_message"] = e.what();
        }

        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class ShowTableColumnsHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        String database_name = request->getPathVariable("database_name");
        String table_name = request->getPathVariable("table_name");

        auto result = infinity->ShowColumns(database_name, table_name);
        nlohmann::json json_response;
        HTTPStatus http_status;
        if (result.IsOk()) {
            SizeT block_rows = result.result_table_->DataBlockCount();
            for (SizeT block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                auto column_cnt = result.result_table_->ColumnCount();
                for (int row = 0; row < row_count; ++row) {
                    nlohmann::json json_table;
                    for (SizeT col = 0; col < column_cnt; ++col) {
                        const String &column_name = result.result_table_->GetColumnNameById(col);
                        Value value = data_block->GetValue(col, row);
                        const String &column_value = value.ToString();
                        json_table[column_name] = column_value;
                    }
                    json_response["columns"].push_back(json_table);
                }
            }
            json_response["error_code"] = 0;
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class ImportHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        String database_name = request->getPathVariable("database_name");
        String table_name = request->getPathVariable("table_name");

        nlohmann::json json_response;
        HTTPStatus http_status = HTTPStatus::CODE_500;

        String data_body = request->readBodyToString();
        try {
            nlohmann::json http_body_json = nlohmann::json::parse(data_body);
            ImportOptions import_options;

            String file_type_str = http_body_json["file_type"];
            ToLower(file_type_str);
            if (file_type_str == "csv") {
                import_options.copy_file_type_ = CopyFileType::kCSV;
            } else if (file_type_str == "json") {
                import_options.copy_file_type_ = CopyFileType::kJSON;
            } else if (file_type_str == "jsonl") {
                import_options.copy_file_type_ = CopyFileType::kJSONL;
            } else if (file_type_str == "fvecs") {
                import_options.copy_file_type_ = CopyFileType::kFVECS;
            } else {
                json_response["error_code"] = ErrorCode::kNotSupported;
                json_response["error_message"] = fmt::format("Not supported file type {}", file_type_str);
                return ResponseFactory::createResponse(http_status, json_response.dump());
            }

            if (import_options.copy_file_type_ == CopyFileType::kCSV) {
                if (http_body_json.contains("header")) {
                    import_options.header_ = http_body_json["header"];
                }
                if (http_body_json.contains("delimiter")) {
                    String delimiter = http_body_json["delimiter"];
                    if (delimiter.size() != 1) {
                        json_response["error_code"] = ErrorCode::kNotSupported;
                        json_response["error_message"] = fmt::format("Not supported delimiter: {}", delimiter);
                        return ResponseFactory::createResponse(http_status, json_response.dump());
                    }
                    import_options.delimiter_ = delimiter[0];
                } else {
                    import_options.delimiter_ = ',';
                }
            }
            String file_path = http_body_json["file_path"];

            auto result = infinity->Import(database_name, table_name, file_path, import_options);
            if (result.IsOk()) {
                json_response["error_code"] = 0;
                http_status = HTTPStatus::CODE_200;
            } else {
                json_response["error_code"] = result.ErrorCode();
                json_response["error_message"] = result.ErrorMsg();
                http_status = HTTPStatus::CODE_500;
            }
        } catch (nlohmann::json::exception &e) {
            json_response["error_code"] = ErrorCode::kInvalidJsonFormat;
            json_response["error_message"] = e.what();
        }

        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class InsertHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        nlohmann::json json_response;
        HTTPStatus http_status = HTTPStatus::CODE_500;

        String data_body = request->readBodyToString();
        try {
            nlohmann::json http_body_json = nlohmann::json::parse(data_body);

            SizeT row_count = http_body_json.size();
            if (http_body_json.is_array() && row_count > 0) {

                // First row
                const auto &first_row_json = http_body_json[0];
                SizeT column_count = first_row_json.size();

                // Used for the column type validation
                HashMap<String, u64> column_name_id_map;

                // inserted columns
                Vector<Vector<ParsedExpr *> *> *column_values = new Vector<Vector<infinity::ParsedExpr *> *>();
                column_values->reserve(row_count);
                DeferFn defer_free_column_values([&]() {
                    if (column_values != nullptr) {
                        for (auto &value_array : *column_values) {
                            for (auto &value_ptr : *value_array) {
                                delete value_ptr;
                                value_ptr = nullptr;
                            }
                            delete value_array;
                            value_array = nullptr;
                        }
                        delete column_values;
                        column_values = nullptr;
                    }
                });

                Vector<String> *columns = new Vector<String>();
                column_name_id_map.reserve(column_count);
                columns->reserve(column_count);
                DeferFn defer_free_columns([&]() {
                    if (columns != nullptr) {
                        delete columns;
                        columns = nullptr;
                    }
                });

                // First row
                {
                    Vector<ParsedExpr *> *values_row = new Vector<infinity::ParsedExpr *>();
                    DeferFn defer_free_value_row([&]() {
                        if (values_row != nullptr) {
                            for (auto &value_ptr : *values_row) {
                                delete value_ptr;
                                value_ptr = nullptr;
                            }
                            delete values_row;
                            values_row = nullptr;
                        }
                    });

                    for (const auto &item : first_row_json.items()) {
                        const auto &key = item.key();
                        if (column_name_id_map.contains(key)) {
                            json_response["error_code"] = ErrorCode::kDuplicateColumnName;
                            json_response["error_message"] = fmt::format("Duplicated column name: {}", key);
                            return ResponseFactory::createResponse(http_status, json_response.dump());
                        }
                        column_name_id_map.emplace(key, columns->size());
                        columns->emplace_back(key);

                        const auto &value = item.value();
                        switch (value.type()) {
                            case nlohmann::json::value_t::boolean: {
                                auto bool_value = value.template get<bool>();

                                // Generate constant expression
                                infinity::ConstantExpr *const_expr = new ConstantExpr(LiteralType::kBoolean);
                                const_expr->bool_value_ = bool_value;
                                values_row->emplace_back(const_expr);
                                const_expr = nullptr;
                                break;
                            }
                            case nlohmann::json::value_t::number_integer: {
                                auto integer_value = value.template get<i64>();

                                // Generate constant expression
                                infinity::ConstantExpr *const_expr = new ConstantExpr(LiteralType::kInteger);
                                const_expr->integer_value_ = integer_value;
                                values_row->emplace_back(const_expr);
                                const_expr = nullptr;
                                break;
                            }
                            case nlohmann::json::value_t::number_unsigned: {
                                auto integer_value = value.template get<u64>();

                                // Generate constant expression
                                infinity::ConstantExpr *const_expr = new ConstantExpr(LiteralType::kInteger);
                                const_expr->integer_value_ = integer_value;
                                values_row->emplace_back(const_expr);
                                const_expr = nullptr;
                                break;
                            }
                            case nlohmann::json::value_t::number_float: {
                                auto float_value = value.template get<f64>();

                                // Generate constant expression
                                infinity::ConstantExpr *const_expr = new ConstantExpr(LiteralType::kDouble);
                                const_expr->double_value_ = float_value;
                                values_row->emplace_back(const_expr);
                                const_expr = nullptr;
                                break;
                            }
                            case nlohmann::json::value_t::array: {
                                SizeT dimension = value.size();
                                if (dimension == 0) {
                                    json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                    json_response["error_message"] = fmt::format("Empty embedding data: {}", value);
                                    return ResponseFactory::createResponse(http_status, json_response.dump());
                                }

                                auto first_elem = value[0];
                                auto first_elem_type = first_elem.type();
                                if (first_elem_type == nlohmann::json::value_t::number_integer or
                                    first_elem_type == nlohmann::json::value_t::number_unsigned) {

                                    // Generate constant expression
                                    infinity::ConstantExpr *const_expr = new ConstantExpr(LiteralType::kIntegerArray);
                                    DeferFn defer_free_integer_array([&]() {
                                        if (const_expr != nullptr) {
                                            delete const_expr;
                                            const_expr = nullptr;
                                        }
                                    });


                                    for (SizeT idx = 0; idx < dimension; ++idx) {
                                        const auto &value_ref = value[idx];
                                        const auto &value_type = value_ref.type();

                                        switch (value_type) {
                                            case nlohmann::json::value_t::number_integer: {
                                                const_expr->long_array_.emplace_back(value_ref.template get<i64>());
                                                break;
                                            }
                                            case nlohmann::json::value_t::number_unsigned: {
                                                const_expr->long_array_.emplace_back(value_ref.template get<u64>());
                                                break;
                                            }
                                            default: {
                                                json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                                json_response["error_message"] = fmt::format("Embedding element type should be integer");
                                                return ResponseFactory::createResponse(http_status, json_response.dump());
                                            }
                                        }
                                    }

                                    values_row->emplace_back(const_expr);
                                    const_expr = nullptr;

                                } else if (first_elem_type == nlohmann::json::value_t::number_float) {

                                    // Generate constant expression
                                    infinity::ConstantExpr *const_expr = new ConstantExpr(LiteralType::kDoubleArray);
                                    DeferFn defer_free_double_array([&]() {
                                        if (const_expr != nullptr) {
                                            delete const_expr;
                                            const_expr = nullptr;
                                        }
                                    });


                                    for (SizeT idx = 0; idx < dimension; ++idx) {
                                        const auto &value_ref = value[idx];
                                        const auto &value_type = value_ref.type();
                                        if (value_type != nlohmann::json::value_t::number_float) {
                                            json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                            json_response["error_message"] = fmt::format("Embedding element type should be float");
                                            return ResponseFactory::createResponse(http_status, json_response.dump());
                                        }

                                        const_expr->double_array_.emplace_back(value_ref.template get<double>());
                                    }
                                    values_row->emplace_back(const_expr);
                                    const_expr = nullptr;

                                } else if(first_elem_type == nlohmann::json::value_t::array) {
                                    //std::cout<<"tensor"<<std::endl;
                                    auto first_elem_first_elem = first_elem[0];
                                    auto first_elem_first_elem_type = first_elem_first_elem.type();
                                    if(first_elem_first_elem_type == nlohmann::json::value_t::number_integer or
                                    first_elem_first_elem_type == nlohmann::json::value_t::number_unsigned) {
                                        infinity::ConstantExpr *const_expr = new ConstantExpr(LiteralType::kSubArrayArray);
                                        DeferFn defer_free_tensor_array([&]() {
                                            if (const_expr != nullptr) {
                                                delete const_expr;
                                                const_expr = nullptr;
                                            }
                                        });
                                        const_expr->sub_array_array_.reserve(dimension);
                                        for (SizeT idx = 0; idx < dimension; ++idx) {
                                            const auto &array = value[idx];
                                            auto subdimension = array.size();
                                            if (subdimension == 0) {
                                                json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                                json_response["error_message"] = fmt::format("Empty tensor array: {}", array);
                                                return ResponseFactory::createResponse(http_status, json_response.dump());
                                            }
                                            infinity::ConstantExpr *const_expr_2 = new ConstantExpr(LiteralType::kIntegerArray);
                                            DeferFn defer_free_tensor_sub_array([&]() {
                                                if (const_expr_2 != nullptr) {
                                                    delete const_expr_2;
                                                    const_expr_2 = nullptr;
                                                }
                                            });
                                            const_expr_2->long_array_.reserve(subdimension);
                                            for(SizeT subidx = 0; subidx < subdimension; ++subidx) {
                                                const auto &value_ref = array[subidx];
                                                const auto &value_type = value_ref.type();
                                                switch (value_type) {
                                                    case nlohmann::json::value_t::number_integer: {
                                                        const_expr_2->long_array_.emplace_back(value_ref.template get<i64>());
                                                        break;
                                                    }
                                                    case nlohmann::json::value_t::number_unsigned: {
                                                        const_expr_2->long_array_.emplace_back(value_ref.template get<u64>());
                                                        break;
                                                    }
                                                    default: {
                                                        json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                                        json_response["error_message"] = fmt::format("Tensor Embedding element type should be integer");
                                                        return ResponseFactory::createResponse(http_status, json_response.dump());
                                                    }
                                                }
                                            }
                                            const_expr->sub_array_array_.emplace_back(std::move(const_expr_2));
                                            const_expr_2 = nullptr;
                                        }
                                        values_row->emplace_back(const_expr);
                                        const_expr = nullptr;
                                    } else if(first_elem_first_elem_type == nlohmann::json::value_t::number_float) {
                                        infinity::ConstantExpr *const_expr = new ConstantExpr(LiteralType::kSubArrayArray);
                                        DeferFn defer_free_tensor_array([&]() {
                                            if (const_expr != nullptr) {
                                                delete const_expr;
                                                const_expr = nullptr;
                                            }
                                        });
                                        const_expr->sub_array_array_.reserve(dimension);
                                        for (SizeT idx = 0; idx < dimension; ++idx) {
                                            const auto &array = value[idx];
                                            auto subdimension = array.size();
                                            if (subdimension == 0) {
                                                json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                                json_response["error_message"] = fmt::format("Empty tensor array: {}", array);
                                                return ResponseFactory::createResponse(http_status, json_response.dump());
                                            }
                                            infinity::ConstantExpr *const_expr_2 = new ConstantExpr(LiteralType::kDoubleArray);
                                            DeferFn defer_free_tensor_sub_array([&]() {
                                                if (const_expr_2 != nullptr) {
                                                    delete const_expr_2;
                                                    const_expr_2 = nullptr;
                                                }
                                            });
                                            const_expr_2->double_array_.reserve(subdimension);
                                            for(SizeT subidx = 0; subidx < subdimension; ++subidx) {
                                                const auto &value_ref = array[subidx];
                                                const auto &value_type = value_ref.type();
                                                switch (value_type) {
                                                    case nlohmann::json::value_t::number_float: {
                                                        const_expr_2->double_array_.emplace_back(value_ref.template get<double>());
                                                        break;
                                                    }
                                                    default: {
                                                        json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                                        json_response["error_message"] = fmt::format("Tensor Embedding element type should be float");
                                                        return ResponseFactory::createResponse(http_status, json_response.dump());
                                                    }
                                                }
                                            }
                                            const_expr->sub_array_array_.emplace_back(std::move(const_expr_2));
                                            const_expr_2 = nullptr;
                                        }
                                        values_row->emplace_back(const_expr);
                                        const_expr = nullptr;
                                    }  else if(first_elem_first_elem_type == nlohmann::json::value_t::array) {
                                        //std::cout<<"tensorarray"<<std::endl;
                                        auto first_elem_first_elem_first_elem = first_elem_first_elem[0];
                                        auto first_elem_first_elem_first_elem_type = first_elem_first_elem_first_elem.type();
                                        if(first_elem_first_elem_first_elem_type == nlohmann::json::value_t::number_integer or
                                        first_elem_first_elem_first_elem_type == nlohmann::json::value_t::number_unsigned) {
                                            infinity::ConstantExpr *const_expr = new ConstantExpr(LiteralType::kSubArrayArray);
                                            DeferFn defer_free_tensor_array([&]() {
                                                if (const_expr != nullptr) {
                                                    delete const_expr;
                                                    const_expr = nullptr;
                                                }
                                            });
                                            const_expr->sub_array_array_.reserve(dimension);
                                            for (SizeT idx = 0; idx < dimension; ++idx) {
                                                const auto &array = value[idx];
                                                auto subdimension = array.size();
                                                if (subdimension == 0) {
                                                    json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                                    json_response["error_message"] = fmt::format("Empty tensor array: {}", array);
                                                    return ResponseFactory::createResponse(http_status, json_response.dump());
                                                }
                                                infinity::ConstantExpr *const_expr_2 = new ConstantExpr(LiteralType::kSubArrayArray);
                                                DeferFn defer_free_tensor_sub_array([&]() {
                                                    if (const_expr_2 != nullptr) {
                                                        delete const_expr_2;
                                                        const_expr_2 = nullptr;
                                                    }
                                                });
                                                const_expr_2->sub_array_array_.reserve(subdimension);
                                                for(SizeT subidx = 0; subidx < subdimension; ++subidx) {
                                                    const auto &subarray = array[subidx];
                                                    auto subsubdimension = subarray.size();
                                                    if (subsubdimension == 0) {
                                                        json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                                        json_response["error_message"] = fmt::format("Empty tensor array: {}", array);
                                                        return ResponseFactory::createResponse(http_status, json_response.dump());
                                                    }
                                                    infinity::ConstantExpr *const_expr_3 = new ConstantExpr(LiteralType::kIntegerArray);
                                                    DeferFn defer_free_tensor_sub_sub_array([&]() {
                                                        if (const_expr_3 != nullptr) {
                                                            delete const_expr_3;
                                                            const_expr_3 = nullptr;
                                                        }
                                                    });
                                                    const_expr_3->long_array_.reserve(subsubdimension);
                                                    for(SizeT subsubidx = 0; subsubidx < subsubdimension; ++subsubidx) {
                                                        const auto &value_ref = subarray[subsubidx];
                                                        const auto &value_type = value_ref.type();
                                                        switch (value_type) {
                                                            case nlohmann::json::value_t::number_integer: {
                                                                const_expr_3->long_array_.emplace_back(value_ref.template get<i64>());
                                                                break;
                                                            }
                                                            case nlohmann::json::value_t::number_unsigned: {
                                                                const_expr_3->long_array_.emplace_back(value_ref.template get<u64>());
                                                                break;
                                                            }
                                                            default: {
                                                                json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                                                json_response["error_message"] = fmt::format("Tensor Embedding element type should be integer");
                                                                return ResponseFactory::createResponse(http_status, json_response.dump());
                                                            }
                                                        }
                                                    }
                                                    const_expr_2->sub_array_array_.emplace_back(std::move(const_expr_3));
                                                    const_expr_3 = nullptr;
                                                }
                                                const_expr->sub_array_array_.emplace_back(std::move(const_expr_2));
                                                const_expr_2 = nullptr;
                                            }
                                            values_row->emplace_back(const_expr);
                                            const_expr = nullptr;
                                        } else if(first_elem_first_elem_first_elem_type == nlohmann::json::value_t::number_float) {
                                            infinity::ConstantExpr *const_expr = new ConstantExpr(LiteralType::kSubArrayArray);
                                            DeferFn defer_free_tensor_array([&]() {
                                                if (const_expr != nullptr) {
                                                    delete const_expr;
                                                    const_expr = nullptr;
                                                }
                                            });
                                            const_expr->sub_array_array_.reserve(dimension);
                                            for (SizeT idx = 0; idx < dimension; ++idx) {
                                                const auto &array = value[idx];
                                                auto subdimension = array.size();
                                                if (subdimension == 0) {
                                                    json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                                    json_response["error_message"] = fmt::format("Empty tensor array: {}", array);
                                                    return ResponseFactory::createResponse(http_status, json_response.dump());
                                                }
                                                infinity::ConstantExpr *const_expr_2 = new ConstantExpr(LiteralType::kSubArrayArray);
                                                DeferFn defer_free_tensor_sub_array([&]() {
                                                    if (const_expr_2 != nullptr) {
                                                        delete const_expr_2;
                                                        const_expr_2 = nullptr;
                                                    }
                                                });
                                                const_expr_2->sub_array_array_.reserve(subdimension);
                                                for(SizeT subidx = 0; subidx < subdimension; ++subidx) {
                                                    const auto &subarray = array[subidx];
                                                    auto subsubdimension = subarray.size();
                                                    if (subsubdimension == 0) {
                                                        json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                                        json_response["error_message"] = fmt::format("Empty tensor array: {}", array);
                                                        return ResponseFactory::createResponse(http_status, json_response.dump());
                                                    }
                                                    infinity::ConstantExpr *const_expr_3 = new ConstantExpr(LiteralType::kDoubleArray);
                                                    DeferFn defer_free_tensor_sub_sub_array([&]() {
                                                        if (const_expr_3 != nullptr) {
                                                            delete const_expr_3;
                                                            const_expr_3 = nullptr;
                                                        }
                                                    });
                                                    const_expr_3->double_array_.reserve(subsubdimension);
                                                    for(SizeT subsubidx = 0; subsubidx < subsubdimension; ++subsubidx) {
                                                        const auto &value_ref = subarray[subsubidx];
                                                        const auto &value_type = value_ref.type();
                                                        switch (value_type) {
                                                            case nlohmann::json::value_t::number_float: {
                                                                const_expr_3->double_array_.emplace_back(value_ref.template get<double>());
                                                                break;
                                                            }
                                                            default: {
                                                                json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                                                json_response["error_message"] = fmt::format("Tensor Embedding element type should be float");
                                                                return ResponseFactory::createResponse(http_status, json_response.dump());
                                                            }
                                                        }
                                                    }
                                                    const_expr_2->sub_array_array_.emplace_back(std::move(const_expr_3));
                                                    const_expr_3 = nullptr;
                                                }
                                                const_expr->sub_array_array_.emplace_back(std::move(const_expr_2));
                                                const_expr_2 = nullptr;
                                            }
                                            values_row->emplace_back(const_expr);
                                            const_expr = nullptr;
                                        } else {
                                            json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                            json_response["error_message"] = fmt::format("Tensorarray element type error");
                                            return ResponseFactory::createResponse(http_status, json_response.dump());
                                        }
                                    } else {
                                        json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                        json_response["error_message"] = fmt::format("Tensor element type error");
                                        return ResponseFactory::createResponse(http_status, json_response.dump());
                                    }
                                } else {
                                    json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                    json_response["error_message"] = fmt::format("Embedding element type can only be integer or float or tensor or tensor array");
                                    return ResponseFactory::createResponse(http_status, json_response.dump());
                                }

                                break;
                            }
                            case nlohmann::json::value_t::string: {
                                auto string_value = value.template get<String>();

                                ConstantExpr *const_expr = new ConstantExpr(LiteralType::kString);
                                const_expr->str_value_ = strdup(string_value.c_str());
                                values_row->emplace_back(const_expr);
                                break;
                            }
                            case nlohmann::json::value_t::object: {
                                ConstantExpr *const_sparse_expr = nullptr;
                                DeferFn defer_free_sparse([&]() {
                                    if (const_sparse_expr != nullptr) {
                                        delete const_sparse_expr;
                                        const_sparse_expr = nullptr;
                                    }
                                });
                                if (value.size() == 0) {
                                    json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                    json_response["error_message"] = fmt::format("Empty sparse vector, cannot decide type");
                                    return ResponseFactory::createResponse(http_status, json_response.dump());
                                }
                                switch (value.begin().value().type()) {
                                    case nlohmann::json::value_t::number_unsigned:
                                    case nlohmann::json::value_t::number_integer: {
                                        const_sparse_expr = new ConstantExpr(LiteralType::kLongSparseArray);
                                        break;
                                    }
                                    case nlohmann::json::value_t::number_float: {
                                        const_sparse_expr = new ConstantExpr(LiteralType::kDoubleSparseArray);
                                        break;
                                    }
                                    default: {
                                        json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                        json_response["error_message"] = fmt::format("Sparse value element type error");
                                        return ResponseFactory::createResponse(http_status, json_response.dump());
                                    }
                                }
                                HashSet<i64> key_set;
                                for (const auto &sparse_it : value.items()) {
                                    const auto &sparse_k = sparse_it.key();
                                    const auto &sparse_v = sparse_it.value();
                                    i64 key_val = std::stoll(sparse_k);
                                    if (const auto [_, insert_ok] = key_set.insert(key_val); !insert_ok) {
                                        json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                        json_response["error_message"] = fmt::format("Duplicate key {} in sparse array!", key);
                                        return ResponseFactory::createResponse(http_status, json_response.dump());
                                    }
                                    bool good_v = false;
                                    switch (sparse_v.type()) {
                                        case nlohmann::json::value_t::number_unsigned:
                                        case nlohmann::json::value_t::number_integer: {
                                            if (const_sparse_expr->literal_type_ == LiteralType::kLongSparseArray) {
                                                const_sparse_expr->long_sparse_array_.first.push_back(key_val);
                                                const_sparse_expr->long_sparse_array_.second.push_back(sparse_v.get<i64>());
                                                good_v = true;
                                            }
                                            break;
                                        }
                                        case nlohmann::json::value_t::number_float: {
                                            if (const_sparse_expr->literal_type_ == LiteralType::kDoubleSparseArray) {
                                                const_sparse_expr->double_sparse_array_.first.push_back(key_val);
                                                const_sparse_expr->double_sparse_array_.second.push_back(sparse_v.get<double>());
                                                good_v = true;
                                            }
                                            break;
                                        }
                                        default: {
                                            break;
                                        }
                                    }
                                    if (!good_v) {
                                        json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                        json_response["error_message"] = fmt::format("Sparse value element type error");
                                        return ResponseFactory::createResponse(http_status, json_response.dump());
                                    }
                                }
                                values_row->emplace_back(const_sparse_expr);
                                const_sparse_expr = nullptr;
                                break;
                            }
                            case nlohmann::json::value_t::binary:
                            case nlohmann::json::value_t::null:
                            case nlohmann::json::value_t::discarded: {
                                json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                json_response["error_message"] = fmt::format("Embedding element type can only be integer or float");
                                return ResponseFactory::createResponse(http_status, json_response.dump());
                            }
                        }
                        //                    std::cout << key << " " << value.is_string() << std::endl;
                    }
                    column_values->emplace_back(values_row);
                    values_row = nullptr;
                }

                // Other rows except the first
                for (SizeT row_id = 1; row_id < row_count; ++row_id) {
                    const auto &row_json = http_body_json[row_id];

                    Vector<ParsedExpr *> *values_row = new Vector<infinity::ParsedExpr *>();
                    DeferFn defer_free_value_row([&]() {
                        if (values_row != nullptr) {
                            for (auto &value_ptr : *values_row) {
                                delete value_ptr;
                                value_ptr = nullptr;
                            }
                            delete values_row;
                            values_row = nullptr;
                        }
                    });
                    values_row->resize(column_count);

                    for (const auto &item : row_json.items()) {
                        const auto &key = item.key();
                        auto id_iter = column_name_id_map.find(key);
                        if (id_iter == column_name_id_map.end()) {
                            json_response["error_code"] = ErrorCode::kColumnNotExist;
                            json_response["error_message"] = fmt::format("Not existed column name: {}", key);
                            return ResponseFactory::createResponse(http_status, json_response.dump());
                        }

                        u64 column_id = id_iter->second;
                        const auto &value = item.value();
                        switch (value.type()) {
                            case nlohmann::json::value_t::boolean: {
                                // Generate constant expression
                                infinity::ConstantExpr *const_expr = new ConstantExpr(LiteralType::kBoolean);
                                const_expr->bool_value_ = value.template get<bool>();
                                (*values_row)[column_id] = const_expr;
                                const_expr = nullptr;
                                break;
                            }
                            case nlohmann::json::value_t::number_integer: {
                                // Generate constant expression
                                infinity::ConstantExpr *const_expr = new ConstantExpr(LiteralType::kInteger);
                                const_expr->integer_value_ = value.template get<i64>();
                                (*values_row)[column_id] = const_expr;
                                const_expr = nullptr;
                                break;
                            }
                            case nlohmann::json::value_t::number_unsigned: {
                                // Generate constant expression
                                infinity::ConstantExpr *const_expr = new ConstantExpr(LiteralType::kInteger);
                                const_expr->integer_value_ = value.template get<u64>();
                                (*values_row)[column_id] = const_expr;
                                const_expr = nullptr;
                                break;
                            }
                            case nlohmann::json::value_t::number_float: {
                                // Generate constant expression
                                infinity::ConstantExpr *const_expr = new ConstantExpr(LiteralType::kDouble);
                                const_expr->double_value_ = value.template get<f64>();
                                (*values_row)[column_id] = const_expr;
                                const_expr = nullptr;
                                break;
                            }
                            case nlohmann::json::value_t::array: {
                                SizeT dimension = value.size();
                                if (dimension == 0) {
                                    json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                    json_response["error_message"] = fmt::format("Empty embedding data: {}", value);
                                    return ResponseFactory::createResponse(http_status, json_response.dump());
                                }

                                auto first_elem = value[0];
                                auto first_elem_type = first_elem.type();
                                if (first_elem_type == nlohmann::json::value_t::number_integer or
                                    first_elem_type == nlohmann::json::value_t::number_unsigned) {

                                    // Generate constant expression
                                    infinity::ConstantExpr *const_expr = new ConstantExpr(LiteralType::kIntegerArray);
                                    DeferFn defer_free_integer_array([&]() {
                                        if (const_expr != nullptr) {
                                            delete const_expr;
                                            const_expr = nullptr;
                                        }
                                    });

                                    for (SizeT idx = 0; idx < dimension; ++idx) {
                                        const auto &value_ref = value[idx];
                                        const auto &value_type = value_ref.type();

                                        switch (value_type) {
                                            case nlohmann::json::value_t::number_integer: {
                                                const_expr->long_array_.emplace_back(value_ref.template get<i64>());
                                                break;
                                            }
                                            case nlohmann::json::value_t::number_unsigned: {
                                                const_expr->long_array_.emplace_back(value_ref.template get<u64>());
                                                break;
                                            }
                                            default: {
                                                json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                                json_response["error_message"] = fmt::format("Embedding element type should be integer");
                                                return ResponseFactory::createResponse(http_status, json_response.dump());
                                            }
                                        }
                                    }

                                    (*values_row)[column_id] = const_expr;
                                    const_expr = nullptr;
                                } else if (first_elem_type == nlohmann::json::value_t::number_float) {

                                    // Generate constant expression
                                    infinity::ConstantExpr *const_expr = new ConstantExpr(LiteralType::kDoubleArray);
                                    DeferFn defer_free_double_array([&]() {
                                        if (const_expr != nullptr) {
                                            delete const_expr;
                                            const_expr = nullptr;
                                        }
                                    });

                                    for (SizeT idx = 0; idx < dimension; ++idx) {
                                        const auto &value_ref = value[idx];
                                        const auto &value_type = value_ref.type();
                                        if (value_type != nlohmann::json::value_t::number_float) {
                                            json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                            json_response["error_message"] = fmt::format("Embedding element type should be float");
                                            return ResponseFactory::createResponse(http_status, json_response.dump());
                                        }

                                        const_expr->double_array_.emplace_back(value_ref.template get<double>());
                                    }

                                    (*values_row)[column_id] = const_expr;
                                    const_expr = nullptr;
                                } else if(first_elem_type == nlohmann::json::value_t::array) {
                                    //std::cout<<"tensor"<<std::endl;
                                    auto first_elem_first_elem = first_elem[0];
                                    auto first_elem_first_elem_type = first_elem_first_elem.type();
                                    if(first_elem_first_elem_type == nlohmann::json::value_t::number_integer or
                                    first_elem_first_elem_type == nlohmann::json::value_t::number_unsigned) {
                                        infinity::ConstantExpr *const_expr = new ConstantExpr(LiteralType::kSubArrayArray);
                                        DeferFn defer_free_tensor_array([&]() {
                                            if (const_expr != nullptr) {
                                                delete const_expr;
                                                const_expr = nullptr;
                                            }
                                        });
                                        const_expr->sub_array_array_.reserve(dimension);
                                        for (SizeT idx = 0; idx < dimension; ++idx) {
                                            const auto &array = value[idx];
                                            auto subdimension = array.size();
                                            if (subdimension == 0) {
                                                json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                                json_response["error_message"] = fmt::format("Empty tensor array: {}", array);
                                                return ResponseFactory::createResponse(http_status, json_response.dump());
                                            }
                                            infinity::ConstantExpr *const_expr_2 = new ConstantExpr(LiteralType::kIntegerArray);
                                            DeferFn defer_free_tensor_sub_array([&]() {
                                                if (const_expr_2 != nullptr) {
                                                    delete const_expr_2;
                                                    const_expr_2 = nullptr;
                                                }
                                            });
                                            const_expr_2->long_array_.reserve(subdimension);
                                            for(SizeT subidx = 0; subidx < subdimension; ++subidx) {
                                                const auto &value_ref = array[subidx];
                                                const auto &value_type = value_ref.type();
                                                switch (value_type) {
                                                    case nlohmann::json::value_t::number_integer: {
                                                        const_expr_2->long_array_.emplace_back(value_ref.template get<i64>());
                                                        break;
                                                    }
                                                    case nlohmann::json::value_t::number_unsigned: {
                                                        const_expr_2->long_array_.emplace_back(value_ref.template get<u64>());
                                                        break;
                                                    }
                                                    default: {
                                                        json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                                        json_response["error_message"] = fmt::format("Tensor Embedding element type should be integer");
                                                        return ResponseFactory::createResponse(http_status, json_response.dump());
                                                    }
                                                }
                                            }
                                            const_expr->sub_array_array_.emplace_back(std::move(const_expr_2));
                                            const_expr_2 = nullptr;
                                        }
                                        (*values_row)[column_id] = const_expr;
                                        const_expr = nullptr;
                                    } else if(first_elem_first_elem_type == nlohmann::json::value_t::number_float) {
                                        infinity::ConstantExpr *const_expr = new ConstantExpr(LiteralType::kSubArrayArray);
                                        DeferFn defer_free_tensor_array([&]() {
                                            if (const_expr != nullptr) {
                                                delete const_expr;
                                                const_expr = nullptr;
                                            }
                                        });
                                        const_expr->sub_array_array_.reserve(dimension);
                                        for (SizeT idx = 0; idx < dimension; ++idx) {
                                            const auto &array = value[idx];
                                            auto subdimension = array.size();
                                            if (subdimension == 0) {
                                                json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                                json_response["error_message"] = fmt::format("Empty tensor array: {}", array);
                                                return ResponseFactory::createResponse(http_status, json_response.dump());
                                            }
                                            infinity::ConstantExpr *const_expr_2 = new ConstantExpr(LiteralType::kDoubleArray);
                                            DeferFn defer_free_tensor_sub_array([&]() {
                                                if (const_expr_2 != nullptr) {
                                                    delete const_expr_2;
                                                    const_expr_2 = nullptr;
                                                }
                                            });
                                            const_expr_2->double_array_.reserve(subdimension);
                                            for(SizeT subidx = 0; subidx < subdimension; ++subidx) {
                                                const auto &value_ref = array[subidx];
                                                const auto &value_type = value_ref.type();
                                                switch (value_type) {
                                                    case nlohmann::json::value_t::number_float: {
                                                        const_expr_2->double_array_.emplace_back(value_ref.template get<double>());
                                                        break;
                                                    }
                                                    default: {
                                                        json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                                        json_response["error_message"] = fmt::format("Tensor Embedding element type should be float");
                                                        return ResponseFactory::createResponse(http_status, json_response.dump());
                                                    }
                                                }
                                            }
                                            const_expr->sub_array_array_.emplace_back(std::move(const_expr_2));
                                            const_expr_2 = nullptr;
                                        }
                                        (*values_row)[column_id] = const_expr;
                                        const_expr = nullptr;
                                    }  else if(first_elem_first_elem_type == nlohmann::json::value_t::array) {
                                        //std::cout<<"tensorarray"<<std::endl;
                                        auto first_elem_first_elem_first_elem = first_elem_first_elem[0];
                                        auto first_elem_first_elem_first_elem_type = first_elem_first_elem_first_elem.type();
                                        if(first_elem_first_elem_first_elem_type == nlohmann::json::value_t::number_integer or
                                        first_elem_first_elem_first_elem_type == nlohmann::json::value_t::number_unsigned) {
                                            infinity::ConstantExpr *const_expr = new ConstantExpr(LiteralType::kSubArrayArray);
                                            DeferFn defer_free_tensor_array([&]() {
                                                if (const_expr != nullptr) {
                                                    delete const_expr;
                                                    const_expr = nullptr;
                                                }
                                            });
                                            const_expr->sub_array_array_.reserve(dimension);
                                            for (SizeT idx = 0; idx < dimension; ++idx) {
                                                const auto &array = value[idx];
                                                auto subdimension = array.size();
                                                if (subdimension == 0) {
                                                    json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                                    json_response["error_message"] = fmt::format("Empty tensor array: {}", array);
                                                    return ResponseFactory::createResponse(http_status, json_response.dump());
                                                }
                                                infinity::ConstantExpr *const_expr_2 = new ConstantExpr(LiteralType::kSubArrayArray);
                                                DeferFn defer_free_tensor_sub_array([&]() {
                                                    if (const_expr_2 != nullptr) {
                                                        delete const_expr_2;
                                                        const_expr_2 = nullptr;
                                                    }
                                                });
                                                const_expr_2->sub_array_array_.reserve(subdimension);
                                                for(SizeT subidx = 0; subidx < subdimension; ++subidx) {
                                                    const auto &subarray = array[subidx];
                                                    auto subsubdimension = subarray.size();
                                                    if (subsubdimension == 0) {
                                                        json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                                        json_response["error_message"] = fmt::format("Empty tensor array: {}", array);
                                                        return ResponseFactory::createResponse(http_status, json_response.dump());
                                                    }
                                                    infinity::ConstantExpr *const_expr_3 = new ConstantExpr(LiteralType::kIntegerArray);
                                                    DeferFn defer_free_tensor_sub_sub_array([&]() {
                                                        if (const_expr_3 != nullptr) {
                                                            delete const_expr_3;
                                                            const_expr_3 = nullptr;
                                                        }
                                                    });
                                                    const_expr_3->long_array_.reserve(subsubdimension);
                                                    for(SizeT subsubidx = 0; subsubidx < subsubdimension; ++subsubidx) {
                                                        const auto &value_ref = subarray[subsubidx];
                                                        const auto &value_type = value_ref.type();
                                                        switch (value_type) {
                                                            case nlohmann::json::value_t::number_integer: {
                                                                const_expr_3->long_array_.emplace_back(value_ref.template get<i64>());
                                                                break;
                                                            }
                                                            case nlohmann::json::value_t::number_unsigned: {
                                                                const_expr_3->long_array_.emplace_back(value_ref.template get<u64>());
                                                                break;
                                                            }
                                                            default: {
                                                                json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                                                json_response["error_message"] = fmt::format("Tensor Embedding element type should be integer");
                                                                return ResponseFactory::createResponse(http_status, json_response.dump());
                                                            }
                                                        }
                                                    }
                                                    const_expr_2->sub_array_array_.emplace_back(std::move(const_expr_3));
                                                    const_expr_3 = nullptr;
                                                }
                                                const_expr->sub_array_array_.emplace_back(std::move(const_expr_2));
                                                const_expr_2 = nullptr;
                                            }
                                            (*values_row)[column_id] = const_expr;
                                            const_expr = nullptr;
                                        } else if(first_elem_first_elem_first_elem_type == nlohmann::json::value_t::number_float) {
                                            infinity::ConstantExpr *const_expr = new ConstantExpr(LiteralType::kSubArrayArray);
                                            DeferFn defer_free_tensor_array([&]() {
                                                if (const_expr != nullptr) {
                                                    delete const_expr;
                                                    const_expr = nullptr;
                                                }
                                            });
                                            const_expr->sub_array_array_.reserve(dimension);
                                            for (SizeT idx = 0; idx < dimension; ++idx) {
                                                const auto &array = value[idx];
                                                auto subdimension = array.size();
                                                if (subdimension == 0) {
                                                    json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                                    json_response["error_message"] = fmt::format("Empty tensor array: {}", array);
                                                    return ResponseFactory::createResponse(http_status, json_response.dump());
                                                }
                                                infinity::ConstantExpr *const_expr_2 = new ConstantExpr(LiteralType::kSubArrayArray);
                                                DeferFn defer_free_tensor_sub_array([&]() {
                                                    if (const_expr_2 != nullptr) {
                                                        delete const_expr_2;
                                                        const_expr_2 = nullptr;
                                                    }
                                                });
                                                const_expr_2->sub_array_array_.reserve(subdimension);
                                                for(SizeT subidx = 0; subidx < subdimension; ++subidx) {
                                                    const auto &subarray = array[subidx];
                                                    auto subsubdimension = subarray.size();
                                                    if (subsubdimension == 0) {
                                                        json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                                        json_response["error_message"] = fmt::format("Empty tensor array: {}", array);
                                                        return ResponseFactory::createResponse(http_status, json_response.dump());
                                                    }
                                                    infinity::ConstantExpr *const_expr_3 = new ConstantExpr(LiteralType::kDoubleArray);
                                                    DeferFn defer_free_tensor_sub_sub_array([&]() {
                                                        if (const_expr_3 != nullptr) {
                                                            delete const_expr_3;
                                                            const_expr_3 = nullptr;
                                                        }
                                                    });
                                                    const_expr_3->double_array_.reserve(subsubdimension);
                                                    for(SizeT subsubidx = 0; subsubidx < subsubdimension; ++subsubidx) {
                                                        const auto &value_ref = subarray[subsubidx];
                                                        const auto &value_type = value_ref.type();
                                                        switch (value_type) {
                                                            case nlohmann::json::value_t::number_float: {
                                                                const_expr_3->double_array_.emplace_back(value_ref.template get<double>());
                                                                break;
                                                            }
                                                            default: {
                                                                json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                                                json_response["error_message"] = fmt::format("Tensor Embedding element type should be float");
                                                                return ResponseFactory::createResponse(http_status, json_response.dump());
                                                            }
                                                        }
                                                    }
                                                    const_expr_2->sub_array_array_.emplace_back(std::move(const_expr_3));
                                                    const_expr_3 = nullptr;
                                                }
                                                const_expr->sub_array_array_.emplace_back(std::move(const_expr_2));
                                                const_expr_2 = nullptr;
                                            }
                                            (*values_row)[column_id] = const_expr;
                                            const_expr = nullptr;
                                        } else {
                                            json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                            json_response["error_message"] = fmt::format("Tensorarray element type error");
                                            return ResponseFactory::createResponse(http_status, json_response.dump());
                                        }
                                    } else {
                                        json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                        json_response["error_message"] = fmt::format("Tensor element type error");
                                        return ResponseFactory::createResponse(http_status, json_response.dump());
                                    }
                                } else {
                                    json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                    json_response["error_message"] = fmt::format("Embedding element type can only be integer or float");
                                    return ResponseFactory::createResponse(http_status, json_response.dump());
                                }

                                break;
                            }
                            case nlohmann::json::value_t::string: {

                                auto string_value = value.template get<String>();

                                ConstantExpr *const_expr = new ConstantExpr(LiteralType::kString);
                                const_expr->str_value_ = strdup(string_value.c_str());
                                (*values_row)[column_id] = const_expr;

                                break;
                            }
                            case nlohmann::json::value_t::object: {
                                ConstantExpr *const_sparse_expr = nullptr;
                                DeferFn defer_free_sparse([&]() {
                                    if (const_sparse_expr != nullptr) {
                                        delete const_sparse_expr;
                                        const_sparse_expr = nullptr;
                                    }
                                });
                                if (value.size() == 0) {
                                    json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                    json_response["error_message"] = fmt::format("Empty sparse vector, cannot decide type");
                                    return ResponseFactory::createResponse(http_status, json_response.dump());
                                }
                                switch (value.begin().value().type()) {
                                    case nlohmann::json::value_t::number_unsigned:
                                    case nlohmann::json::value_t::number_integer: {
                                        const_sparse_expr = new ConstantExpr(LiteralType::kLongSparseArray);
                                        break;
                                    }
                                    case nlohmann::json::value_t::number_float: {
                                        const_sparse_expr = new ConstantExpr(LiteralType::kDoubleSparseArray);
                                        break;
                                    }
                                    default: {
                                        json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                        json_response["error_message"] = fmt::format("Sparse value element type error");
                                        return ResponseFactory::createResponse(http_status, json_response.dump());
                                    }
                                }
                                HashSet<i64> key_set;
                                for (const auto &sparse_it : value.items()) {
                                    const auto &sparse_k = sparse_it.key();
                                    const auto &sparse_v = sparse_it.value();
                                    i64 key_val = std::stoll(sparse_k);
                                    if (const auto [_, insert_ok] = key_set.insert(key_val); !insert_ok) {
                                        json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                        json_response["error_message"] = fmt::format("Duplicate key {} in sparse array!", key);
                                        return ResponseFactory::createResponse(http_status, json_response.dump());
                                    }
                                    bool good_v = false;
                                    switch (sparse_v.type()) {
                                        case nlohmann::json::value_t::number_unsigned:
                                        case nlohmann::json::value_t::number_integer: {
                                            if (const_sparse_expr->literal_type_ == LiteralType::kLongSparseArray) {
                                                const_sparse_expr->long_sparse_array_.first.push_back(key_val);
                                                const_sparse_expr->long_sparse_array_.second.push_back(sparse_v.get<i64>());
                                                good_v = true;
                                            }
                                            break;
                                        }
                                        case nlohmann::json::value_t::number_float: {
                                            if (const_sparse_expr->literal_type_ == LiteralType::kDoubleSparseArray) {
                                                const_sparse_expr->double_sparse_array_.first.push_back(key_val);
                                                const_sparse_expr->double_sparse_array_.second.push_back(sparse_v.get<double>());
                                                good_v = true;
                                            }
                                            break;
                                        }
                                        default: {
                                            break;
                                        }
                                    }
                                    if (!good_v) {
                                        json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                        json_response["error_message"] = fmt::format("Sparse value element type error");
                                        return ResponseFactory::createResponse(http_status, json_response.dump());
                                    }
                                }
                                (*values_row)[column_id] = const_sparse_expr;
                                const_sparse_expr = nullptr;
                                break;
                            }
                            case nlohmann::json::value_t::binary:
                            case nlohmann::json::value_t::null:
                            case nlohmann::json::value_t::discarded: {
                                json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                json_response["error_message"] = fmt::format("Embedding element type can only be integer or float");
                                return ResponseFactory::createResponse(http_status, json_response.dump());
                            }
                        }
                    }
                    column_values->emplace_back(values_row);
                    values_row = nullptr;
                }

                auto database_name = request->getPathVariable("database_name");
                auto table_name = request->getPathVariable("table_name");
                auto result = infinity->Insert(database_name, table_name, columns, column_values);
                columns = nullptr;
                column_values = nullptr;
                if (result.IsOk()) {
                    json_response["error_code"] = 0;
                    http_status = HTTPStatus::CODE_200;
                } else {
                    json_response["error_code"] = result.ErrorCode();
                    json_response["error_message"] = result.ErrorMsg();
                    http_status = HTTPStatus::CODE_500;
                }

            } else {
                json_response["error_code"] = ErrorCode::kInvalidJsonFormat;
                json_response["error_message"] = fmt::format("Invalid json format: {}", data_body);
            }

        } catch (nlohmann::json::exception &e) {
            json_response["error_code"] = ErrorCode::kInvalidJsonFormat;
            json_response["error_message"] = e.what();
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class DeleteHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        nlohmann::json json_response;
        HTTPStatus http_status = HTTPStatus::CODE_500;

        String data_body = request->readBodyToString();
        try {
            nlohmann::json http_body_json = nlohmann::json::parse(data_body);

            const String filter_string = http_body_json["filter"];
            if(filter_string != "") {
                UniquePtr<ExpressionParserResult> expr_parsed_result = MakeUnique<ExpressionParserResult>();
                ExprParser expr_parser;
                expr_parser.Parse(filter_string, expr_parsed_result.get());
                if (expr_parsed_result->IsError() || expr_parsed_result->exprs_ptr_->size() != 1) {
                    json_response["error_code"] = ErrorCode::kInvalidFilterExpression;
                    json_response["error_message"] = fmt::format("Invalid filter expression: {}", filter_string);
                    return ResponseFactory::createResponse(http_status, json_response.dump());
                }

                auto database_name = request->getPathVariable("database_name");
                auto table_name = request->getPathVariable("table_name");
                const QueryResult result = infinity->Delete(database_name, table_name, expr_parsed_result->exprs_ptr_->at(0));
                expr_parsed_result->exprs_ptr_->at(0) = nullptr;

                if (result.IsOk()) {
                    json_response["error_code"] = 0;
                    http_status = HTTPStatus::CODE_200;

                    // Only one block
                    DataBlock *data_block = result.result_table_->GetDataBlockById(0).get();
                    // Get sum delete rows
                    Value value = data_block->GetValue(1, 0);
                    json_response["delete_row_count"] = value.value_.big_int;
                } else {
                    json_response["error_code"] = result.ErrorCode();
                    json_response["error_message"] = result.ErrorMsg();
                    http_status = HTTPStatus::CODE_500;
                }
            }
            else {
                auto database_name = request->getPathVariable("database_name");
                auto table_name = request->getPathVariable("table_name");
                const QueryResult result = infinity->Delete(database_name, table_name, nullptr);

                if (result.IsOk()) {
                    json_response["error_code"] = 0;
                    http_status = HTTPStatus::CODE_200;

                    // Only one block
                    DataBlock *data_block = result.result_table_->GetDataBlockById(0).get();
                    // Get sum delete rows
                    Value value = data_block->GetValue(1, 0);
                    json_response["delete_row_count"] = value.value_.big_int;
                } else {
                    json_response["error_code"] = result.ErrorCode();
                    json_response["error_message"] = result.ErrorMsg();
                    http_status = HTTPStatus::CODE_500;
                }
            }
        } catch (nlohmann::json::exception &e) {
            json_response["error_code"] = ErrorCode::kInvalidJsonFormat;
            json_response["error_message"] = e.what();
        }

        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class UpdateHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        nlohmann::json json_response;
        HTTPStatus http_status = HTTPStatus::CODE_500;

        String data_body = request->readBodyToString();
        try {
            nlohmann::json http_body_json = nlohmann::json::parse(data_body);
            const auto &update_clause = http_body_json["update"];

            Vector<UpdateExpr *> *update_expr_array = new Vector<UpdateExpr *>();
            DeferFn defer_free_update_expr_array([&]() {
                if (update_expr_array != nullptr) {
                    for (auto &expr : *update_expr_array) {
                        delete expr;
                    }
                    delete update_expr_array;
                    update_expr_array = nullptr;
                }
            });
            update_expr_array->reserve(update_clause.size());

            for (const auto &update_elem : update_clause.items()) {
                UpdateExpr *update_expr = new UpdateExpr();
                DeferFn defer_free_update_expr([&]() {
                    if (update_expr != nullptr) {
                        delete update_expr;
                        update_expr = nullptr;
                    }
                });
                update_expr->column_name = update_elem.key();
                const auto &value = update_elem.value();
                switch (value.type()) {
                    case nlohmann::json::value_t::boolean: {
                        // Generate constant expression
                        infinity::ConstantExpr *const_expr = new ConstantExpr(LiteralType::kBoolean);
                        const_expr->bool_value_ = value.template get<bool>();
                        update_expr->value = const_expr;
                        const_expr = nullptr;
                        break;
                    }
                    case nlohmann::json::value_t::number_integer: {
                        // Generate constant expression
                        infinity::ConstantExpr *const_expr = new ConstantExpr(LiteralType::kInteger);
                        const_expr->integer_value_ = value.template get<i64>();
                        update_expr->value = const_expr;
                        const_expr = nullptr;
                        break;
                    }
                    case nlohmann::json::value_t::number_unsigned: {
                        // Generate constant expression
                        infinity::ConstantExpr *const_expr = new ConstantExpr(LiteralType::kInteger);
                        const_expr->integer_value_ = value.template get<u64>();
                        update_expr->value = const_expr;
                        const_expr = nullptr;
                        break;
                    }
                    case nlohmann::json::value_t::number_float: {
                        // Generate constant expression
                        infinity::ConstantExpr *const_expr = new ConstantExpr(LiteralType::kDouble);
                        const_expr->double_value_ = value.template get<f64>();
                        update_expr->value = const_expr;
                        const_expr = nullptr;
                        break;
                    }
                    case nlohmann::json::value_t::string: {
                        infinity::ConstantExpr *const_expr = new ConstantExpr(LiteralType::kString);
                        String str_value = value.template get<String>();
                        const_expr->str_value_ = strdup(str_value.c_str());
                        update_expr->value = const_expr;
                        const_expr = nullptr;
                        break;
                    }
                    case nlohmann::json::value_t::array: {
                        SizeT dimension = value.size();
                        if (dimension == 0) {
                            json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                            json_response["error_message"] = fmt::format("Empty embedding data: {}", value);
                            return ResponseFactory::createResponse(http_status, json_response.dump());
                        }

                        auto first_elem = value[0];
                        auto first_elem_type = first_elem.type();
                        if (first_elem_type == nlohmann::json::value_t::number_integer or
                            first_elem_type == nlohmann::json::value_t::number_unsigned) {

                            // Generate constant expression
                            infinity::ConstantExpr *const_expr = new ConstantExpr(LiteralType::kIntegerArray);
                            DeferFn defer_free_integer_array([&]() {
                                if (const_expr != nullptr) {
                                    delete const_expr;
                                    const_expr = nullptr;
                                }
                            });

                            for (SizeT idx = 0; idx < dimension; ++idx) {
                                const auto &value_ref = value[idx];
                                const auto &value_type = value_ref.type();
                                
                                switch (value_type) {
                                    case nlohmann::json::value_t::number_integer: {
                                        const_expr->long_array_.emplace_back(value_ref.template get<i64>());
                                        break;
                                    }
                                    case nlohmann::json::value_t::number_unsigned: {
                                        const_expr->long_array_.emplace_back(value_ref.template get<u64>());
                                        break;
                                    }
                                    default: {
                                        json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                        json_response["error_message"] = fmt::format("Embedding element type should be integer");
                                        return ResponseFactory::createResponse(http_status, json_response.dump());
                                    }
                                }
                            }

                            update_expr->value = const_expr;
                            const_expr = nullptr;
                        } else if (first_elem_type == nlohmann::json::value_t::number_float) {

                            // Generate constant expression
                            infinity::ConstantExpr *const_expr = new ConstantExpr(LiteralType::kDoubleArray);
                            DeferFn defer_free_double_array([&]() {
                                if (const_expr != nullptr) {
                                    delete const_expr;
                                    const_expr = nullptr;
                                }
                            });

                            for (SizeT idx = 0; idx < dimension; ++idx) {
                                const auto &value_ref = value[idx];
                                const auto &value_type = value_ref.type();
                                if (value_type != nlohmann::json::value_t::number_float) {
                                    json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                    json_response["error_message"] = fmt::format("Embedding element type should be float");
                                    return ResponseFactory::createResponse(http_status, json_response.dump());
                                }

                                const_expr->double_array_.emplace_back(value_ref.template get<double>());
                            }

                            update_expr->value = const_expr;
                            const_expr = nullptr;
                        } else {
                            json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                            json_response["error_message"] = fmt::format("Embedding element type can only be integer or float");
                            return ResponseFactory::createResponse(http_status, json_response.dump());
                        }
                        break;
                    }

                    case nlohmann::json::value_t::object:
                    case nlohmann::json::value_t::binary:
                    case nlohmann::json::value_t::null:
                    case nlohmann::json::value_t::discarded: {
                        json_response["error_code"] = ErrorCode::kInvalidExpression;
                        json_response["error_message"] = fmt::format("Invalid update set expression");
                        return ResponseFactory::createResponse(http_status, json_response.dump());
                    }
                }

                update_expr_array->emplace_back(update_expr);
                update_expr = nullptr;
            }

            String where_clause = http_body_json["filter"];

            UniquePtr<ExpressionParserResult> expr_parsed_result = MakeUnique<ExpressionParserResult>();
            ExprParser expr_parser;
            expr_parser.Parse(where_clause, expr_parsed_result.get());
            if (expr_parsed_result->IsError() || expr_parsed_result->exprs_ptr_->size() != 1) {
                json_response["error_code"] = ErrorCode::kInvalidFilterExpression;
                json_response["error_message"] = fmt::format("Invalid filter expression: {}", where_clause);
                return ResponseFactory::createResponse(http_status, json_response.dump());
            }

            auto database_name = request->getPathVariable("database_name");
            auto table_name = request->getPathVariable("table_name");

            const QueryResult result = infinity->Update(database_name, table_name, expr_parsed_result->exprs_ptr_->at(0), update_expr_array);
            expr_parsed_result->exprs_ptr_->at(0) = nullptr;
            update_expr_array = nullptr;

            if (result.IsOk()) {
                json_response["error_code"] = 0;
                http_status = HTTPStatus::CODE_200;
            } else {
                json_response["error_code"] = result.ErrorCode();
                json_response["error_message"] = result.ErrorMsg();
                http_status = HTTPStatus::CODE_500;
            }

        } catch (nlohmann::json::exception &e) {
            json_response["error_code"] = ErrorCode::kInvalidJsonFormat;
            json_response["error_message"] = e.what();
        }

        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class SelectHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto database_name = request->getPathVariable("database_name");
        auto table_name = request->getPathVariable("table_name");
        String data_body = request->readBodyToString();

        nlohmann::json json_response;
        HTTPStatus http_status;

        HTTPSearch::Process(infinity.get(), database_name, table_name, data_body, http_status, json_response);

        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class ExplainHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto database_name = request->getPathVariable("database_name");
        auto table_name = request->getPathVariable("table_name");
        String data_body = request->readBodyToString();

        nlohmann::json json_response;
        HTTPStatus http_status;

        HTTPSearch::Explain(infinity.get(), database_name, table_name, data_body, http_status, json_response);

        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class ListTableIndexesHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto database_name = request->getPathVariable("database_name");
        auto table_name = request->getPathVariable("table_name");
        auto result = infinity->ListTableIndexes(database_name, table_name);

        HTTPStatus http_status;
        nlohmann::json json_response;

        if (result.IsOk()) {

            SizeT block_rows = result.result_table_->DataBlockCount();
            for (SizeT block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();

                for (int row = 0; row < row_count; ++row) {
                    nlohmann::json json_index;

                    {
                        // index name
                        Value value = data_block->GetValue(0, row);
                        const String &column_value = value.ToString();
                        json_index["index_name"] = column_value;
                    }

                    {
                        // index type
                        Value value = data_block->GetValue(1, row);
                        const String &column_value = value.ToString();
                        json_index["index_type"] = column_value;
                    }

                    {
                        // columns
                        Value value = data_block->GetValue(3, row);
                        const String &column_value = value.ToString();
                        json_index["columns"] = column_value;
                    }

                    json_response["indexes"].push_back(json_index);
                }
            }
            json_response["error_code"] = 0;
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class ShowTableIndexDetailHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto database_name = request->getPathVariable("database_name");
        auto table_name = request->getPathVariable("table_name");
        auto index_name = request->getPathVariable("index_name");

        auto result = infinity->ShowIndex(database_name, table_name, index_name);

        HTTPStatus http_status;
        nlohmann::json json_response;

        if (result.IsOk()) {

            SizeT block_rows = result.result_table_->DataBlockCount();
            for (SizeT block_id = 0; block_id < block_rows; ++block_id) {
                SharedPtr<DataBlock> data_block = result.result_table_->GetDataBlockById(block_id);
                auto row_count = data_block->row_count();
                for (int row = 0; row < row_count; ++row) {
                    auto field_name = data_block->GetValue(0, row).ToString();
                    auto field_value = data_block->GetValue(1, row).ToString();
                    json_response[field_name] = field_value;
                }
            }

            json_response["error_code"] = 0;
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class ShowTableIndexSegmentHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto database_name = request->getPathVariable("database_name");
        auto table_name = request->getPathVariable("table_name");
        auto index_name = request->getPathVariable("index_name");
        auto segment_id = std::strtoll(request->getPathVariable("segment_id").get()->c_str(), nullptr, 0);

        auto result = infinity->ShowIndexSegment(database_name, table_name, index_name, segment_id);

        HTTPStatus http_status;
        nlohmann::json json_response;

        if (result.IsOk()) {

            SizeT block_rows = result.result_table_->DataBlockCount();
            for (SizeT block_id = 0; block_id < block_rows; ++block_id) {
                SharedPtr<DataBlock> data_block = result.result_table_->GetDataBlockById(block_id);
                auto row_count = data_block->row_count();
                for (int row = 0; row < row_count; ++row) {
                    auto field_name = data_block->GetValue(0, row).ToString();
                    auto field_value = data_block->GetValue(1, row).ToString();
                    json_response[field_name] = field_value;
                }
            }

            json_response["error_code"] = 0;
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class ShowTableIndexChunkHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto database_name = request->getPathVariable("database_name");
        auto table_name = request->getPathVariable("table_name");
        auto index_name = request->getPathVariable("index_name");
        auto segment_id = std::strtoll(request->getPathVariable("segment_id").get()->c_str(), nullptr, 0);
        auto chunk_id = std::strtoll(request->getPathVariable("chunk_id").get()->c_str(), nullptr, 0);
        auto result = infinity->ShowIndexChunk(database_name, table_name, index_name, segment_id, chunk_id);

        HTTPStatus http_status;
        nlohmann::json json_response;

        if (result.IsOk()) {

            SizeT block_rows = result.result_table_->DataBlockCount();
            for (SizeT block_id = 0; block_id < block_rows; ++block_id) {
                SharedPtr<DataBlock> data_block = result.result_table_->GetDataBlockById(block_id);
                auto row_count = data_block->row_count();
                for (int row = 0; row < row_count; ++row) {
                    auto field_name = data_block->GetValue(0, row).ToString();
                    auto field_value = data_block->GetValue(1, row).ToString();
                    json_response[field_name] = field_value;
                }
            }

            json_response["error_code"] = 0;
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class DropIndexHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto database_name = request->getPathVariable("database_name");
        auto table_name = request->getPathVariable("table_name");
        auto index_name = request->getPathVariable("index_name");

        String body_info = request->readBodyToString();

        nlohmann::json body_info_json = nlohmann::json::parse(body_info);

        nlohmann::json json_response;
        json_response["error_code"] = 0;
        HTTPStatus http_status;
        http_status = HTTPStatus::CODE_500;
        DropIndexOptions options{ConflictType::kInvalid};
        if (body_info_json.contains("drop_option")) {
            auto drop_option = body_info_json["drop_option"];
            if (drop_option.is_string()) {
                String option = drop_option;
                if (option == "ignore_if_not_exists") {
                    options.conflict_type_ = ConflictType::kIgnore;
                } else if (option == "error") {
                    options.conflict_type_ = ConflictType::kError;
                } else {
                    json_response["error_code"] = 3075;
                    json_response["error_message"] = fmt::format("Invalid drop option: {}", option);
                    http_status = HTTPStatus::CODE_500;
                }
            } else {
                json_response["error_code"] = 3067;
                json_response["error_message"] = "'CREATE OPTION' field value should be string type";
                http_status = HTTPStatus::CODE_500;
            }
        }

        if(json_response["error_code"] == 0) {
            auto result = infinity->DropIndex(database_name, table_name, index_name, options);
            if (result.IsOk()) {
                json_response["error_code"] = 0;
                http_status = HTTPStatus::CODE_200;
            } else {
                json_response["error_code"] = result.ErrorCode();
                json_response["error_message"] = result.ErrorMsg();
                http_status = HTTPStatus::CODE_500;
            }
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class CreateIndexHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto database_name = request->getPathVariable("database_name");
        auto table_name = request->getPathVariable("table_name");
        auto index_name = request->getPathVariable("index_name");

        String body_info_str = request->readBodyToString();
        nlohmann::json body_info_json = nlohmann::json::parse(body_info_str);

        nlohmann::json json_response;
        json_response["error_code"] = 0;
        HTTPStatus http_status;
        http_status = HTTPStatus::CODE_200;

        CreateIndexOptions options;
        if (body_info_json.contains("create_option")) {
            auto create_option = body_info_json["create_option"];
            if (create_option.is_string()) {
                String option = create_option;
                if (option == "ignore_if_exists") {
                    options.conflict_type_ = ConflictType::kIgnore;
                } else if (option == "error") {
                    options.conflict_type_ = ConflictType::kError;
                } else if (option == "replace_if_exists") {
                    options.conflict_type_ = ConflictType::kReplace;
                } else {
                    json_response["error_code"] = 3075;
                    json_response["error_message"] = fmt::format("Invalid create option: {}", option);
                    http_status = HTTPStatus::CODE_500;
                }
            } else {
                json_response["error_code"] = 3067;
                json_response["error_message"] = "'CREATE OPTION' field value should be string type";
                http_status = HTTPStatus::CODE_500;
            }
        }

        auto fields = body_info_json["fields"];
        auto index = body_info_json["index"];

        auto index_info = new IndexInfo();
        DeferFn release_index_info([&]() {
            if(index_info != nullptr) {
                delete index_info;
                index_info = nullptr;
            }
        });
        {
            index_info->column_name_ = fields[0];
            ToLower(index_info->column_name_);
            auto index_param_list = new Vector<InitParameter *>();
            DeferFn release_index_param_list([&]() {
                if(index_param_list != nullptr) {
                    for (auto &index_param_ptr : *index_param_list) {
                        delete index_param_ptr;
                    }
                    delete index_param_list;
                    index_param_list = nullptr;
                }
            });

            for (auto &ele : index.items()) {
                String name = ele.key();
                ToLower(name);
                auto value = ele.value();
                if (!ele.value().is_string()) {
                    value = ele.value().dump();
                }

                if (strcmp(name.c_str(), "type") == 0) {
                    String version_str = value;
                    ToUpper(version_str);
                    index_info->index_type_ = IndexInfo::StringToIndexType(version_str);
                    if (index_info->index_type_ == IndexType::kInvalid) {
                        json_response["error_code"] = ErrorCode::kInvalidIndexType;
                        json_response["error_message"] = fmt::format("Invalid index type: {}", name);
                        http_status = HTTPStatus::CODE_500;
                        return ResponseFactory::createResponse(http_status, json_response.dump());
                    }
                } else {
                    index_param_list->push_back(new InitParameter(name, value));
                }
            }

            index_info->index_param_list_ = index_param_list;
            index_param_list = nullptr;
        }

        if(json_response["error_code"] == 0) {
            auto result = infinity->CreateIndex(database_name, table_name, index_name, index_info, options);
            index_info = nullptr;
            if (result.IsOk()) {
                json_response["error_code"] = 0;
                http_status = HTTPStatus::CODE_200;
            } else {
                json_response["error_code"] = result.ErrorCode();
                json_response["error_message"] = result.ErrorMsg();
                http_status = HTTPStatus::CODE_500;
            }
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class OptimizeIndexHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto database_name = request->getPathVariable("database_name");
        auto table_name = request->getPathVariable("table_name");
        auto index_name = request->getPathVariable("index_name");

        String body_info_str = request->readBodyToString();
        nlohmann::json body_info_json = nlohmann::json::parse(body_info_str);

        nlohmann::json json_response;
        json_response["error_code"] = 0;
        HTTPStatus http_status;
        http_status = HTTPStatus::CODE_200;

        OptimizeOptions optimize_options;
        optimize_options.index_name_ = index_name;
        if (body_info_json.contains("optimize_options")) {
            if(body_info_json["optimize_options"].type() != nlohmann::json::value_t::object) {
                json_response["error_code"] = ErrorCode::kInvalidParameterValue;
                json_response["error_message"] = "Optimize options should be key value pairs!";
                http_status = HTTPStatus::CODE_500;
                return ResponseFactory::createResponse(http_status, json_response.dump());
            }
            for(const auto &option : body_info_json["optimize_options"].items()) {
                const auto &key = option.key();
                const auto &value = option.value();
                auto *init_param = new InitParameter();
                init_param->param_name_ = key;
                init_param->param_value_ = value;
                optimize_options.opt_params_.emplace_back(init_param);
            }
        }

        const QueryResult result = infinity->Optimize(database_name, table_name, std::move(optimize_options));
        if (result.IsOk()) {
            json_response["error_code"] = 0;
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class AddColumnsHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto database_name = request->getPathVariable("database_name");
        auto table_name = request->getPathVariable("table_name");

        String data_body = request->readBodyToString();
        nlohmann::json json_body = nlohmann::json::parse(data_body);

        nlohmann::json json_response;
        json_response["error_code"] = 0;
        HTTPStatus http_status;
        http_status = HTTPStatus::CODE_200;

        Vector<ColumnDef *> column_def_ptrs;
        DeferFn defer_free_column_def_ptrs([&]() {
            for (auto &column_def_ptr : column_def_ptrs) {
                delete column_def_ptr;
            }
        });
        const nlohmann::json &fields = json_body["fields"];
        if (int res = ParseColumnDefs(fields, column_def_ptrs, json_response); res) {
            HTTPStatus http_status;
            http_status = HTTPStatus::CODE_500;
            return ResponseFactory::createResponse(http_status, json_response.dump());
        }
        Vector<SharedPtr<ColumnDef>> column_defs;
        for (auto &column_def_ptr : column_def_ptrs) {
            column_defs.emplace_back(column_def_ptr);
        }
        column_def_ptrs.clear();

        const QueryResult result = infinity->AddColumns(database_name, table_name, std::move(column_defs));
        if (result.IsOk()) {
            json_response["error_code"] = 0;
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class DropColumnsHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto database_name = request->getPathVariable("database_name");
        auto table_name = request->getPathVariable("table_name");

        String data_body = request->readBodyToString();
        nlohmann::json json_body = nlohmann::json::parse(data_body);

        nlohmann::json json_response;
        json_response["error_code"] = 0;
        HTTPStatus http_status;
        http_status = HTTPStatus::CODE_200;

        Vector<String> column_names;
        for (const auto &column : json_body["column_names"]) {
            column_names.emplace_back(column);
        }

        const QueryResult result = infinity->DropColumns(database_name, table_name, std::move(column_names));
        if (result.IsOk()) {
            json_response["error_code"] = 0;
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class ShowSegmentDetailHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto database_name = request->getPathVariable("database_name");
        auto table_name = request->getPathVariable("table_name");
        auto segment_id = std::strtoll(request->getPathVariable("segment_id").get()->c_str(), nullptr, 0);
        auto result = infinity->ShowSegment(database_name, table_name, segment_id);

        HTTPStatus http_status;
        nlohmann::json json_response;

        if (result.IsOk()) {

            SizeT block_rows = result.result_table_->DataBlockCount();
            auto column_cnt = result.result_table_->ColumnCount();
            for (SizeT block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                for (int row = 0; row < row_count; ++row) {
                    for (SizeT col = 0; col < column_cnt; ++col) {
                        const String &column_name = result.result_table_->GetColumnNameById(col);
                        Value value = data_block->GetValue(col, row);
                        const String &column_value = value.ToString();
                        json_response[column_name] = column_value;
                    }
                }
            }
            json_response["error_code"] = 0;
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class ShowSegmentsListHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto database_name = request->getPathVariable("database_name");
        auto table_name = request->getPathVariable("table_name");
        auto result = infinity->ShowSegments(database_name, table_name);

        HTTPStatus http_status;
        nlohmann::json json_response;

        if (result.IsOk()) {

            SizeT block_rows = result.result_table_->DataBlockCount();
            auto column_cnt = result.result_table_->ColumnCount();
            for (SizeT block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                for (int row = 0; row < row_count; ++row) {

                    nlohmann::json json_segment;
                    for (SizeT col = 0; col < column_cnt; ++col) {
                        const String &column_name = result.result_table_->GetColumnNameById(col);
                        Value value = data_block->GetValue(col, row);
                        const String &column_value = value.ToString();
                        json_segment[column_name] = column_value;
                    }
                    json_response["segments"].push_back(json_segment);
                }
            }
            json_response["table_name"] = table_name;
            json_response["error_code"] = 0;
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class ShowBlocksListHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto database_name = request->getPathVariable("database_name");
        auto table_name = request->getPathVariable("table_name");
        auto segment_id = std::strtoll(request->getPathVariable("segment_id").get()->c_str(), nullptr, 0);
        auto result = infinity->ShowBlocks(database_name, table_name, segment_id);

        HTTPStatus http_status;
        nlohmann::json json_response;

        if (result.IsOk()) {

            SizeT block_rows = result.result_table_->DataBlockCount();
            auto column_cnt = result.result_table_->ColumnCount();
            for (SizeT block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                for (int row = 0; row < row_count; ++row) {

                    nlohmann::json json_block;
                    for (SizeT col = 0; col < column_cnt; ++col) {
                        const String &column_name = result.result_table_->GetColumnNameById(col);
                        Value value = data_block->GetValue(col, row);
                        const String &column_value = value.ToString();
                        json_block[column_name] = column_value;
                    }
                    json_response["blocks"].push_back(json_block);
                }
            }

            json_response["segment_id"] = segment_id;
            json_response["error_code"] = 0;
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class ShowBlockDetailHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto database_name = request->getPathVariable("database_name");
        auto table_name = request->getPathVariable("table_name");
        auto segment_id = std::strtoll(request->getPathVariable("segment_id").get()->c_str(), nullptr, 0);
        auto block_id = std::strtoll(request->getPathVariable("block_id").get()->c_str(), nullptr, 0);
        auto result = infinity->ShowBlock(database_name, table_name, segment_id, block_id);

        HTTPStatus http_status;
        nlohmann::json json_response;

        if (result.IsOk()) {
            SizeT block_rows = result.result_table_->DataBlockCount();
            auto column_cnt = result.result_table_->ColumnCount();
            for (SizeT block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                for (int row = 0; row < row_count; ++row) {
                    for (SizeT col = 0; col < column_cnt; ++col) {
                        const String &column_name = result.result_table_->GetColumnNameById(col);
                        Value value = data_block->GetValue(col, row);
                        const String &column_value = value.ToString();
                        json_response[column_name] = column_value;
                    }
                }
            }
            json_response["error_code"] = 0;
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class ShowBlockColumnHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto database_name = request->getPathVariable("database_name");
        auto table_name = request->getPathVariable("table_name");
        auto segment_id = std::strtoll(request->getPathVariable("segment_id").get()->c_str(), nullptr, 0);
        auto block_id = std::strtoll(request->getPathVariable("block_id").get()->c_str(), nullptr, 0);
        auto column_id = std::strtoll(request->getPathVariable("column_id").get()->c_str(), nullptr, 0);
        auto result = infinity->ShowBlockColumn(database_name, table_name, segment_id, block_id, column_id);

        nlohmann::json json_response;
        nlohmann::json json_res;
        HTTPStatus http_status;
        if (result.IsOk()) {
            SizeT block_rows = result.result_table_->DataBlockCount();
            for (SizeT block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                auto column_cnt = result.result_table_->ColumnCount();
                for (int row = 0; row < row_count; ++row) {
                    nlohmann::json json_table;
                    for (SizeT col = 0; col < column_cnt; ++col) {
                        const String &column_name = result.result_table_->GetColumnNameById(col);
                        Value value = data_block->GetValue(col, row);
                        const String &column_value = value.ToString();
                        json_table[column_name] = column_value;
                    }
                    json_res["tables"].push_back(json_table);
                }
                for (auto &element : json_res["tables"]) {
                    json_response[element["name"]] = element["description"];
                }
            }
            json_response["error_code"] = 0;
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};


class ShowConfigsHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto result = infinity->ShowConfigs();

        nlohmann::json json_response;
        HTTPStatus http_status;

        if (result.IsOk()) {
            json_response["error_code"] = 0;
            DataBlock *data_block = result.result_table_->GetDataBlockById(0).get(); // Assume the config output data only included in one data block
            auto row_count = data_block->row_count();
            for (int row = 0; row < row_count; ++row) {
                // config name
                Value name_value = data_block->GetValue(0, row);
                const String& config_name = name_value.ToString();
                // config value
                Value value = data_block->GetValue(1, row);
                const String &config_value = value.ToString();
                json_response[config_name] = config_value;
            }
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class ShowConfigHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto config_name = request->getPathVariable("config_name");
        auto result = infinity->ShowConfig(config_name);

        nlohmann::json json_response;
        HTTPStatus http_status;

        if (result.IsOk()) {
            json_response["error_code"] = 0;
            DataBlock *data_block = result.result_table_->GetDataBlockById(0).get();
            Value value = data_block->GetValue(0, 0);
            const String &variable_value = value.ToString();
            json_response[config_name] = variable_value;
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class ShowGlobalVariablesHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto result = infinity->ShowVariables(SetScope::kGlobal);

        nlohmann::json json_response;
        HTTPStatus http_status;

        if (result.IsOk()) {
            json_response["error_code"] = 0;
            DataBlock *data_block = result.result_table_->GetDataBlockById(0).get(); // Assume the variables output data only included in one data block
            auto row_count = data_block->row_count();
            for (int row = 0; row < row_count; ++row) {
                // variable name
                Value name_value = data_block->GetValue(0, row);
                const String& config_name = name_value.ToString();
                // variable value
                Value value = data_block->GetValue(1, row);
                const String &config_value = value.ToString();
                json_response[config_name] = config_value;
            }
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class ShowGlobalVariableHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto variable_name = request->getPathVariable("variable_name");
        auto result = infinity->ShowVariable(variable_name, SetScope::kGlobal);

        nlohmann::json json_response;
        HTTPStatus http_status;

        if (result.IsOk()) {
            json_response["error_code"] = 0;
            DataBlock *data_block = result.result_table_->GetDataBlockById(0).get();
            Value value = data_block->GetValue(0, 0);
            const String &variable_value = value.ToString();
            json_response[variable_name] = variable_value;

            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class SetGlobalVariableHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        nlohmann::json json_response;
        HTTPStatus http_status;
        QueryResult result;

        String data_body = request->readBodyToString();
        try {

            nlohmann::json http_body_json = nlohmann::json::parse(data_body);
            if(http_body_json.size() != 1) {
                json_response["error_code"] = 3076;
                json_response["error_message"] = "No variable will be set";
                http_status = HTTPStatus::CODE_500;
                return ResponseFactory::createResponse(http_status, json_response.dump());
            }

            for (const auto &set_variable : http_body_json.items()) {
                String var_name = set_variable.key();
                const auto &var_value = set_variable.value();
                switch (var_value.type()) {
                    case nlohmann::json::value_t::boolean: {
                        bool bool_value = var_value.template get<bool>();
                        result = infinity->SetVariableOrConfig(var_name, bool_value, SetScope::kGlobal);
                        break;
                    }
                    case nlohmann::json::value_t::number_integer: {
                        i64 integer_value = var_value.template get<i64>();
                        result = infinity->SetVariableOrConfig(var_name, integer_value, SetScope::kGlobal);
                        break;
                    }
                    case nlohmann::json::value_t::number_unsigned: {
                        i64 integer_value = var_value.template get<u64>();
                        result = infinity->SetVariableOrConfig(var_name, integer_value, SetScope::kGlobal);
                        break;
                    }
                    case nlohmann::json::value_t::number_float: {
                        f64 double_value = var_value.template get<f64>();
                        result = infinity->SetVariableOrConfig(var_name, double_value, SetScope::kGlobal);
                        break;
                    }
                    case nlohmann::json::value_t::string: {
                        String str_value = var_value.template get<std::string>();
                        result = infinity->SetVariableOrConfig(var_name, str_value, SetScope::kGlobal);
                        break;
                    }
                    case nlohmann::json::value_t::array:
                    case nlohmann::json::value_t::object:
                    case nlohmann::json::value_t::binary:
                    case nlohmann::json::value_t::null:
                    case nlohmann::json::value_t::discarded: {
                        json_response["error_code"] = ErrorCode::kInvalidExpression;
                        json_response["error_message"] = fmt::format("Invalid set variable expression");
                        return ResponseFactory::createResponse(http_status, json_response.dump());
                    }
                }
            }
        } catch (nlohmann::json::exception &e) {
            json_response["error_code"] = ErrorCode::kInvalidJsonFormat;
            json_response["error_message"] = e.what();
        }
        if (result.IsOk()) {
            json_response["error_code"] = 0;
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class SetConfigHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        nlohmann::json json_response;
        HTTPStatus http_status;
        QueryResult result;

        String data_body = request->readBodyToString();
        try {

            nlohmann::json http_body_json = nlohmann::json::parse(data_body);
            if(http_body_json.size() != 1) {
                json_response["error_code"] = 3076;
                json_response["error_message"] = "No config will be set";
                http_status = HTTPStatus::CODE_500;
                return ResponseFactory::createResponse(http_status, json_response.dump());
            }

            for (const auto &set_config : http_body_json.items()) {
                String config_name = set_config.key();
                const auto &config_value = set_config.value();
                switch (config_value.type()) {
                    case nlohmann::json::value_t::boolean: {
                        bool bool_value = config_value.template get<bool>();
                        result = infinity->SetVariableOrConfig(config_name, bool_value, SetScope::kConfig);
                        break;
                    }
                    case nlohmann::json::value_t::number_integer: {
                        i64 integer_value = config_value.template get<i64>();
                        result = infinity->SetVariableOrConfig(config_name, integer_value, SetScope::kConfig);
                        break;
                    }
                    case nlohmann::json::value_t::number_unsigned: {
                        i64 integer_value = config_value.template get<u64>();
                        result = infinity->SetVariableOrConfig(config_name, integer_value, SetScope::kConfig);
                        break;
                    }
                    case nlohmann::json::value_t::number_float: {
                        f64 double_value = config_value.template get<f64>();
                        result = infinity->SetVariableOrConfig(config_name, double_value, SetScope::kConfig);
                        break;
                    }
                    case nlohmann::json::value_t::string: {
                        String str_value = config_value.template get<std::string>();
                        result = infinity->SetVariableOrConfig(config_name, str_value, SetScope::kConfig);
                        break;
                    }
                    case nlohmann::json::value_t::array:
                    case nlohmann::json::value_t::object:
                    case nlohmann::json::value_t::binary:
                    case nlohmann::json::value_t::null:
                    case nlohmann::json::value_t::discarded: {
                        json_response["error_code"] = ErrorCode::kInvalidExpression;
                        json_response["error_message"] = fmt::format("Invalid set config expression");
                        return ResponseFactory::createResponse(http_status, json_response.dump());
                    }
                }
            }
        } catch (nlohmann::json::exception &e) {
            json_response["error_code"] = ErrorCode::kInvalidJsonFormat;
            json_response["error_message"] = e.what();
        }
        if (result.IsOk()) {
            json_response["error_code"] = 0;
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};


} // namespace

namespace infinity {

void HTTPServer::Start(const String& ip_address, u16 port) {
    if(started_) {
        return ;
    }

    WebEnvironment::init();

    SharedPtr<HttpRouter> router = HttpRouter::createShared();

    // database
    router->route("GET", "/databases", MakeShared<ListDatabaseHandler>());
    router->route("POST", "/databases/{database_name}", MakeShared<CreateDatabaseHandler>());
    router->route("DELETE", "/databases/{database_name}", MakeShared<DropDatabaseHandler>());
    router->route("GET", "/databases/{database_name}", MakeShared<ShowDatabaseHandler>());

    // table
    router->route("GET", "/databases/{database_name}/tables", MakeShared<ListTableHandler>());
    router->route("POST", "/databases/{database_name}/tables/{table_name}", MakeShared<CreateTableHandler>());
    router->route("DELETE", "/databases/{database_name}/tables/{table_name}", MakeShared<DropTableHandler>());
    router->route("GET", "/databases/{database_name}/tables/{table_name}", MakeShared<ShowTableHandler>());
    router->route("GET", "/databases/{database_name}/table/{table_name}", MakeShared<ExportTableHandler>()); // Export table
    router->route("GET", "/databases/{database_name}/tables/{table_name}/columns", MakeShared<ShowTableColumnsHandler>());

    // DML
    router->route("PUT", "/databases/{database_name}/tables/{table_name}", MakeShared<ImportHandler>());
    router->route("POST", "/databases/{database_name}/tables/{table_name}/docs", MakeShared<InsertHandler>());
    router->route("DELETE", "/databases/{database_name}/tables/{table_name}/docs", MakeShared<DeleteHandler>());
    router->route("PUT", "/databases/{database_name}/tables/{table_name}/docs", MakeShared<UpdateHandler>());

    // DQL
    router->route("GET", "/databases/{database_name}/tables/{table_name}/docs", MakeShared<SelectHandler>());
    router->route("GET", "/databases/{database_name}/tables/{table_name}/meta", MakeShared<ExplainHandler>());

    // index
    router->route("GET", "/databases/{database_name}/tables/{table_name}/indexes", MakeShared<ListTableIndexesHandler>());
    router->route("GET", "/databases/{database_name}/tables/{table_name}/indexes/{index_name}", MakeShared<ShowTableIndexDetailHandler>());
    router->route("GET", "/databases/{database_name}/tables/{table_name}/indexes/{index_name}/segment/{segment_id}", MakeShared<ShowTableIndexSegmentHandler>());
    router->route("GET", "/databases/{database_name}/tables/{table_name}/indexes/{index_name}/segment/{segment_id}/chunk/{chunk_id}", MakeShared<ShowTableIndexChunkHandler>());
    router->route("DELETE", "/databases/{database_name}/tables/{table_name}/indexes/{index_name}", MakeShared<DropIndexHandler>());
    router->route("POST", "/databases/{database_name}/tables/{table_name}/indexes/{index_name}", MakeShared<CreateIndexHandler>());
    router->route("PUT", "/databases/{database_name}/tables/{table_name}/indexes/{index_name}", MakeShared<OptimizeIndexHandler>());

    // alter
    router->route("PUT", "/databases/{database_name}/tables/{table_name}/columns", MakeShared<AddColumnsHandler>());
    router->route("DELETE", "/databases/{database_name}/tables/{table_name}/columns", MakeShared<DropColumnsHandler>());

    // segment
    router->route("GET", "/databases/{database_name}/tables/{table_name}/segments/{segment_id}", MakeShared<ShowSegmentDetailHandler>());
    router->route("GET", "/databases/{database_name}/tables/{table_name}/segments", MakeShared<ShowSegmentsListHandler>());

    // block
    router->route("GET", "/databases/{database_name}/tables/{table_name}/segments/{segment_id}/blocks", MakeShared<ShowBlocksListHandler>());
    router->route("GET",
                  "/databases/{database_name}/tables/{table_name}/segments/{segment_id}/blocks/{block_id}",
                  MakeShared<ShowBlockDetailHandler>());
    router->route("GET",
                  "/databases/{database_name}/tables/{table_name}/segments/{segment_id}/blocks/{block_id}/{column_id}",
                  MakeShared<ShowBlockColumnHandler>());

    // config
    router->route("GET", "/configs", MakeShared<ShowConfigsHandler>());
    router->route("GET", "/configs/{config_name}", MakeShared<ShowConfigHandler>());

    router->route("POST", "/configs", MakeShared<SetConfigHandler>());

    // variable
    router->route("GET", "/variables", MakeShared<ShowGlobalVariablesHandler>());
    router->route("GET", "/variables/{variable_name}", MakeShared<ShowGlobalVariableHandler>());

    router->route("POST", "/variables", MakeShared<SetGlobalVariableHandler>());

    SharedPtr<HttpConnectionProvider> connection_provider = HttpConnectionProvider::createShared({ip_address, port, WebAddress::IP_4});
    SharedPtr<HttpConnectionHandler> connection_handler = HttpConnectionHandler::createShared(router);

    server_ = MakeShared<WebServer>(connection_provider, connection_handler);

    fmt::print("HTTP server listen on {}: {}\n", ip_address, port);

    started_ = true;

    server_->run();
}

void HTTPServer::Shutdown() {
    if(started_) {
        server_->stop();
        WebEnvironment::destroy();
        started_ = false;
    }
}

} // namespace infinity
