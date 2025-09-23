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

module infinity_core:http_server.impl;

import :http_server;
import :infinity;
import :status;
import :defer_op;
import :data_block;
import :data_table;
import :value;
import :infinity_exception;
import :query_result;
import :query_options;
import :column_vector;
import :infinity_context;
import :query_context;
import :http_search;
import :status;
import :physical_import;
import :utility;

import std.compat;
import third_party;

import constant_expr;
import command_statement;
import knn_expr;
import function_expr;
import column_expr;
import type_info;
import logical_type;
import embedding_info;
import sparse_info;
import decimal_info;
import array_info;
import column_def;
import internal_types;
import parsed_expr;
import constant_expr;
import function_expr;
import insert_row_expr;
import expr_parser;
import expression_parser_result;
import create_index_info;
import statement_common;
import extra_ddl_info;
import update_statement;
import data_type;

namespace {

using namespace infinity;

std::pair<std::shared_ptr<DataType>, infinity::Status> ParseColumnType(const std::span<const std::string> tokens, std::string_view json_sv) {
    simdjson::padded_string json_pad(json_sv);
    simdjson::parser parser;
    simdjson::document doc = parser.iterate(json_pad);
    std::shared_ptr<DataType> column_type;
    if (tokens.empty()) {
        return {nullptr, infinity::Status::ParserError("Empty column type")};
    } else if (tokens[0] == "vector" || tokens[0] == "multivector" || tokens[0] == "tensor" || tokens[0] == "tensorarray") {
        if (tokens.size() != 3) {
            return {nullptr, infinity::Status::ParserError("vector / multivector / tensor / tensorarray type syntax error")};
        }
        const size_t dimension = std::stoull(tokens[1]);
        const auto &etype = tokens[2];
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
        } else if (etype == "bit") {
            e_data_type = EmbeddingDataType::kElemBit;
        } else {
            return {nullptr, infinity::Status::InvalidEmbeddingDataType(etype)};
        }
        auto type_info = EmbeddingInfo::Make(e_data_type, dimension);
        auto logical_type_v = LogicalType::kInvalid;
        if (tokens[0] == "vector") {
            logical_type_v = LogicalType::kEmbedding;
        } else if (tokens[0] == "multivector") {
            logical_type_v = LogicalType::kMultiVector;
        } else if (tokens[0] == "tensor") {
            logical_type_v = LogicalType::kTensor;
        } else if (tokens[0] == "tensorarray") {
            logical_type_v = LogicalType::kTensorArray;
        }
        column_type = std::make_shared<DataType>(logical_type_v, std::move(type_info));
    } else if (tokens[0] == "sparse") {
        if (tokens.size() != 4) {
            return {nullptr, infinity::Status::ParserError("sparse type syntax error")};
        }
        const size_t dimension = std::stoull(tokens[1]);
        const auto &dtype = tokens[2];
        const auto &itype = tokens[3];
        EmbeddingDataType d_data_type;
        EmbeddingDataType i_data_type;
        if (dtype == "integer" || dtype == "int" || dtype == "int32") {
            d_data_type = EmbeddingDataType::kElemInt32;
        } else if (dtype == "float" || dtype == "float32") {
            d_data_type = EmbeddingDataType::kElemFloat;
        } else if (dtype == "double" || dtype == "float64") {
            d_data_type = EmbeddingDataType::kElemDouble;
        } else {
            return {nullptr, infinity::Status::InvalidEmbeddingDataType(dtype)};
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
            return {nullptr, infinity::Status::InvalidEmbeddingDataType(itype)};
        }
        auto type_info = SparseInfo::Make(d_data_type, i_data_type, dimension, SparseStoreType::kSort);
        column_type = std::make_shared<DataType>(LogicalType::kSparse, std::move(type_info));
    } else if (tokens[0] == "decimal") {
        auto type_info = DecimalInfo::Make(doc["precision"].get<i64>(), doc["scale"].get<i64>());
        column_type = std::make_shared<DataType>(LogicalType::kDecimal, std::move(type_info));
    } else if (tokens[0] == "array") {
        auto [element_type, stat] = ParseColumnType(tokens.subspan<1>(), doc.raw_json());
        if (!stat.ok()) {
            return {nullptr, std::move(stat)};
        }
        auto type_info = ArrayInfo::Make(std::move(*element_type));
        column_type = std::make_shared<DataType>(LogicalType::kArray, std::move(type_info));
    } else if (tokens.size() == 1) {
        column_type = DataType::StringDeserialize(tokens[0]);
    } else {
        return {nullptr, infinity::Status::ParserError(fmt::format("{} isn't supported.", tokens[0]))};
    }
    return std::make_pair(std::move(column_type), infinity::Status::OK());
}

infinity::Status ParseColumnDefs(std::string_view json_sv, std::vector<ColumnDef *> &column_definitions) {
    simdjson::padded_string json_pad(json_sv);
    simdjson::parser parser;
    simdjson::document doc = parser.iterate(json_pad);
    for (size_t column_id = 0; auto element : doc.get_array()) {
        std::string_view element_sv = element.raw_json();
        std::string column_name;
        if (simdjson::value val; element["name"].get(val) != simdjson::SUCCESS || !val.is_string()) {
            return infinity::Status::InvalidColumnDefinition("Name field is missing or not string");
        } else {
            column_name = val.get<std::string>();
        }

        std::string value_type;
        if (simdjson::value val; element["type"].get(val) != simdjson::SUCCESS || !val.is_string()) {
            return infinity::Status::InvalidColumnDefinition("Type field is missing or not string");
        } else {
            value_type = val.get<std::string>();
        }
        ToLower(value_type);

        std::vector<std::string> tokens;
        tokens = infinity::SplitStrByComma(value_type);

        std::shared_ptr<DataType> column_type{nullptr};
        try {
            auto [result_type, err_status] = ParseColumnType(tokens, element_sv);
            if (!err_status.ok()) {
                return std::move(err_status);
            }
            column_type = std::move(result_type);
        } catch (std::exception &e) {
            return infinity::Status::ParserError("type syntax error");
        }

        if (column_type) {
            std::set<ConstraintType> constraints;
            if (simdjson::array array; element["constraints"].get(array) == simdjson::SUCCESS) {
                for (auto constraint_json : array) {
                    std::string constraint = constraint_json.get<std::string>();
                    ToLower(constraint);
                    constraints.insert(StringToConstraintType(constraint));
                }
            }

            std::string table_comment;
            if (simdjson::value val; element["comment"].get(val) == simdjson::SUCCESS) {
                table_comment = val.get<std::string>();
            }

            std::shared_ptr<ParsedExpr> default_expr{nullptr};
            if (simdjson::value val; element["default"].get(val) == simdjson::SUCCESS) {
                switch (column_type->type()) {
                    case LogicalType::kSparse: {
                        default_expr =
                            BuildConstantSparseExprFromJson(val.raw_json(), dynamic_cast<const SparseInfo *>(column_type->type_info().get()));
                        break;
                    }
                    default: {
                        default_expr = BuildConstantExprFromJson(val.raw_json());
                        break;
                    }
                }
            }
            auto *col_def = new ColumnDef(column_id++, column_type, column_name, constraints, table_comment, default_expr);
            column_definitions.emplace_back(col_def);
        } else {
            return infinity::Status::NotSupport(
                fmt::format("{} type is not supported yet.", std::string(static_cast<std::string_view>(element["type"].raw_json()))));
        }
    }
    return Status::OK();
}

class ListDatabaseHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto result = infinity->ListDatabases();
        nlohmann::json json_response;
        HTTPStatus http_status;
        if (result.IsOk()) {
            size_t block_rows = result.result_table_->DataBlockCount();
            for (size_t block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                for (int i = 0; i < row_count; ++i) {
                    Value value = data_block->GetValue(0, i);
                    const std::string &db_name = value.GetVarchar();
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
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        // get database name
        std::string database_name = request->getPathVariable("database_name");

        // get create option
        std::string body_info = request->readBodyToString();
        simdjson::padded_string json_pad(body_info);
        simdjson::parser parser;
        simdjson::document doc = parser.iterate(json_pad);
        std::string option = doc["create_option"].get<std::string>();

        HTTPStatus http_status;
        nlohmann::json json_response;

        CreateDatabaseOptions options;
        if (simdjson::value val; doc["create_option"].get(val) == simdjson::SUCCESS) {
            if (val.is_string()) {
                std::string option = val.get<std::string>();
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
                json_response["error_code"] = 3074;
                json_response["error_message"] = "'CREATE OPTION' field value should be string type";
                http_status = HTTPStatus::CODE_500;
                return ResponseFactory::createResponse(http_status, json_response.dump());
            }
        }

        std::string db_comment;
        if (simdjson::value val; doc["comment"].get(val) == simdjson::SUCCESS) {
            db_comment = val.get<std::string>();
        }

        // create database
        auto result = infinity->CreateDatabase(database_name, options, db_comment);

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
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        // get database name
        std::string database_name = request->getPathVariable("database_name");

        // get drop option
        HTTPStatus http_status;
        nlohmann::json json_response;

        std::string body_info = request->readBodyToString();
        simdjson::padded_string json_pad(body_info);
        simdjson::parser parser;
        simdjson::document doc = parser.iterate(json_pad);
        DropDatabaseOptions options;
        if (simdjson::value val; doc["drop_option"].get(val) == simdjson::SUCCESS) {
            if (val.is_string()) {
                std::string option = val.get<std::string>();
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
                json_response["error_code"] = 3075;
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
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto database_name = request->getPathVariable("database_name");
        auto result = infinity->ShowDatabase(database_name);

        nlohmann::json json_response;
        nlohmann::json json_res;
        HTTPStatus http_status;

        if (result.IsOk()) {
            size_t block_rows = result.result_table_->DataBlockCount();
            for (size_t block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                auto column_cnt = result.result_table_->ColumnCount();

                for (int row = 0; row < row_count; ++row) {
                    nlohmann::json json_database;
                    for (size_t col = 0; col < column_cnt; ++col) {
                        Value value = data_block->GetValue(col, row);
                        const std::string &column_name = result.result_table_->GetColumnNameById(col);
                        const std::string &column_value = value.ToString();
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
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        std::string database_name = request->getPathVariable("database_name");
        std::string table_name = request->getPathVariable("table_name");

        std::string body_info = request->readBodyToString();
        simdjson::padded_string json_pad(body_info);
        simdjson::parser parser;
        simdjson::document doc = parser.iterate(json_pad);

        auto fields = doc["fields"];

        nlohmann::json json_response;
        json_response["error_code"] = 0;
        HTTPStatus http_status;
        http_status = HTTPStatus::CODE_200;

        if (fields.type() != simdjson::json_type::array) {
            infinity::Status status = infinity::Status::InvalidColumnDefinition("Expect json array in column definitions");
            json_response["error_code"] = status.code();
            json_response["error_message"] = status.message();
            HTTPStatus http_status;
            http_status = HTTPStatus::CODE_500;
            return ResponseFactory::createResponse(http_status, json_response.dump());
        }

        std::vector<ColumnDef *> column_definitions;
        std::vector<TableConstraint *> table_constraint;
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
        if (infinity::Status status = ParseColumnDefs(fields.raw_json(), column_definitions); !status.ok()) {
            json_response["error_code"] = status.code();
            json_response["error_message"] = status.message();
            HTTPStatus http_status;
            http_status = HTTPStatus::CODE_500;
            return ResponseFactory::createResponse(http_status, json_response.dump());
        }

        CreateTableOptions options;
        if (simdjson::value val; doc["create_option"].get(val) == simdjson::SUCCESS) {
            if (val.is_string()) {
                std::string option = val.get<std::string>();
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
                json_response["error_code"] = 3074;
                json_response["error_message"] = "'CREATE OPTION' field value should be string type";
                http_status = HTTPStatus::CODE_500;
            }
        }

        if (json_response["error_code"] == 0) {
            auto result = infinity->CreateTable(database_name, table_name, column_definitions, table_constraint, options);
            column_definitions.clear();
            table_constraint.clear();
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

class RenameTableHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        std::string database_name = request->getPathVariable("database_name");
        std::string table_name = request->getPathVariable("table_name");
        std::string body_info = request->readBodyToString();
        simdjson::padded_string json_pad(body_info);
        simdjson::parser parser;
        simdjson::document doc = parser.iterate(json_pad);
        std::string new_table_name = doc["new_table_name"].get<std::string>();

        auto result = infinity->RenameTable(database_name, table_name, new_table_name);

        nlohmann::json json_response;
        HTTPStatus http_status;
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

class DropTableHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        std::string database_name = request->getPathVariable("database_name");
        std::string table_name = request->getPathVariable("table_name");
        std::string body_info = request->readBodyToString();

        simdjson::padded_string json_pad(body_info);
        simdjson::parser parser;
        simdjson::document doc = parser.iterate(json_pad);

        HTTPStatus http_status;
        http_status = HTTPStatus::CODE_200;
        nlohmann::json json_response;
        json_response["error_code"] = 0;

        DropTableOptions options;
        if (simdjson::value val; doc["drop_option"].get(val) == simdjson::SUCCESS) {
            if (val.is_string()) {
                std::string option = val.get<std::string>();
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
                json_response["error_code"] = 3075;
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
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        std::string database_name = request->getPathVariable("database_name");
        auto result = infinity->ShowTables(database_name);
        nlohmann::json json_response;
        HTTPStatus http_status;
        if (result.IsOk()) {
            size_t block_rows = result.result_table_->DataBlockCount();
            for (size_t block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                for (int row = 0; row < row_count; ++row) {
                    // Column 1: table name
                    Value value = data_block->GetValue(1, row);
                    const std::string &column_value = value.ToString();
                    json_response["table_names"].push_back(column_value);
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
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        std::string database_name = request->getPathVariable("database_name");
        std::string table_name = request->getPathVariable("table_name");

        auto result = infinity->ShowTable(database_name, table_name);
        nlohmann::json json_response;
        nlohmann::json json_res;
        HTTPStatus http_status;
        if (result.IsOk()) {
            size_t block_rows = result.result_table_->DataBlockCount();
            for (size_t block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                auto column_cnt = result.result_table_->ColumnCount();
                for (int row = 0; row < row_count; ++row) {
                    nlohmann::json json_table;
                    for (size_t col = 0; col < column_cnt; ++col) {
                        const std::string &column_name = result.result_table_->GetColumnNameById(col);
                        Value value = data_block->GetValue(col, row);
                        const std::string &column_value = value.ToString();
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
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        std::string database_name = request->getPathVariable("database_name");
        std::string table_name = request->getPathVariable("table_name");

        nlohmann::json json_response;
        HTTPStatus http_status = HTTPStatus::CODE_500;

        std::string data_body = request->readBodyToString();
        try {
            simdjson::padded_string json_pad(data_body);
            simdjson::parser parser;
            simdjson::document doc = parser.iterate(json_pad);
            ExportOptions export_options;

            std::string file_type_str = doc["file_type"].get<std::string>();
            ToLower(file_type_str);
            if (file_type_str == "csv") {
                export_options.copy_file_type_ = CopyFileType::kCSV;
            } else if (file_type_str == "jsonl") {
                export_options.copy_file_type_ = CopyFileType::kJSONL;
            } else if (file_type_str == "fvecs") {
                export_options.copy_file_type_ = CopyFileType::kFVECS;
            } else {
                json_response["error_code"] = ErrorCode::kNotSupported;
                json_response["error_message"] = fmt::format("Not supported file type {}", file_type_str);
                return ResponseFactory::createResponse(http_status, json_response.dump());
            }

            if (json_response["error_code"] != ErrorCode::kNotSupported) {
                if (simdjson::value val; doc["header"].get(val) == simdjson::SUCCESS) {
                    export_options.header_ = val.get<bool>();
                }
                if (simdjson::value val; doc["offset"].get(val) == simdjson::SUCCESS) {
                    export_options.offset_ = val.get<size_t>();
                }
                if (simdjson::value val; doc["limit"].get(val) == simdjson::SUCCESS) {
                    export_options.limit_ = val.get<size_t>();
                }
                if (simdjson::value val; doc["row_limit"].get(val) == simdjson::SUCCESS) {
                    export_options.row_limit_ = val.get<size_t>();
                }
                if (simdjson::value val; doc["delimiter"].get(val) == simdjson::SUCCESS) {
                    std::string delimiter = val.get<std::string>();
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
            std::string file_path = doc["file_path"].get<std::string>();
            std::vector<ParsedExpr *> *export_columns{nullptr};
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

            if (simdjson::value val; doc["columns"].get(val) == simdjson::SUCCESS) {
                export_columns = new std::vector<ParsedExpr *>();

                for (auto column : val.get_array()) {
                    if (column.is_string()) {
                        std::string column_name = column.get<std::string>();
                        ToLower(column_name);
                        if (column_name == "_row_id") {
                            auto *expr = new FunctionExpr();
                            expr->func_name_ = "row_id";
                            export_columns->emplace_back(expr);
                        } else if (column_name == "_create_timestamp") {
                            auto *expr = new FunctionExpr();
                            expr->func_name_ = "create_timestamp";
                            export_columns->emplace_back(expr);
                        } else if (column_name == "_delete_timestamp") {
                            auto *expr = new FunctionExpr();
                            expr->func_name_ = "delete_timestamp";
                            export_columns->emplace_back(expr);
                        } else {
                            auto *expr = new ColumnExpr();
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
        } catch (simdjson::simdjson_error &e) {
            json_response["error_code"] = ErrorCode::kInvalidJsonFormat;
            json_response["error_message"] = e.what();
        }

        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class ShowTableColumnsHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        std::string database_name = request->getPathVariable("database_name");
        std::string table_name = request->getPathVariable("table_name");

        auto result = infinity->ShowColumns(database_name, table_name);
        nlohmann::json json_response;
        HTTPStatus http_status;
        if (result.IsOk()) {
            size_t block_rows = result.result_table_->DataBlockCount();
            for (size_t block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                auto column_cnt = result.result_table_->ColumnCount();
                for (int row = 0; row < row_count; ++row) {
                    nlohmann::json json_table;
                    for (size_t col = 0; col < column_cnt; ++col) {
                        const std::string &column_name = result.result_table_->GetColumnNameById(col);
                        Value value = data_block->GetValue(col, row);
                        const std::string &column_value = value.ToString();
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
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        std::string database_name = request->getPathVariable("database_name");
        std::string table_name = request->getPathVariable("table_name");

        nlohmann::json json_response;
        HTTPStatus http_status = HTTPStatus::CODE_500;

        std::string data_body = request->readBodyToString();
        try {
            simdjson::padded_string json_pad(data_body);
            simdjson::parser parser;
            simdjson::document doc = parser.iterate(json_pad);
            ImportOptions import_options;

            std::string file_type_str = doc["file_type"].get<std::string>();
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
                if (simdjson::value val; doc["header"].get(val) == simdjson::SUCCESS) {
                    import_options.header_ = val.get<bool>();
                }
                if (simdjson::value val; doc["delimiter"].get(val) == simdjson::SUCCESS) {
                    std::string delimiter = val.get<std::string>();
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
            std::string file_path = doc["file_path"].get<std::string>();

            auto result = infinity->Import(database_name, table_name, file_path, import_options);
            if (result.IsOk()) {
                json_response["error_code"] = 0;
                http_status = HTTPStatus::CODE_200;
            } else {
                json_response["error_code"] = result.ErrorCode();
                json_response["error_message"] = result.ErrorMsg();
                http_status = HTTPStatus::CODE_500;
            }
        } catch (simdjson::simdjson_error &e) {
            json_response["error_code"] = ErrorCode::kInvalidJsonFormat;
            json_response["error_message"] = e.what();
        }

        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class InsertHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        nlohmann::json json_response;
        HTTPStatus http_status = HTTPStatus::CODE_500;

        std::string data_body = request->readBodyToString();
        try {
            simdjson::padded_string json_pad(data_body);
            simdjson::parser parser;
            simdjson::document doc = parser.iterate(json_pad);

            if (!(doc.type() == simdjson::json_type::array && doc.count_elements() > 0)) {
                json_response["error_code"] = ErrorCode::kInvalidJsonFormat;
                json_response["error_message"] = fmt::format("Invalid json format: {}", data_body);
            }
            auto *insert_rows = new std::vector<InsertRowExpr *>();
            DeferFn free_insert_rows([&]() {
                if (insert_rows != nullptr) {
                    for (auto *insert_row : *insert_rows) {
                        delete insert_row;
                        insert_row = nullptr;
                    }
                    delete insert_rows;
                    insert_rows = nullptr;
                }
            });
            for (auto element : doc.get_array()) {
                auto insert_one_row = std::make_unique<InsertRowExpr>();
                for (std::set<std::string> column_name_set; auto field : element.get_object()) {
                    std::string key = std::string((std::string_view)field.unescaped_key());
                    ToLower(key);
                    if (const auto [_, success] = column_name_set.insert(key); !success) {
                        json_response["error_code"] = ErrorCode::kDuplicateColumnName;
                        json_response["error_message"] = fmt::format("Duplicated column name: {}", key);
                        return ResponseFactory::createResponse(http_status, json_response.dump());
                    }
                    insert_one_row->columns_.emplace_back(key);
                    auto value = field.value();
                    switch (value.type()) {
                        case simdjson::json_type::boolean: {
                            // Generate constant expression
                            auto const_expr = std::make_unique<ConstantExpr>(LiteralType::kBoolean);
                            const_expr->bool_value_ = value.get<bool>();
                            insert_one_row->values_.emplace_back(std::move(const_expr));
                            break;
                        }
                        case simdjson::json_type::number: {
                            switch (value.get_number_type()) {
                                case simdjson::number_type::signed_integer: {
                                    // Generate constant expression
                                    auto const_expr = std::make_unique<ConstantExpr>(LiteralType::kInteger);
                                    const_expr->integer_value_ = value.get<i64>();
                                    insert_one_row->values_.emplace_back(std::move(const_expr));
                                    break;
                                }
                                case simdjson::number_type::unsigned_integer: {
                                    // Generate constant expression
                                    auto const_expr = std::make_unique<ConstantExpr>(LiteralType::kInteger);
                                    const_expr->integer_value_ = value.get<u64>();
                                    insert_one_row->values_.emplace_back(std::move(const_expr));
                                    break;
                                }
                                case simdjson::number_type::floating_point_number: {
                                    // Generate constant expression
                                    auto const_expr = std::make_unique<ConstantExpr>(LiteralType::kDouble);
                                    const_expr->double_value_ = value.get<f64>();
                                    insert_one_row->values_.emplace_back(std::move(const_expr));
                                    break;
                                }
                                default: {
                                    json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                    json_response["error_message"] = "Embedding element type can only be integer or float";
                                    return ResponseFactory::createResponse(http_status, json_response.dump());
                                }
                            }
                            break;
                        }
                        case simdjson::json_type::string: {
                            auto const_expr = std::make_unique<ConstantExpr>(LiteralType::kString);
                            const_expr->str_value_ = strdup(((std::string)value.get<std::string>()).c_str());
                            insert_one_row->values_.emplace_back(std::move(const_expr));
                            break;
                        }
                        case simdjson::json_type::array: {
                            size_t dimension = value.count_elements();
                            if (dimension == 0) {
                                json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                json_response["error_message"] =
                                    fmt::format("Empty embedding data: {}", std::string((std::string_view)value.raw_json()));
                                return ResponseFactory::createResponse(http_status, json_response.dump());
                            }

                            std::unique_ptr<ConstantExpr> const_expr = {};
                            for (auto sub_value : value.get_array()) {
                                switch (sub_value.type()) {
                                    case simdjson::json_type::array: {
                                        if (const_expr == nullptr) {
                                            const_expr = std::make_unique<ConstantExpr>(LiteralType::kSubArrayArray);
                                        }
                                        size_t subdimension = sub_value.count_elements();
                                        if (subdimension == 0) {
                                            json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                            json_response["error_message"] =
                                                fmt::format("Empty tensor array: {}", std::string((std::string_view)sub_value.raw_json()));
                                            return ResponseFactory::createResponse(http_status, json_response.dump());
                                        }

                                        std::unique_ptr<ConstantExpr> const_expr_2 = {};
                                        for (auto sub_sub_value : sub_value.get_array()) {
                                            switch (sub_sub_value.type()) {
                                                case simdjson::json_type::array: {
                                                    if (const_expr_2 == nullptr) {
                                                        const_expr_2 = std::make_unique<ConstantExpr>(LiteralType::kSubArrayArray);
                                                    }
                                                    size_t subsubdimension = sub_sub_value.count_elements();
                                                    if (subsubdimension == 0) {
                                                        json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                                        json_response["error_message"] =
                                                            fmt::format("Empty tensor array: {}",
                                                                        std::string((std::string_view)sub_sub_value.raw_json()));
                                                        return ResponseFactory::createResponse(http_status, json_response.dump());
                                                    }

                                                    std::unique_ptr<ConstantExpr> const_expr_3 = {};
                                                    for (auto sub_sub_sub_value : sub_sub_value.get_array()) {
                                                        if (sub_sub_sub_value.type() != simdjson::json_type::number) {
                                                            json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                                            json_response["error_message"] =
                                                                "Embedding element type can only be integer or float or tensor or tensor array";
                                                            return ResponseFactory::createResponse(http_status, json_response.dump());
                                                        }

                                                        switch (sub_sub_sub_value.get_number_type()) {
                                                            case simdjson::number_type::signed_integer: {
                                                                if (const_expr_3 == nullptr) {
                                                                    const_expr_3 = std::make_unique<ConstantExpr>(LiteralType::kIntegerArray);
                                                                }
                                                                const_expr_3->long_array_.emplace_back(sub_sub_sub_value.get<i64>());
                                                                break;
                                                            }
                                                            case simdjson::number_type::unsigned_integer: {
                                                                if (const_expr_3 == nullptr) {
                                                                    const_expr_3 = std::make_unique<ConstantExpr>(LiteralType::kIntegerArray);
                                                                }
                                                                const_expr_3->long_array_.emplace_back(sub_sub_sub_value.get<u64>());
                                                                break;
                                                            }
                                                            case simdjson::number_type::floating_point_number: {
                                                                if (const_expr_3 == nullptr) {
                                                                    const_expr_3 = std::make_unique<ConstantExpr>(LiteralType::kDoubleArray);
                                                                }
                                                                const_expr_3->double_array_.emplace_back(sub_sub_sub_value.get<double>());
                                                                break;
                                                            }
                                                            default: {
                                                                json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                                                json_response["error_message"] =
                                                                    "Embedding element type can only be integer or float or tensor or tensor array";
                                                                return ResponseFactory::createResponse(http_status, json_response.dump());
                                                            }
                                                        }
                                                    }
                                                    const_expr_2->sub_array_array_.emplace_back(std::move(const_expr_3));
                                                    break;
                                                }
                                                case simdjson::json_type::number: {
                                                    switch (sub_sub_value.get_number_type()) {
                                                        case simdjson::number_type::signed_integer: {
                                                            if (const_expr_2 == nullptr) {
                                                                const_expr_2 = std::make_unique<ConstantExpr>(LiteralType::kIntegerArray);
                                                            }
                                                            const_expr_2->long_array_.emplace_back(sub_sub_value.get<i64>());
                                                            break;
                                                        }
                                                        case simdjson::number_type::unsigned_integer: {
                                                            if (const_expr_2 == nullptr) {
                                                                const_expr_2 = std::make_unique<ConstantExpr>(LiteralType::kIntegerArray);
                                                            }
                                                            const_expr_2->long_array_.emplace_back(sub_sub_value.get<u64>());
                                                            break;
                                                        }
                                                        case simdjson::number_type::floating_point_number: {
                                                            if (const_expr_2 == nullptr) {
                                                                const_expr_2 = std::make_unique<ConstantExpr>(LiteralType::kDoubleArray);
                                                            }
                                                            const_expr_2->double_array_.emplace_back(sub_sub_value.get<double>());
                                                            break;
                                                        }
                                                        default: {
                                                            json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                                            json_response["error_message"] =
                                                                "Embedding element type can only be integer or float or tensor or tensor array";
                                                            return ResponseFactory::createResponse(http_status, json_response.dump());
                                                        }
                                                    }
                                                    break;
                                                }
                                                default: {
                                                    json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                                    json_response["error_message"] =
                                                        "Embedding element type can only be integer or float or tensor or tensor array";
                                                    return ResponseFactory::createResponse(http_status, json_response.dump());
                                                }
                                            }
                                        }
                                        const_expr->sub_array_array_.emplace_back(std::move(const_expr_2));
                                        break;
                                    }
                                    case simdjson::json_type::number: {
                                        switch (sub_value.get_number_type()) {
                                            case simdjson::number_type::signed_integer: {
                                                // Generate constant expression
                                                if (const_expr == nullptr) {
                                                    const_expr = std::make_unique<ConstantExpr>(LiteralType::kIntegerArray);
                                                }
                                                const_expr->long_array_.emplace_back(sub_value.get<i64>());
                                                break;
                                            }
                                            case simdjson::number_type::unsigned_integer: {
                                                // Generate constant expression
                                                if (const_expr == nullptr) {
                                                    const_expr = std::make_unique<ConstantExpr>(LiteralType::kIntegerArray);
                                                }
                                                const_expr->long_array_.emplace_back(sub_value.get<u64>());
                                                break;
                                            }
                                            case simdjson::number_type::floating_point_number: {
                                                if (const_expr == nullptr) {
                                                    const_expr = std::make_unique<ConstantExpr>(LiteralType::kDoubleArray);
                                                }
                                                const_expr->double_array_.emplace_back(sub_value.get<double>());
                                                break;
                                            }
                                            default: {
                                                json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                                json_response["error_message"] =
                                                    "Embedding element type can only be integer or float or tensor or tensor array";
                                                return ResponseFactory::createResponse(http_status, json_response.dump());
                                            }
                                        }
                                        break;
                                    }
                                    default: {
                                        json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                        json_response["error_message"] =
                                            "Embedding element type can only be integer or float or tensor or tensor array";
                                        return ResponseFactory::createResponse(http_status, json_response.dump());
                                    }
                                }
                            }
                            insert_one_row->values_.emplace_back(std::move(const_expr));
                            break;
                        }
                        case simdjson::json_type::object: {
                            simdjson::object value_obj = value.get_object();

                            // Check for FDE function call
                            if (simdjson::value fde_func; value_obj["function"].get(fde_func) == simdjson::SUCCESS) {
                                std::string func_name = std::string((std::string_view)fde_func.get_string());
                                ToLower(func_name);

                                if (func_name == "fde") {
                                    // Parse FDE function parameters
                                    auto tensor_data_result = value_obj["tensor_data"].get_array();
                                    auto target_dim_result = value_obj["target_dimension"].get_uint64();

                                    if (tensor_data_result.error() != simdjson::SUCCESS || target_dim_result.error() != simdjson::SUCCESS) {
                                        json_response["error_code"] = ErrorCode::kInvalidJsonFormat;
                                        json_response["error_message"] =
                                            fmt::format("FDE function requires 'tensor_data' (array) and 'target_dimension' (number) "
                                                        "parameters. tensor_result: {}, dim_result: {}",
                                                        static_cast<int>(tensor_data_result.error()),
                                                        static_cast<int>(target_dim_result.error()));
                                        return ResponseFactory::createResponse(http_status, json_response.dump());
                                    }

                                    auto tensor_array = tensor_data_result.value();
                                    auto target_dimension = target_dim_result.value();

                                    // Create FDE function expression
                                    auto func_expr = std::make_unique<FunctionExpr>();
                                    func_expr->func_name_ = "fde";
                                    func_expr->arguments_ = new std::vector<ParsedExpr *>();

                                    // Parse tensor data (2D array) using SubArrayArray

                                    auto tensor_const_expr = std::make_unique<ConstantExpr>(LiteralType::kSubArrayArray);

                                    for (auto row : tensor_array) {
                                        auto row_array_result = row.get_array();
                                        if (row_array_result.error() != simdjson::SUCCESS) {
                                            json_response["error_code"] = ErrorCode::kInvalidJsonFormat;
                                            json_response["error_message"] = "FDE tensor_data must be a 2D array";
                                            return ResponseFactory::createResponse(http_status, json_response.dump());
                                        }

                                        auto row_array = row_array_result.value();

                                        // Create a sub-array for this row
                                        auto sub_array_expr = std::make_unique<ConstantExpr>(LiteralType::kDoubleArray);
                                        for (auto element : row_array) {
                                            if (element.type() != simdjson::json_type::number) {
                                                json_response["error_code"] = ErrorCode::kInvalidJsonFormat;
                                                json_response["error_message"] = "FDE tensor elements must be numbers";
                                                return ResponseFactory::createResponse(http_status, json_response.dump());
                                            }
                                            sub_array_expr->double_array_.emplace_back(element.get<double>());
                                        }
                                        tensor_const_expr->sub_array_array_.emplace_back(std::move(sub_array_expr));
                                    }

                                    func_expr->arguments_->emplace_back(tensor_const_expr.release());

                                    // Parse target dimension
                                    auto dim_const_expr = std::make_unique<ConstantExpr>(LiteralType::kInteger);
                                    dim_const_expr->integer_value_ = static_cast<i64>(target_dimension);
                                    func_expr->arguments_->emplace_back(dim_const_expr.release());

                                    insert_one_row->values_.emplace_back(std::move(func_expr));
                                    break;
                                } else {
                                    json_response["error_code"] = ErrorCode::kInvalidJsonFormat;
                                    json_response["error_message"] = fmt::format("Unsupported function: {}", func_name);
                                    return ResponseFactory::createResponse(http_status, json_response.dump());
                                }
                            }

                            // check array type
                            if (simdjson::value val_arr; value_obj.count_fields() == 1 && value_obj["array"].get(val_arr) == simdjson::SUCCESS) {
                                std::shared_ptr<ConstantExpr> array_expr;
                                try {
                                    auto array_result = BuildConstantExprFromJson(value_obj.raw_json());
                                    if (!array_result) {
                                        throw std::runtime_error("Empty return value!");
                                    }
                                    array_expr = std::move(array_result);
                                } catch (std::exception &e) {
                                    json_response["error_code"] = ErrorCode::kSyntaxError;
                                    json_response["error_message"] = fmt::format("Error when parsing array value: {}", e.what());
                                    return ResponseFactory::createResponse(http_status, json_response.dump());
                                }
                                auto const_expr = std::make_unique<ConstantExpr>(std::move(*array_expr));
                                insert_one_row->values_.emplace_back(std::move(const_expr));
                                break;
                            }
                            std::unique_ptr<ConstantExpr> const_sparse_expr = {};
                            if (value_obj.count_fields() == 0) {
                                json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                json_response["error_message"] = "Empty sparse vector, cannot decide type";
                                return ResponseFactory::createResponse(http_status, json_response.dump());
                            }
                            for (std::unordered_set<i64> key_set; auto sparse_it : value_obj) {
                                const std::string sparse_k = std::string((std::string_view)sparse_it.unescaped_key());
                                auto sparse_v = sparse_it.value();
                                i64 key_val = std::stoll(sparse_k);

                                if (sparse_v.type() != simdjson::json_type::number) {
                                    json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                    json_response["error_message"] = "Sparse value element type error";
                                    return ResponseFactory::createResponse(http_status, json_response.dump());
                                }

                                if (const_sparse_expr == nullptr) {
                                    switch (sparse_v.get_number_type()) {
                                        case simdjson::number_type::signed_integer:
                                        case simdjson::number_type::unsigned_integer: {
                                            const_sparse_expr = std::make_unique<ConstantExpr>(LiteralType::kLongSparseArray);
                                            break;
                                        }
                                        case simdjson::number_type::floating_point_number: {
                                            const_sparse_expr = std::make_unique<ConstantExpr>(LiteralType::kDoubleSparseArray);
                                            break;
                                        }
                                        default: {
                                            json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                            json_response["error_message"] = "Sparse value element type error";
                                            return ResponseFactory::createResponse(http_status, json_response.dump());
                                        }
                                    }
                                }

                                if (const auto [_, insert_ok] = key_set.insert(key_val); !insert_ok) {
                                    json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                    json_response["error_message"] = fmt::format("Duplicate key {} in sparse array!", key);
                                    return ResponseFactory::createResponse(http_status, json_response.dump());
                                }
                                bool good_v = false;
                                switch (sparse_v.get_number_type()) {
                                    case simdjson::number_type::signed_integer:
                                    case simdjson::number_type::unsigned_integer: {
                                        if (const_sparse_expr->literal_type_ == LiteralType::kLongSparseArray) {
                                            const_sparse_expr->long_sparse_array_.first.push_back(key_val);
                                            const_sparse_expr->long_sparse_array_.second.push_back(sparse_v.get<i64>());
                                            good_v = true;
                                        }
                                        break;
                                    }
                                    case simdjson::number_type::floating_point_number: {
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
                                    json_response["error_message"] = "Sparse value element type error";
                                    return ResponseFactory::createResponse(http_status, json_response.dump());
                                }
                            }
                            insert_one_row->values_.emplace_back(std::move(const_sparse_expr));
                            break;
                        }
                        default: {
                            json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                            json_response["error_message"] = "Embedding element type can only be integer or float";
                            return ResponseFactory::createResponse(http_status, json_response.dump());
                        }
                    }
                }
                insert_rows->emplace_back(insert_one_row.release());
            }

            auto database_name = request->getPathVariable("database_name");
            auto table_name = request->getPathVariable("table_name");
            auto result = infinity->Insert(database_name, table_name, insert_rows);
            insert_rows = nullptr;
            if (result.IsOk()) {
                json_response["error_code"] = 0;
                http_status = HTTPStatus::CODE_200;
            } else {
                json_response["error_code"] = result.ErrorCode();
                json_response["error_message"] = result.ErrorMsg();
                http_status = HTTPStatus::CODE_500;
            }

        } catch (simdjson::simdjson_error &e) {
            json_response["error_code"] = ErrorCode::kInvalidJsonFormat;
            json_response["error_message"] = e.what();
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class DeleteHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        nlohmann::json json_response;
        HTTPStatus http_status = HTTPStatus::CODE_500;

        std::string data_body = request->readBodyToString();
        try {
            simdjson::padded_string json_pad(data_body);
            simdjson::parser parser;
            simdjson::document doc = parser.iterate(json_pad);
            const std::string filter_string = doc["filter"].get<std::string>();
            if (filter_string != "") {
                std::unique_ptr<ExpressionParserResult> expr_parsed_result = std::make_unique<ExpressionParserResult>();
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
                    json_response["deleted_rows"] = value.value_.big_int;
                } else {
                    json_response["error_code"] = result.ErrorCode();
                    json_response["error_message"] = result.ErrorMsg();
                    http_status = HTTPStatus::CODE_500;
                }
            } else {
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
                    json_response["deleted_rows"] = value.value_.big_int;
                } else {
                    json_response["error_code"] = result.ErrorCode();
                    json_response["error_message"] = result.ErrorMsg();
                    http_status = HTTPStatus::CODE_500;
                }
            }
        } catch (simdjson::simdjson_error &e) {
            json_response["error_code"] = ErrorCode::kInvalidJsonFormat;
            json_response["error_message"] = e.what();
        }

        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class UpdateHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        nlohmann::json json_response;
        HTTPStatus http_status = HTTPStatus::CODE_500;

        std::string data_body = request->readBodyToString();
        try {
            simdjson::padded_string json_pad(data_body);
            simdjson::parser parser;
            simdjson::document doc = parser.iterate(json_pad);
            auto update_clause = doc["update"];

            std::vector<UpdateExpr *> *update_expr_array = new std::vector<UpdateExpr *>();
            DeferFn defer_free_update_expr_array([&]() {
                if (update_expr_array != nullptr) {
                    for (auto &expr : *update_expr_array) {
                        delete expr;
                    }
                    delete update_expr_array;
                    update_expr_array = nullptr;
                }
            });
            update_expr_array->reserve(update_clause.count_fields());

            for (auto update_elem : update_clause.get_object()) {
                UpdateExpr *update_expr = new UpdateExpr();
                DeferFn defer_free_update_expr([&]() {
                    if (update_expr != nullptr) {
                        delete update_expr;
                        update_expr = nullptr;
                    }
                });
                update_expr->column_name = std::string((std::string_view)update_elem.unescaped_key());
                auto value = update_elem.value();
                switch (value.type()) {
                    case simdjson::json_type::boolean: {
                        // Generate constant expression
                        infinity::ConstantExpr *const_expr = new ConstantExpr(LiteralType::kBoolean);
                        const_expr->bool_value_ = value.get<bool>();
                        update_expr->value = const_expr;
                        const_expr = nullptr;
                        break;
                    }
                    case simdjson::json_type::number: {
                        switch (value.get_number_type()) {
                            case simdjson::number_type::signed_integer: {
                                // Generate constant expression
                                infinity::ConstantExpr *const_expr = new ConstantExpr(LiteralType::kInteger);
                                const_expr->integer_value_ = value.get<i64>();
                                update_expr->value = const_expr;
                                const_expr = nullptr;
                                break;
                            }
                            case simdjson::number_type::unsigned_integer: {
                                // Generate constant expression
                                infinity::ConstantExpr *const_expr = new ConstantExpr(LiteralType::kInteger);
                                const_expr->integer_value_ = value.get<u64>();
                                update_expr->value = const_expr;
                                const_expr = nullptr;
                                break;
                            }
                            case simdjson::number_type::floating_point_number: {
                                // Generate constant expression
                                infinity::ConstantExpr *const_expr = new ConstantExpr(LiteralType::kDouble);
                                const_expr->double_value_ = value.get<f64>();
                                update_expr->value = const_expr;
                                const_expr = nullptr;
                                break;
                            }
                            default: {
                                json_response["error_code"] = ErrorCode::kInvalidExpression;
                                json_response["error_message"] = "Invalid update set expression";
                                return ResponseFactory::createResponse(http_status, json_response.dump());
                            }
                        }
                        break;
                    }
                    case simdjson::json_type::string: {
                        infinity::ConstantExpr *const_expr = new ConstantExpr(LiteralType::kString);
                        std::string str_value = value.get<std::string>();
                        const_expr->str_value_ = strdup(str_value.c_str());
                        update_expr->value = const_expr;
                        const_expr = nullptr;
                        break;
                    }
                    case simdjson::json_type::array: {
                        size_t dimension = value.count_elements();
                        if (dimension == 0) {
                            json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                            json_response["error_message"] = fmt::format("Empty embedding data: {}", std::string((std::string_view)value.raw_json()));
                            return ResponseFactory::createResponse(http_status, json_response.dump());
                        }

                        infinity::ConstantExpr *const_expr = nullptr;
                        DeferFn defer_free_integer_array([&]() {
                            if (const_expr != nullptr) {
                                delete const_expr;
                                const_expr = nullptr;
                            }
                        });
                        for (auto sub_value : value.get_array()) {
                            if (sub_value.type() != simdjson::json_type::number) {
                                json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                json_response["error_message"] = "Embedding element type can only be integer or float";
                                return ResponseFactory::createResponse(http_status, json_response.dump());
                            }

                            switch (sub_value.get_number_type()) {
                                case simdjson::number_type::signed_integer: {
                                    if (const_expr == nullptr) {
                                        const_expr = new ConstantExpr(LiteralType::kIntegerArray);
                                    }
                                    const_expr->long_array_.emplace_back(sub_value.get<i64>());
                                    break;
                                }
                                case simdjson::number_type::unsigned_integer: {
                                    if (const_expr == nullptr) {
                                        const_expr = new ConstantExpr(LiteralType::kIntegerArray);
                                    }
                                    const_expr->long_array_.emplace_back(sub_value.get<u64>());
                                    break;
                                }
                                case simdjson::number_type::floating_point_number: {
                                    if (const_expr == nullptr) {
                                        const_expr = new ConstantExpr(LiteralType::kDoubleArray);
                                    }
                                    const_expr->double_array_.emplace_back(sub_value.get<double>());
                                    break;
                                }
                                default: {
                                    json_response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                                    json_response["error_message"] = "Embedding element type can only be integer or float";
                                    return ResponseFactory::createResponse(http_status, json_response.dump());
                                }
                            }
                        }
                        update_expr->value = const_expr;
                        const_expr = nullptr;
                        break;
                    }
                    default: {
                        json_response["error_code"] = ErrorCode::kInvalidExpression;
                        json_response["error_message"] = "Invalid update set expression";
                        return ResponseFactory::createResponse(http_status, json_response.dump());
                    }
                }

                update_expr_array->emplace_back(update_expr);
                update_expr = nullptr;
            }

            auto filter_value = doc.find_field("filter");
            if (filter_value.is_null()) {
                json_response["error_code"] = ErrorCode::kSyntaxError;
                json_response["error_message"] = "No filter in update clause";
                http_status = HTTPStatus::CODE_500;
                return ResponseFactory::createResponse(http_status, json_response.dump());
            }

            std::string where_clause = filter_value.get<std::string>();

            std::unique_ptr<ExpressionParserResult> expr_parsed_result = std::make_unique<ExpressionParserResult>();
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

        } catch (simdjson::simdjson_error &e) {
            json_response["error_code"] = ErrorCode::kInvalidJsonFormat;
            json_response["error_message"] = e.what();
        }

        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class SelectHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto database_name = request->getPathVariable("database_name");
        auto table_name = request->getPathVariable("table_name");
        std::string data_body = request->readBodyToString();

        nlohmann::json json_response;
        HTTPStatus http_status;

        HTTPSearch::Process(infinity.get(), database_name, table_name, data_body, http_status, json_response);

        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class ExplainHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto database_name = request->getPathVariable("database_name");
        auto table_name = request->getPathVariable("table_name");
        std::string data_body = request->readBodyToString();

        nlohmann::json json_response;
        HTTPStatus http_status;

        HTTPSearch::Explain(infinity.get(), database_name, table_name, data_body, http_status, json_response);

        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class ListTableIndexesHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto database_name = request->getPathVariable("database_name");
        auto table_name = request->getPathVariable("table_name");
        auto result = infinity->ListTableIndexes(database_name, table_name);

        HTTPStatus http_status;
        nlohmann::json json_response;

        if (result.IsOk()) {

            size_t block_rows = result.result_table_->DataBlockCount();
            for (size_t block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();

                for (int row = 0; row < row_count; ++row) {
                    nlohmann::json json_index;

                    {
                        // index name
                        Value value = data_block->GetValue(0, row);
                        const std::string &column_value = value.ToString();
                        json_index["index_name"] = column_value;
                    }

                    {
                        // index type
                        Value value = data_block->GetValue(1, row);
                        const std::string &column_value = value.ToString();
                        json_index["index_type"] = column_value;
                    }

                    {
                        // columns
                        Value value = data_block->GetValue(3, row);
                        const std::string &column_value = value.ToString();
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
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto database_name = request->getPathVariable("database_name");
        auto table_name = request->getPathVariable("table_name");
        auto index_name = request->getPathVariable("index_name");

        auto result = infinity->ShowIndex(database_name, table_name, index_name);

        HTTPStatus http_status;
        nlohmann::json json_response;

        if (result.IsOk()) {

            size_t block_rows = result.result_table_->DataBlockCount();
            for (size_t block_id = 0; block_id < block_rows; ++block_id) {
                std::shared_ptr<DataBlock> data_block = result.result_table_->GetDataBlockById(block_id);
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
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
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

            size_t block_rows = result.result_table_->DataBlockCount();
            for (size_t block_id = 0; block_id < block_rows; ++block_id) {
                std::shared_ptr<DataBlock> data_block = result.result_table_->GetDataBlockById(block_id);
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
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
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

            size_t block_rows = result.result_table_->DataBlockCount();
            for (size_t block_id = 0; block_id < block_rows; ++block_id) {
                std::shared_ptr<DataBlock> data_block = result.result_table_->GetDataBlockById(block_id);
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
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto database_name = request->getPathVariable("database_name");
        auto table_name = request->getPathVariable("table_name");
        auto index_name = request->getPathVariable("index_name");

        std::string body_info = request->readBodyToString();

        simdjson::padded_string json_pad(body_info);
        simdjson::parser parser;
        simdjson::document doc = parser.iterate(json_pad);

        nlohmann::json json_response;
        json_response["error_code"] = 0;
        HTTPStatus http_status;
        http_status = HTTPStatus::CODE_500;
        DropIndexOptions options{ConflictType::kInvalid};

        if (simdjson::value val; doc["drop_option"].get(val) == simdjson::SUCCESS) {
            if (val.is_string()) {
                std::string option = val.get<std::string>();
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
                json_response["error_code"] = 3075;
                json_response["error_message"] = "'DROP OPTION' field value should be string type";
                http_status = HTTPStatus::CODE_500;
            }
        }

        if (json_response["error_code"] == 0) {
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
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto database_name = request->getPathVariable("database_name");
        auto table_name = request->getPathVariable("table_name");
        auto index_name = request->getPathVariable("index_name");

        std::string body_info_str = request->readBodyToString();
        simdjson::padded_string json_pad(body_info_str);
        simdjson::parser parser;
        simdjson::document doc = parser.iterate(json_pad);

        nlohmann::json json_response;
        json_response["error_code"] = 0;
        HTTPStatus http_status;
        http_status = HTTPStatus::CODE_200;

        std::string index_comment;
        if (simdjson::value val; doc["comment"].get(val) == simdjson::SUCCESS) {
            index_comment = val.get<std::string>();
        }

        CreateIndexOptions options;
        if (simdjson::value val; doc["create_option"].get(val) == simdjson::SUCCESS) {
            if (val.is_string()) {
                std::string option = val.get<std::string>();
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
                json_response["error_code"] = 3074;
                json_response["error_message"] = "'CREATE OPTION' field value should be string type";
                http_status = HTTPStatus::CODE_500;
            }
        }

        auto index_info = new IndexInfo();
        DeferFn release_index_info([&]() {
            if (index_info != nullptr) {
                delete index_info;
                index_info = nullptr;
            }
        });
        {
            index_info->column_name_ = doc["fields"].get_array().at(0).get<std::string>();
            ToLower(index_info->column_name_);
            auto index_param_list = new std::vector<InitParameter *>();
            DeferFn release_index_param_list([&]() {
                if (index_param_list != nullptr) {
                    for (auto &index_param_ptr : *index_param_list) {
                        delete index_param_ptr;
                    }
                    delete index_param_list;
                    index_param_list = nullptr;
                }
            });

            for (auto element : doc["index"].get_object()) {
                std::string name = std::string((std::string_view)element.unescaped_key());
                ToLower(name);
                std::string value;
                if (element.value().is_string()) {
                    value = element.value().get<std::string>();
                } else {
                    value = std::string((std::string_view)element.value().raw_json());
                }

                if (strcmp(name.c_str(), "type") == 0) {
                    std::string version_str = value;
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

        if (json_response["error_code"] == 0) {
            auto result = infinity->CreateIndex(database_name, table_name, index_name, index_comment, index_info, options);
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
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto database_name = request->getPathVariable("database_name");
        auto table_name = request->getPathVariable("table_name");
        auto index_name = request->getPathVariable("index_name");

        std::string body_info_str = request->readBodyToString();
        simdjson::padded_string json_pad(body_info_str);
        simdjson::parser parser;
        simdjson::document doc = parser.iterate(json_pad);

        nlohmann::json json_response;
        json_response["error_code"] = 0;
        HTTPStatus http_status;
        http_status = HTTPStatus::CODE_200;

        OptimizeOptions optimize_options;
        optimize_options.index_name_ = index_name;
        if (simdjson::value val; doc["optimize_options"].get(val) == simdjson::SUCCESS) {
            if (val.type() != simdjson::json_type::object) {
                json_response["error_code"] = ErrorCode::kInvalidParameterValue;
                json_response["error_message"] = "Optimize options should be key value pairs!";
                http_status = HTTPStatus::CODE_500;
                return ResponseFactory::createResponse(http_status, json_response.dump());
            }
            for (auto option : val.get_object()) {
                const std::string key = std::string((std::string_view)option.unescaped_key());
                const std::string value = option.value().get<std::string>();
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
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto database_name = request->getPathVariable("database_name");
        auto table_name = request->getPathVariable("table_name");

        std::string data_body = request->readBodyToString();
        simdjson::padded_string json_pad(data_body);
        simdjson::parser parser;
        simdjson::document doc = parser.iterate(json_pad);

        nlohmann::json json_response;
        json_response["error_code"] = 0;
        HTTPStatus http_status;
        http_status = HTTPStatus::CODE_200;

        std::vector<ColumnDef *> column_def_ptrs;
        DeferFn defer_free_column_def_ptrs([&]() {
            for (auto &column_def_ptr : column_def_ptrs) {
                delete column_def_ptr;
            }
        });
        if (infinity::Status status = ParseColumnDefs(doc["fields"].raw_json(), column_def_ptrs); !status.ok()) {
            json_response["error_code"] = status.code();
            json_response["error_message"] = status.message();
            http_status = HTTPStatus::CODE_500;
            return ResponseFactory::createResponse(http_status, json_response.dump());
        }
        std::vector<std::shared_ptr<ColumnDef>> column_defs;
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
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto database_name = request->getPathVariable("database_name");
        auto table_name = request->getPathVariable("table_name");

        std::string data_body = request->readBodyToString();
        simdjson::padded_string json_pad(data_body);
        simdjson::parser parser;
        simdjson::document doc = parser.iterate(json_pad);

        nlohmann::json json_response;
        json_response["error_code"] = 0;
        HTTPStatus http_status;
        http_status = HTTPStatus::CODE_200;

        std::vector<std::string> column_names;
        for (auto column : doc["column_names"].get_array()) {
            column_names.emplace_back(column.get<std::string>());
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
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto database_name = request->getPathVariable("database_name");
        auto table_name = request->getPathVariable("table_name");
        auto segment_id = std::strtoll(request->getPathVariable("segment_id").get()->c_str(), nullptr, 0);
        auto result = infinity->ShowSegment(database_name, table_name, segment_id);

        HTTPStatus http_status;
        nlohmann::json json_response;

        if (result.IsOk()) {

            size_t block_rows = result.result_table_->DataBlockCount();
            auto column_cnt = result.result_table_->ColumnCount();
            for (size_t block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                for (int row = 0; row < row_count; ++row) {
                    for (size_t col = 0; col < column_cnt; ++col) {
                        const std::string &column_name = result.result_table_->GetColumnNameById(col);
                        Value value = data_block->GetValue(col, row);
                        const std::string &column_value = value.ToString();
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
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto database_name = request->getPathVariable("database_name");
        auto table_name = request->getPathVariable("table_name");
        auto result = infinity->ShowSegments(database_name, table_name);

        HTTPStatus http_status;
        nlohmann::json json_response;

        if (result.IsOk()) {

            size_t block_rows = result.result_table_->DataBlockCount();
            auto column_cnt = result.result_table_->ColumnCount();
            for (size_t block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                for (int row = 0; row < row_count; ++row) {

                    nlohmann::json json_segment;
                    for (size_t col = 0; col < column_cnt; ++col) {
                        const std::string &column_name = result.result_table_->GetColumnNameById(col);
                        Value value = data_block->GetValue(col, row);
                        const std::string &column_value = value.ToString();
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
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto database_name = request->getPathVariable("database_name");
        auto table_name = request->getPathVariable("table_name");
        auto segment_id = std::strtoll(request->getPathVariable("segment_id").get()->c_str(), nullptr, 0);
        auto result = infinity->ShowBlocks(database_name, table_name, segment_id);

        HTTPStatus http_status;
        nlohmann::json json_response;

        if (result.IsOk()) {

            size_t block_rows = result.result_table_->DataBlockCount();
            auto column_cnt = result.result_table_->ColumnCount();
            for (size_t block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                for (int row = 0; row < row_count; ++row) {

                    nlohmann::json json_block;
                    for (size_t col = 0; col < column_cnt; ++col) {
                        const std::string &column_name = result.result_table_->GetColumnNameById(col);
                        Value value = data_block->GetValue(col, row);
                        const std::string &column_value = value.ToString();
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
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
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
            size_t block_rows = result.result_table_->DataBlockCount();
            auto column_cnt = result.result_table_->ColumnCount();
            for (size_t block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                for (int row = 0; row < row_count; ++row) {
                    for (size_t col = 0; col < column_cnt; ++col) {
                        const std::string &column_name = result.result_table_->GetColumnNameById(col);
                        Value value = data_block->GetValue(col, row);
                        const std::string &column_value = value.ToString();
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
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
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
            size_t block_rows = result.result_table_->DataBlockCount();
            for (size_t block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                auto column_cnt = result.result_table_->ColumnCount();
                for (int row = 0; row < row_count; ++row) {
                    nlohmann::json json_table;
                    for (size_t col = 0; col < column_cnt; ++col) {
                        const std::string &column_name = result.result_table_->GetColumnNameById(col);
                        Value value = data_block->GetValue(col, row);
                        const std::string &column_value = value.ToString();
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
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
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
                const std::string &config_name = name_value.ToString();
                // config value
                Value value = data_block->GetValue(1, row);
                const std::string &config_value = value.ToString();
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
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
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
            if (value.type().type() == LogicalType::kVarchar) {
                json_response[config_name] = value.GetVarchar();
            } else if (value.type().type() == LogicalType::kBigInt) {
                json_response[config_name] = value.value_.big_int;
            } else if (value.type().type() == LogicalType::kBoolean) {
                json_response[config_name] = value.value_.boolean;
            } else if (value.type().type() == LogicalType::kDouble) {
                json_response[config_name] = value.value_.float64;
            } else {
                UnrecoverableError("ShowConfig: unsupported config value type.");
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

class ShowGlobalVariablesHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto result = infinity->ShowVariables(SetScope::kGlobal);

        nlohmann::json json_response;
        HTTPStatus http_status;

        if (result.IsOk()) {
            json_response["error_code"] = 0;
            DataBlock *data_block =
                result.result_table_->GetDataBlockById(0).get(); // Assume the variables output data only included in one data block
            auto row_count = data_block->row_count();
            for (int row = 0; row < row_count; ++row) {
                // variable name
                Value name_value = data_block->GetValue(0, row);
                const std::string &config_name = name_value.ToString();
                // variable value
                Value value = data_block->GetValue(1, row);
                const std::string &config_value = value.ToString();
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
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
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
            const std::string &variable_value = value.ToString();
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
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        nlohmann::json json_response;
        HTTPStatus http_status;
        QueryResult result;

        std::string data_body = request->readBodyToString();
        try {
            simdjson::padded_string json_pad(data_body);
            simdjson::parser parser;
            simdjson::document doc = parser.iterate(json_pad);
            if (doc.count_fields() != 1) {
                json_response["error_code"] = 3076;
                json_response["error_message"] = "No variable will be set";
                http_status = HTTPStatus::CODE_500;
                return ResponseFactory::createResponse(http_status, json_response.dump());
            }

            for (auto set_variable : doc.get_object()) {
                std::string var_name = std::string((std::string_view)set_variable.unescaped_key());
                auto var_value = set_variable.value();
                switch (var_value.type()) {
                    case simdjson::json_type::boolean: {
                        bool bool_value = var_value.get<bool>();
                        result = infinity->SetVariableOrConfig(var_name, bool_value, SetScope::kGlobal);
                        break;
                    }
                    case simdjson::json_type::number: {
                        switch (var_value.get_number_type()) {
                            case simdjson::number_type::signed_integer: {
                                i64 integer_value = var_value.get<i64>();
                                result = infinity->SetVariableOrConfig(var_name, integer_value, SetScope::kGlobal);
                                break;
                            }
                            case simdjson::number_type::unsigned_integer: {
                                i64 integer_value = var_value.get<u64>();
                                result = infinity->SetVariableOrConfig(var_name, integer_value, SetScope::kGlobal);
                                break;
                            }
                            case simdjson::number_type::floating_point_number: {
                                f64 double_value = var_value.get<f64>();
                                result = infinity->SetVariableOrConfig(var_name, double_value, SetScope::kGlobal);
                                break;
                            }
                            default: {
                                json_response["error_code"] = ErrorCode::kInvalidExpression;
                                json_response["error_message"] = "Invalid set variable expression";
                                return ResponseFactory::createResponse(http_status, json_response.dump());
                            }
                        }
                    }
                    case simdjson::json_type::string: {
                        std::string str_value = var_value.get<std::string>();
                        result = infinity->SetVariableOrConfig(var_name, str_value, SetScope::kGlobal);
                        break;
                    }
                    default: {
                        json_response["error_code"] = ErrorCode::kInvalidExpression;
                        json_response["error_message"] = "Invalid set variable expression";
                        return ResponseFactory::createResponse(http_status, json_response.dump());
                    }
                }
            }
        } catch (simdjson::simdjson_error &e) {
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

class ShowSessionVariablesHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto result = infinity->ShowVariables(SetScope::kSession);

        nlohmann::json json_response;
        HTTPStatus http_status;

        if (result.IsOk()) {
            json_response["error_code"] = 0;
            DataBlock *data_block =
                result.result_table_->GetDataBlockById(0).get(); // Assume the variables output data only included in one data block
            auto row_count = data_block->row_count();
            for (int row = 0; row < row_count; ++row) {
                // variable name
                Value name_value = data_block->GetValue(0, row);
                const std::string &config_name = name_value.ToString();
                // variable value
                Value value = data_block->GetValue(1, row);
                const std::string &config_value = value.ToString();
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

class ShowSessionVariableHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto variable_name = request->getPathVariable("variable_name");
        auto result = infinity->ShowVariable(variable_name, SetScope::kSession);

        nlohmann::json json_response;
        HTTPStatus http_status;

        if (result.IsOk()) {
            json_response["error_code"] = 0;
            DataBlock *data_block = result.result_table_->GetDataBlockById(0).get();
            Value value = data_block->GetValue(0, 0);
            const std::string &variable_value = value.ToString();
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

class SetSessionVariableHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        nlohmann::json json_response;
        HTTPStatus http_status;
        QueryResult result;

        std::string data_body = request->readBodyToString();
        try {
            simdjson::padded_string json_pad(data_body);
            simdjson::parser parser;
            simdjson::document doc = parser.iterate(json_pad);
            if (doc.count_fields() != 1) {
                json_response["error_code"] = 3076;
                json_response["error_message"] = "No variable will be set";
                http_status = HTTPStatus::CODE_500;
                return ResponseFactory::createResponse(http_status, json_response.dump());
            }

            for (auto set_variable : doc.get_object()) {
                auto var_name = std::string(static_cast<std::string_view>(set_variable.unescaped_key()));
                auto var_value = set_variable.value();
                switch (var_value.type()) {
                    case simdjson::json_type::boolean: {
                        bool bool_value = var_value.get<bool>();
                        result = infinity->SetVariableOrConfig(var_name, bool_value, SetScope::kSession);
                        break;
                    }
                    case simdjson::json_type::number: {
                        switch (var_value.get_number_type()) {
                            case simdjson::number_type::signed_integer: {
                                i64 integer_value = var_value.get<i64>();
                                result = infinity->SetVariableOrConfig(var_name, integer_value, SetScope::kSession);
                                break;
                            }
                            case simdjson::number_type::unsigned_integer: {
                                i64 integer_value = var_value.get<u64>();
                                result = infinity->SetVariableOrConfig(var_name, integer_value, SetScope::kSession);
                                break;
                            }
                            case simdjson::number_type::floating_point_number: {
                                f64 double_value = var_value.get<f64>();
                                result = infinity->SetVariableOrConfig(var_name, double_value, SetScope::kSession);
                                break;
                            }
                            default: {
                                json_response["error_code"] = ErrorCode::kInvalidExpression;
                                json_response["error_message"] = "Invalid set variable expression";
                                return ResponseFactory::createResponse(http_status, json_response.dump());
                            }
                        }
                    }
                    case simdjson::json_type::string: {
                        std::string str_value = var_value.get<std::string>();
                        result = infinity->SetVariableOrConfig(var_name, str_value, SetScope::kSession);
                        break;
                    }
                    default: {
                        json_response["error_code"] = ErrorCode::kInvalidExpression;
                        json_response["error_message"] = "Invalid set variable expression";
                        return ResponseFactory::createResponse(http_status, json_response.dump());
                    }
                }
            }
        } catch (simdjson::simdjson_error &e) {
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
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        nlohmann::json json_response;
        HTTPStatus http_status;
        QueryResult result;

        std::string data_body = request->readBodyToString();
        try {
            simdjson::padded_string json_pad(data_body);
            simdjson::parser parser;
            simdjson::document doc = parser.iterate(json_pad);
            if (doc.count_fields() != 1) {
                json_response["error_code"] = 3076;
                json_response["error_message"] = "No config will be set";
                http_status = HTTPStatus::CODE_500;
                return ResponseFactory::createResponse(http_status, json_response.dump());
            }

            for (auto set_config : doc.get_object()) {
                auto config_name = std::string(static_cast<std::string_view>(set_config.unescaped_key()));
                auto config_value = set_config.value();
                switch (config_value.type()) {
                    case simdjson::json_type::boolean: {
                        bool bool_value = config_value.get<bool>();
                        result = infinity->SetVariableOrConfig(config_name, bool_value, SetScope::kConfig);
                        break;
                    }
                    case simdjson::json_type::number: {
                        switch (config_value.get_number_type()) {
                            case simdjson::number_type::signed_integer: {
                                i64 integer_value = config_value.get<i64>();
                                result = infinity->SetVariableOrConfig(config_name, integer_value, SetScope::kConfig);
                                break;
                            }
                            case simdjson::number_type::unsigned_integer: {
                                i64 integer_value = config_value.get<u64>();
                                result = infinity->SetVariableOrConfig(config_name, integer_value, SetScope::kConfig);
                                break;
                            }
                            case simdjson::number_type::floating_point_number: {
                                f64 double_value = config_value.get<f64>();
                                result = infinity->SetVariableOrConfig(config_name, double_value, SetScope::kConfig);
                                break;
                            }
                            default: {
                                json_response["error_code"] = ErrorCode::kInvalidExpression;
                                json_response["error_message"] = "Invalid set config expression";
                                return ResponseFactory::createResponse(http_status, json_response.dump());
                            }
                        }
                    }
                    case simdjson::json_type::string: {
                        std::string str_value = config_value.get<std::string>();
                        result = infinity->SetVariableOrConfig(config_name, str_value, SetScope::kConfig);
                        break;
                    }
                    default: {
                        json_response["error_code"] = ErrorCode::kInvalidExpression;
                        json_response["error_message"] = "Invalid set config expression";
                        return ResponseFactory::createResponse(http_status, json_response.dump());
                    }
                }
            }
        } catch (simdjson::simdjson_error &e) {
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

class ShowBufferHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        nlohmann::json json_response;
        HTTPStatus http_status;
        QueryResult result = infinity->Query("show buffer");

        if (result.IsOk()) {
            size_t block_rows = result.result_table_->DataBlockCount();
            for (size_t block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                auto column_cnt = result.result_table_->ColumnCount();
                for (int row = 0; row < row_count; ++row) {
                    nlohmann::json json_table;
                    for (size_t col = 0; col < column_cnt; ++col) {
                        const std::string &column_name = result.result_table_->GetColumnNameById(col);
                        Value value = data_block->GetValue(col, row);
                        const std::string &column_value = value.ToString();
                        json_table[column_name] = column_value;
                    }
                    json_response["buffer"].push_back(json_table);
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

class ShowProfilesHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        nlohmann::json json_response;
        HTTPStatus http_status;
        QueryResult result = infinity->Query("show profiles");

        if (result.IsOk()) {
            size_t block_rows = result.result_table_->DataBlockCount();
            for (size_t block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                auto column_cnt = result.result_table_->ColumnCount();
                for (int row = 0; row < row_count; ++row) {
                    nlohmann::json json_table;
                    for (size_t col = 0; col < column_cnt; ++col) {
                        const std::string &column_name = result.result_table_->GetColumnNameById(col);
                        Value value = data_block->GetValue(col, row);
                        const std::string &column_value = value.ToString();
                        json_table[column_name] = column_value;
                    }
                    json_response["profiles"].push_back(json_table);
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

class ShowMemIndexHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        nlohmann::json json_response;
        HTTPStatus http_status;
        QueryResult result = infinity->Query("show memindex");

        if (result.IsOk()) {
            size_t block_rows = result.result_table_->DataBlockCount();
            for (size_t block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                auto column_cnt = result.result_table_->ColumnCount();
                for (int row = 0; row < row_count; ++row) {
                    nlohmann::json json_table;
                    for (size_t col = 0; col < column_cnt; ++col) {
                        const std::string &column_name = result.result_table_->GetColumnNameById(col);
                        Value value = data_block->GetValue(col, row);
                        const std::string &column_value = value.ToString();
                        json_table[column_name] = column_value;
                    }
                    json_response["index"].push_back(json_table);
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

class ShowQueriesHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        nlohmann::json json_response;
        HTTPStatus http_status;
        QueryResult result = infinity->Query("show queries");

        if (result.IsOk()) {
            size_t block_rows = result.result_table_->DataBlockCount();
            for (size_t block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                auto column_cnt = result.result_table_->ColumnCount();
                for (int row = 0; row < row_count; ++row) {
                    nlohmann::json json_table;
                    for (size_t col = 0; col < column_cnt; ++col) {
                        const std::string &column_name = result.result_table_->GetColumnNameById(col);
                        Value value = data_block->GetValue(col, row);
                        const std::string &column_value = value.ToString();
                        json_table[column_name] = column_value;
                    }
                    json_response["queries"].push_back(json_table);
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

class ShowLogsHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        nlohmann::json json_response;
        HTTPStatus http_status;
        QueryResult result = infinity->ShowLogs();

        if (result.IsOk()) {
            size_t block_rows = result.result_table_->DataBlockCount();
            for (size_t block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                auto column_cnt = result.result_table_->ColumnCount();
                for (int row = 0; row < row_count; ++row) {
                    nlohmann::json json_table;
                    for (size_t col = 0; col < column_cnt; ++col) {
                        const std::string &column_name = result.result_table_->GetColumnNameById(col);
                        Value value = data_block->GetValue(col, row);
                        const std::string &column_value = value.ToString();
                        json_table[column_name] = column_value;
                    }
                    json_response["logs"].push_back(json_table);
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

class ShowQueryHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        nlohmann::json json_response;
        nlohmann::json json_table;
        HTTPStatus http_status;
        std::string query_id = request->getPathVariable("query_id");
        QueryResult result = infinity->Query(fmt::format("show query {}", query_id));

        if (result.IsOk()) {
            DataBlock *data_block = result.result_table_->GetDataBlockById(0).get();
            auto column_cnt = result.result_table_->ColumnCount();
            for (size_t col = 0; col < column_cnt; ++col) {
                const std::string &column_name = result.result_table_->GetColumnNameById(col);
                Value value = data_block->GetValue(col, 0);
                const std::string &column_value = value.ToString();
                json_table[column_name] = column_value;
            }
            json_response["query"] = json_table;
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

class ShowTransactionsHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        nlohmann::json json_response;
        HTTPStatus http_status;
        QueryResult result = infinity->Query("show transactions");

        if (result.IsOk()) {
            size_t block_rows = result.result_table_->DataBlockCount();
            for (size_t block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                auto column_cnt = result.result_table_->ColumnCount();
                for (int row = 0; row < row_count; ++row) {
                    nlohmann::json json_table;
                    for (size_t col = 0; col < column_cnt; ++col) {
                        const std::string &column_name = result.result_table_->GetColumnNameById(col);
                        Value value = data_block->GetValue(col, row);
                        const std::string &column_value = value.ToString();
                        json_table[column_name] = column_value;
                    }
                    json_response["transactions"].push_back(json_table);
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

class ShowTransactionHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        nlohmann::json json_response;
        nlohmann::json json_table;
        HTTPStatus http_status;
        std::string transaction_id = request->getPathVariable("transaction_id");
        QueryResult result = infinity->Query(fmt::format("show transaction {}", transaction_id));

        if (result.IsOk()) {
            DataBlock *data_block = result.result_table_->GetDataBlockById(0).get();
            auto column_cnt = result.result_table_->ColumnCount();
            for (size_t col = 0; col < column_cnt; ++col) {
                const std::string &column_name = result.result_table_->GetColumnNameById(col);
                Value value = data_block->GetValue(col, 0);
                const std::string &column_value = value.ToString();
                json_table[column_name] = column_value;
            }
            json_response["error_code"] = 0;
            json_response["transaction"] = json_table;
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }

        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class ShowObjectsHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        nlohmann::json json_response;
        HTTPStatus http_status;
        QueryResult result = infinity->ShowObjects();

        if (result.IsOk()) {
            size_t block_rows = result.result_table_->DataBlockCount();
            for (size_t block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                auto column_cnt = result.result_table_->ColumnCount();
                for (int row = 0; row < row_count; ++row) {
                    nlohmann::json json_table;
                    for (size_t col = 0; col < column_cnt; ++col) {
                        const std::string &column_name = result.result_table_->GetColumnNameById(col);
                        Value value = data_block->GetValue(col, row);
                        const std::string &column_value = value.ToString();
                        json_table[column_name] = column_value;
                    }
                    json_response["objects"].push_back(json_table);
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

class ShowObjectHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        nlohmann::json json_response;
        nlohmann::json json_table;
        HTTPStatus http_status;
        std::string object_name = request->getPathVariable("object_name");
        QueryResult result = infinity->ShowObject(object_name);

        if (result.IsOk()) {
            DataBlock *data_block = result.result_table_->GetDataBlockById(0).get();
            auto column_cnt = result.result_table_->ColumnCount();
            for (size_t col = 0; col < column_cnt; ++col) {
                const std::string &column_name = result.result_table_->GetColumnNameById(col);
                Value value = data_block->GetValue(col, 0);
                const std::string &column_value = value.ToString();
                json_table[column_name] = column_value;
            }
            json_response["error_code"] = 0;
            json_response["object"] = json_table;
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }

        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class ShowFilesHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        nlohmann::json json_response;
        HTTPStatus http_status;
        QueryResult result = infinity->ShowFilesInObject();

        if (result.IsOk()) {
            size_t block_rows = result.result_table_->DataBlockCount();
            for (size_t block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                auto column_cnt = result.result_table_->ColumnCount();
                for (int row = 0; row < row_count; ++row) {
                    nlohmann::json json_table;
                    for (size_t col = 0; col < column_cnt; ++col) {
                        const std::string &column_name = result.result_table_->GetColumnNameById(col);
                        Value value = data_block->GetValue(col, row);
                        const std::string &column_value = value.ToString();
                        json_table[column_name] = column_value;
                    }
                    json_response["files"].push_back(json_table);
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

class ShowMemoryHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        nlohmann::json json_response;
        HTTPStatus http_status;
        QueryResult result = infinity->ShowMemory();

        if (result.IsOk()) {
            json_response["error_code"] = 0;

            size_t block_rows = result.result_table_->DataBlockCount();
            for (size_t block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                size_t row_count = data_block->row_count();
                for (size_t row = 0; row < row_count; row++) {
                    // config name
                    Value name_value = data_block->GetValue(0, row);
                    const std::string &config_name = name_value.ToString();
                    // config value
                    Value value = data_block->GetValue(1, row);
                    const std::string &config_value = value.ToString();
                    json_response[config_name] = config_value;
                }
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

class ShowMemoryObjectsHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        nlohmann::json json_response;
        HTTPStatus http_status;

        QueryResult result = infinity->ShowMemoryObjects();
        if (result.IsOk()) {
            size_t block_rows = result.result_table_->DataBlockCount();
            for (size_t block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                auto column_cnt = result.result_table_->ColumnCount();
                for (int row = 0; row < row_count; ++row) {
                    nlohmann::json json_table;
                    for (size_t col = 0; col < column_cnt; ++col) {
                        const std::string &column_name = result.result_table_->GetColumnNameById(col);
                        Value value = data_block->GetValue(col, row);
                        const std::string &column_value = value.ToString();
                        json_table[column_name] = column_value;
                    }
                    json_response["memory_objects"].push_back(json_table);
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

class ShowMemoryAllocationsHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        nlohmann::json json_response;
        HTTPStatus http_status;
        QueryResult result = infinity->ShowMemoryAllocations();
        if (result.IsOk()) {
            size_t block_rows = result.result_table_->DataBlockCount();
            for (size_t block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                auto column_cnt = result.result_table_->ColumnCount();
                for (int row = 0; row < row_count; ++row) {
                    nlohmann::json json_table;
                    for (size_t col = 0; col < column_cnt; ++col) {
                        const std::string &column_name = result.result_table_->GetColumnNameById(col);
                        Value value = data_block->GetValue(col, row);
                        const std::string &column_value = value.ToString();
                        json_table[column_name] = column_value;
                    }
                    json_response["memory_allocations"].push_back(json_table);
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

class ForceGlobalCheckpointHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        nlohmann::json json_response;
        HTTPStatus http_status;
        QueryResult result = infinity->ForceCheckpoint();

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

class CompactTableHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        std::string db_name = request->getPathVariable("database_name");
        std::string table_name = request->getPathVariable("table_name");

        nlohmann::json json_response;
        HTTPStatus http_status;
        QueryResult result = infinity->CompactTable(db_name, table_name);

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

class CreateTableSnapshotHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        std::string data_body = request->readBodyToString();
        simdjson::padded_string json_pad(data_body);
        simdjson::parser parser;
        simdjson::document doc = parser.iterate(json_pad);
        std::string db_name = doc["db_name"].get<std::string>();
        std::string table_name = doc["table_name"].get<std::string>();
        std::string snapshot_name = doc["snapshot_name"].get<std::string>();

        nlohmann::json json_response;
        HTTPStatus http_status;
        QueryResult result = infinity->CreateTableSnapshot(db_name, table_name, snapshot_name);

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

class RestoreTableSnapshotHandler final : public HttpRequestHandler {

    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        std::string data_body = request->readBodyToString();
        simdjson::padded_string json_pad(data_body);
        simdjson::parser parser;
        simdjson::document doc = parser.iterate(json_pad);
        std::string snapshot_name = doc["snapshot_name"].get<std::string>();

        nlohmann::json json_response;
        HTTPStatus http_status;
        QueryResult result = infinity->RestoreTableSnapshot(snapshot_name);

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

class DropSnapshotHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        // Get snapshot name from URL path parameter
        std::string snapshot_name = request->getPathVariable("snapshot_name");

        nlohmann::json json_response;
        HTTPStatus http_status;
        QueryResult result = infinity->DropSnapshot(snapshot_name);

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

class ListSnapshotsHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        nlohmann::json json_response;
        HTTPStatus http_status;

        // Call Infinity API to list snapshots
        QueryResult result = infinity->ListSnapshots();

        if (result.IsOk()) {
            json_response["error_code"] = 0;
            json_response["error_message"] = "";

            // Convert QueryResult to JSON format
            nlohmann::json snapshots_array = nlohmann::json::array();

            // Get the result table from QueryResult
            auto result_table = result.result_table_;
            if (result_table != nullptr) {
                size_t blocks_count = result_table->DataBlockCount();

                for (size_t block_idx = 0; block_idx < blocks_count; ++block_idx) {
                    auto data_block = result_table->GetDataBlockById(block_idx);
                    size_t row_count = data_block->row_count();

                    for (size_t row_idx = 0; row_idx < row_count; ++row_idx) {
                        nlohmann::json snapshot_obj;

                        // Extract snapshot name (column 0)
                        auto &name_column = data_block->column_vectors[0];
                        if (name_column->data_type()->type() == LogicalType::kVarchar) {
                            auto varchar_value = name_column->GetValueByIndex(row_idx);
                            snapshot_obj["name"] = varchar_value.GetVarchar();
                        }

                        // Extract scope (column 1)
                        auto &scope_column = data_block->column_vectors[1];
                        if (scope_column->data_type()->type() == LogicalType::kVarchar) {
                            auto scope_value = scope_column->GetValueByIndex(row_idx);
                            snapshot_obj["scope"] = scope_value.GetVarchar();
                        }

                        // Extract create time (column 2)
                        auto &time_column = data_block->column_vectors[2];
                        if (time_column->data_type()->type() == LogicalType::kVarchar) {
                            auto time_value = time_column->GetValueByIndex(row_idx);
                            snapshot_obj["time"] = time_value.GetVarchar();
                        }

                        // Extract commit timestamp (column 3)
                        auto &commit_column = data_block->column_vectors[3];
                        if (commit_column->data_type()->type() == LogicalType::kBigInt) {
                            auto commit_value = commit_column->GetValueByIndex(row_idx);
                            snapshot_obj["commit"] = commit_value.GetValue<BigIntT>();
                        }

                        // Extract size (column 4)
                        auto &size_column = data_block->column_vectors[4];
                        if (size_column->data_type()->type() == LogicalType::kVarchar) {
                            auto size_value = size_column->GetValueByIndex(row_idx);
                            snapshot_obj["size"] = size_value.GetVarchar();
                        }

                        snapshots_array.push_back(snapshot_obj);
                    }
                }
            }

            json_response["snapshots"] = snapshots_array;
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }

        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class ShowSnapshotHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        nlohmann::json json_response;
        HTTPStatus http_status;

        // Get snapshot name from path
        std::string snapshot_name = request->getPathVariable("snapshot_name");
        // Validate snapshot name
        if (snapshot_name.empty()) {
            json_response["error_code"] = 3001;
            json_response["error_message"] = "Snapshot name is required";
            http_status = HTTPStatus::CODE_400;
            return ResponseFactory::createResponse(http_status, json_response.dump());
        }

        // Call Infinity API to show snapshot details
        QueryResult result = infinity->ShowSnapshot(snapshot_name);

        if (result.IsOk()) {
            json_response["error_code"] = 0;
            json_response["error_message"] = "";

            // Convert QueryResult to JSON format
            nlohmann::json snapshot_obj;

            // Get the result table from QueryResult
            auto result_table = result.result_table_;
            if (result_table != nullptr) {
                size_t blocks_count = result_table->DataBlockCount();
                // The ShowSnapshot result has key-value pairs in alternating rows
                // We need to extract the values for each field
                std::string snapshot_name_value, scope, create_time, size;
                i64 commit_ts = 0;

                for (size_t block_idx = 0; block_idx < blocks_count; ++block_idx) {
                    auto data_block = result_table->GetDataBlockById(block_idx);
                    size_t row_count = data_block->row_count();

                    for (size_t row_idx = 0; row_idx < row_count; row_idx += 2) {
                        // Each pair consists of a key row and a value row
                        if (row_idx + 1 < row_count) {
                            auto &key_column = data_block->column_vectors[0];
                            auto &value_column = data_block->column_vectors[1];

                            if (key_column->data_type()->type() == LogicalType::kVarchar &&
                                value_column->data_type()->type() == LogicalType::kVarchar) {

                                auto key_value = key_column->GetValueByIndex(row_idx);
                                auto value_value = value_column->GetValueByIndex(row_idx + 1);

                                std::string key = key_value.GetVarchar();
                                std::string value = value_value.GetVarchar();
                                if (key == "snapshot_name") {
                                    snapshot_name_value = value;
                                } else if (key == "snapshot_scope") {
                                    scope = value;
                                } else if (key == "create_time") {
                                    create_time = value;
                                } else if (key == "commit_timestamp") {
                                    commit_ts = std::stoll(value);
                                } else if (key == "snapshot_size") {
                                    size = value;
                                }
                            }
                        }
                    }
                }

                // Fill the JSON object
                snapshot_obj["name"] = snapshot_name_value;
                snapshot_obj["scope"] = scope;
                snapshot_obj["time"] = create_time;
                snapshot_obj["commit"] = commit_ts;
                snapshot_obj["size"] = size;
            }

            json_response["snapshot"] = snapshot_obj;
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }

        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class CreateSystemSnapshotHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        std::string data_body = request->readBodyToString();
        simdjson::padded_string json_pad(data_body);
        simdjson::parser parser;
        simdjson::document doc = parser.iterate(json_pad);
        std::string snapshot_name = doc["snapshot_name"].get<std::string>();

        nlohmann::json json_response;
        HTTPStatus http_status;
        QueryResult result = infinity->CreateSystemSnapshot(snapshot_name);

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

class RestoreSystemSnapshotHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        std::string data_body = request->readBodyToString();
        simdjson::padded_string json_pad(data_body);
        simdjson::parser parser;
        simdjson::document doc = parser.iterate(json_pad);
        std::string snapshot_name = doc["snapshot_name"].get<std::string>();

        nlohmann::json json_response;
        HTTPStatus http_status;
        QueryResult result = infinity->RestoreSystemSnapshot(snapshot_name);

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

class CreateDatabaseSnapshotHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        std::string data_body = request->readBodyToString();
        simdjson::padded_string json_pad(data_body);
        simdjson::parser parser;
        simdjson::document doc = parser.iterate(json_pad);
        std::string snapshot_name = doc["snapshot_name"].get<std::string>();
        std::string db_name = doc["db_name"].get<std::string>();
        nlohmann::json json_response;
        HTTPStatus http_status;
        QueryResult result = infinity->CreateDatabaseSnapshot(snapshot_name, db_name);

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

class RestoreDatabaseSnapshotHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        std::string data_body = request->readBodyToString();
        simdjson::padded_string json_pad(data_body);
        simdjson::parser parser;
        simdjson::document doc = parser.iterate(json_pad);
        std::string snapshot_name = doc["snapshot_name"].get<std::string>();

        nlohmann::json json_response;
        HTTPStatus http_status;
        QueryResult result = infinity->RestoreDatabaseSnapshot(snapshot_name);

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

class AdminShowCurrentNodeHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {

        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto result = infinity->AdminShowCurrentNode();

        nlohmann::json json_response;
        nlohmann::json node_info;
        HTTPStatus http_status;

        if (result.IsOk()) {
            size_t block_rows = result.result_table_->DataBlockCount();
            for (size_t block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                for (int row = 0; row < row_count; ++row) {
                    // variable name
                    const std::string &attrib_name = data_block->GetValue(0, row).ToString();
                    // variable value
                    const std::string &attrib_value = data_block->GetValue(1, row).ToString();
                    node_info[attrib_name] = attrib_value;
                }
            }
            http_status = HTTPStatus::CODE_200;
            json_response["error_code"] = ErrorCode::kOk;
            json_response["node"] = node_info;

        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class AdminShowNodeByNameHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        nlohmann::json json_response;
        nlohmann::json node_info;
        HTTPStatus http_status;

        std::string node_name = request->getPathVariable("node_name");
        QueryResult result = infinity->AdminShowNode(node_name);

        if (result.IsOk()) {
            size_t block_rows = result.result_table_->DataBlockCount();
            for (size_t block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                for (int row = 0; row < row_count; ++row) {
                    // variable name
                    const std::string &attrib_name = data_block->GetValue(0, row).ToString();
                    // variable value
                    const std::string &attrib_value = data_block->GetValue(1, row).ToString();
                    node_info[attrib_name] = attrib_value;
                }
            }
            http_status = HTTPStatus::CODE_200;
            json_response["node"] = node_info;
            json_response["error_code"] = ErrorCode::kOk;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }

        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class AdminListAllNodesHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        HTTPStatus http_status;
        nlohmann::json json_response;
        nlohmann::json nodes_json;

        QueryResult result = infinity->AdminShowNodes();
        if (result.IsOk()) {
            size_t block_rows = result.result_table_->DataBlockCount();
            for (size_t block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                for (int row = 0; row < row_count; ++row) {
                    nlohmann::json node_json;
                    {
                        std::string node_name = data_block->GetValue(0, row).ToString();
                        node_json["name"] = node_name;
                    }
                    {
                        std::string node_role = data_block->GetValue(1, row).ToString();
                        node_json["role"] = node_role;
                    }
                    {
                        std::string node_status = data_block->GetValue(2, row).ToString();
                        node_json["status"] = node_status;
                    }
                    {
                        std::string node_address = data_block->GetValue(3, row).ToString();
                        node_json["address"] = node_address;
                    }
                    {
                        std::string last_update = data_block->GetValue(4, row).ToString();
                        node_json["last_update"] = last_update;
                    }
                    {
                        std::string heartbeat = data_block->GetValue(5, row).ToString();
                        node_json["heartbeat"] = heartbeat;
                    }
                    nodes_json.push_back(node_json);
                }
            }
            http_status = HTTPStatus::CODE_200;
            json_response["error_code"] = ErrorCode::kOk;
            json_response["nodes"] = nodes_json;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class AdminSetNodeRoleHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        HTTPStatus http_status;
        nlohmann::json json_response;
        infinity::Status status;

        std::string data_body = request->readBodyToString();

        try {
            simdjson::padded_string json_pad(data_body);
            simdjson::parser parser;
            simdjson::document doc = parser.iterate(json_pad);

            if (simdjson::value val; doc["role"].get(val) != simdjson::SUCCESS || !val.is_string()) {
                http_status = HTTPStatus::CODE_500;
                json_response["error_code"] = ErrorCode::kInvalidCommand;
                json_response["error_message"] = "Field 'role' is required";
                return ResponseFactory::createResponse(http_status, json_response.dump());
            }

            std::string role = doc["role"].get<std::string>();
            QueryResult result;
            ToLower(role);
            if (role == "admin") {
                result = infinity->AdminSetAdmin();
            } else if (role == "standalone") {
                result = infinity->AdminSetStandalone();
            } else if (role == "leader") {
                result = infinity->AdminSetLeader(doc["name"].get<std::string>());
            } else if (role == "follower") {
                result = infinity->AdminSetFollower(doc["name"].get<std::string>(), doc["address"].get<std::string>());
            } else if (role == "learner") {
                result = infinity->AdminSetLearner(doc["name"].get<std::string>(), doc["address"].get<std::string>());
            } else {
                http_status = HTTPStatus::CODE_500;
                json_response["error_code"] = ErrorCode::kInvalidNodeRole;
                json_response["error_message"] = fmt::format("Invalid node role {}", role);
                return ResponseFactory::createResponse(http_status, json_response.dump());
            }

            if (result.IsOk()) {
                http_status = HTTPStatus::CODE_200;
                json_response["error_code"] = ErrorCode::kOk;
            } else {
                http_status = HTTPStatus::CODE_500;
                json_response["error_code"] = result.ErrorCode();
                json_response["error_message"] = result.ErrorMsg();
            }
        } catch (simdjson::simdjson_error &e) {
            http_status = HTTPStatus::CODE_500;
            json_response["error_code"] = ErrorCode::kInvalidJsonFormat;
            json_response["error_message"] = e.what();
            return ResponseFactory::createResponse(http_status, json_response.dump());
        }

        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class AdminShowNodeVariablesHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        HTTPStatus http_status;
        nlohmann::json json_response;

        auto result = infinity->AdminShowVariables();
        if (result.IsOk()) {
            json_response["error_code"] = 0;
            DataBlock *data_block =
                result.result_table_->GetDataBlockById(0).get(); // Assume the variables output data only included in one data block
            auto row_count = data_block->row_count();
            for (int row = 0; row < row_count; ++row) {
                // variable name
                Value name_value = data_block->GetValue(0, row);
                const std::string &config_name = name_value.ToString();
                // variable value
                Value value = data_block->GetValue(1, row);
                const std::string &config_value = value.ToString();
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

class AdminShowNodeConfigsHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        HTTPStatus http_status;
        nlohmann::json json_response;

        auto result = infinity->AdminShowConfigs();
        if (result.IsOk()) {
            json_response["error_code"] = 0;
            DataBlock *data_block = result.result_table_->GetDataBlockById(0).get(); // Assume the config output data only included in one data block
            auto row_count = data_block->row_count();
            for (int row = 0; row < row_count; ++row) {
                // config name
                Value name_value = data_block->GetValue(0, row);
                const std::string &config_name = name_value.ToString();
                // config value
                Value value = data_block->GetValue(1, row);
                const std::string &config_value = value.ToString();
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

class AdminShowNodeVariableHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        std::string variable_name = request->getPathVariable("variable_name");
        auto result = infinity->AdminShowVariable(variable_name);

        HTTPStatus http_status;
        nlohmann::json json_response;

        if (result.IsOk()) {
            json_response["error_code"] = 0;
            DataBlock *data_block = result.result_table_->GetDataBlockById(0).get();
            if (data_block->row_count() == 0) {
                json_response["error_code"] = ErrorCode::kNoSuchSystemVar;
                json_response["error_message"] = fmt::format("variable does not exist : {}.", variable_name);
                http_status = HTTPStatus::CODE_500;
                return ResponseFactory::createResponse(http_status, json_response.dump());
            }
            Value value = data_block->GetValue(0, 0);
            const std::string &variable_value = value.ToString();
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

class AdminShowLogsHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        HTTPStatus http_status;
        nlohmann::json json_response;
        nlohmann::json nodes_json;

        auto result = infinity->AdminShowLogs();
        if (result.IsOk()) {
            json_response["error_code"] = 0;
            DataBlock *data_block = result.result_table_->GetDataBlockById(0).get(); // Assume the config output data only included in one data block
            auto row_count = data_block->row_count();
            for (int row = 0; row < row_count; ++row) {
                nlohmann::json node_json;
                {
                    std::string index = data_block->GetValue(0, row).ToString();
                    node_json["index"] = index;
                }
                {
                    std::string filename = data_block->GetValue(1, row).ToString();
                    node_json["filename"] = filename;
                }
                {
                    std::string type = data_block->GetValue(2, row).ToString();
                    node_json["type"] = type;
                }
                nodes_json.push_back(node_json);
            }
            json_response["logs"] = nodes_json;
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }

        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class AdminRemoveNodeHandler final : public HttpRequestHandler {
public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        nlohmann::json json_response;
        HTTPStatus http_status;

        std::string node_name = request->getPathVariable("node_name");
        auto result = infinity->AdminRemoveNode(node_name);

        if (result.IsOk()) {
            json_response["error_code"] = 0;
            json_response["message"] = fmt::format("Node {} removed successfully.", node_name);
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

// oatpp example
// https://github.com/oatpp/example-server-stop/blob/master/src/App_StopSimple.cpp

std::thread HTTPServer::Start(const std::string &ip_address, u16 port) {
    {
        auto expected = HTTPServerStatus::kStopped;
        if (!status_.compare_exchange_strong(expected, HTTPServerStatus::kStarting)) {
            UnrecoverableError("HTTP server is already running.");
        }
    }
    WebEnvironment::init();

    std::shared_ptr<HttpRouter> router = HttpRouter::createShared();

    // database
    router->route("GET", "/databases", std::make_shared<ListDatabaseHandler>());
    router->route("POST", "/databases/{database_name}", std::make_shared<CreateDatabaseHandler>());
    router->route("DELETE", "/databases/{database_name}", std::make_shared<DropDatabaseHandler>());
    router->route("GET", "/databases/{database_name}", std::make_shared<ShowDatabaseHandler>());

    // table
    router->route("GET", "/databases/{database_name}/tables", std::make_shared<ListTableHandler>());
    router->route("POST", "/databases/{database_name}/tables/{table_name}", std::make_shared<CreateTableHandler>());
    router->route("POST", "/databases/{database_name}/tables/{table_name}/rename", std::make_shared<RenameTableHandler>());
    router->route("DELETE", "/databases/{database_name}/tables/{table_name}", std::make_shared<DropTableHandler>());
    router->route("GET", "/databases/{database_name}/tables/{table_name}", std::make_shared<ShowTableHandler>());
    router->route("GET", "/databases/{database_name}/table/{table_name}", std::make_shared<ExportTableHandler>()); // Export table
    router->route("GET", "/databases/{database_name}/tables/{table_name}/columns", std::make_shared<ShowTableColumnsHandler>());

    // DML
    router->route("PUT", "/databases/{database_name}/tables/{table_name}", std::make_shared<ImportHandler>());
    router->route("POST", "/databases/{database_name}/tables/{table_name}/docs", std::make_shared<InsertHandler>());
    router->route("DELETE", "/databases/{database_name}/tables/{table_name}/docs", std::make_shared<DeleteHandler>());
    router->route("PUT", "/databases/{database_name}/tables/{table_name}/docs", std::make_shared<UpdateHandler>());
    router->route("PUT", "/databases/{database_name}/tables/{table_name}/compact", std::make_shared<CompactTableHandler>());

    // DQL
    router->route("GET", "/databases/{database_name}/tables/{table_name}/docs", std::make_shared<SelectHandler>());
    router->route("GET", "/databases/{database_name}/tables/{table_name}/meta", std::make_shared<ExplainHandler>());

    // index
    router->route("GET", "/databases/{database_name}/tables/{table_name}/indexes", std::make_shared<ListTableIndexesHandler>());
    router->route("GET", "/databases/{database_name}/tables/{table_name}/indexes/{index_name}", std::make_shared<ShowTableIndexDetailHandler>());
    router->route("GET",
                  "/databases/{database_name}/tables/{table_name}/indexes/{index_name}/segment/{segment_id}",
                  std::make_shared<ShowTableIndexSegmentHandler>());
    router->route("GET",
                  "/databases/{database_name}/tables/{table_name}/indexes/{index_name}/segment/{segment_id}/chunk/{chunk_id}",
                  std::make_shared<ShowTableIndexChunkHandler>());
    router->route("DELETE", "/databases/{database_name}/tables/{table_name}/indexes/{index_name}", std::make_shared<DropIndexHandler>());
    router->route("POST", "/databases/{database_name}/tables/{table_name}/indexes/{index_name}", std::make_shared<CreateIndexHandler>());
    router->route("PUT", "/databases/{database_name}/tables/{table_name}/indexes/{index_name}", std::make_shared<OptimizeIndexHandler>());

    // alter
    router->route("POST", "/databases/{database_name}/tables/{table_name}/columns", std::make_shared<AddColumnsHandler>());
    router->route("DELETE", "/databases/{database_name}/tables/{table_name}/columns", std::make_shared<DropColumnsHandler>());

    // snapshot
    router->route("POST", "/snapshots", std::make_shared<CreateTableSnapshotHandler>());
    router->route("POST", "/snapshots/restore", std::make_shared<RestoreTableSnapshotHandler>());
    router->route("DELETE", "/snapshots/{snapshot_name}", std::make_shared<DropSnapshotHandler>());
    router->route("GET", "/snapshots", std::make_shared<ListSnapshotsHandler>());
    router->route("GET", "/snapshots/{snapshot_name}", std::make_shared<ShowSnapshotHandler>());
    router->route("POST", "/snapshots/system", std::make_shared<CreateSystemSnapshotHandler>());
    router->route("POST", "/snapshots/system/restore", std::make_shared<RestoreSystemSnapshotHandler>());
    router->route("POST", "/snapshots/database", std::make_shared<CreateDatabaseSnapshotHandler>());
    router->route("POST", "/snapshots/database/restore", std::make_shared<RestoreDatabaseSnapshotHandler>());

    // segment
    router->route("GET", "/databases/{database_name}/tables/{table_name}/segments/{segment_id}", std::make_shared<ShowSegmentDetailHandler>());
    router->route("GET", "/databases/{database_name}/tables/{table_name}/segments", std::make_shared<ShowSegmentsListHandler>());

    // block
    router->route("GET", "/databases/{database_name}/tables/{table_name}/segments/{segment_id}/blocks", std::make_shared<ShowBlocksListHandler>());
    router->route("GET",
                  "/databases/{database_name}/tables/{table_name}/segments/{segment_id}/blocks/{block_id}",
                  std::make_shared<ShowBlockDetailHandler>());
    router->route("GET",
                  "/databases/{database_name}/tables/{table_name}/segments/{segment_id}/blocks/{block_id}/{column_id}",
                  std::make_shared<ShowBlockColumnHandler>());

    // config
    router->route("GET", "/configs", std::make_shared<ShowConfigsHandler>());
    router->route("GET", "/configs/{config_name}", std::make_shared<ShowConfigHandler>());

    router->route("POST", "/configs", std::make_shared<SetConfigHandler>());

    // metrics
    router->route("GET", "/instance/buffer", std::make_shared<ShowBufferHandler>());
    router->route("GET", "/instance/profiles", std::make_shared<ShowProfilesHandler>());
    router->route("GET", "/instance/memindex", std::make_shared<ShowMemIndexHandler>());
    router->route("GET", "/instance/queries", std::make_shared<ShowQueriesHandler>());
    router->route("GET", "/instance/logs", std::make_shared<ShowLogsHandler>());
    router->route("GET", "/instance/queries/{query_id}", std::make_shared<ShowQueryHandler>());
    router->route("GET", "/instance/transactions", std::make_shared<ShowTransactionsHandler>());
    router->route("GET", "/instance/transactions/{transaction_id}", std::make_shared<ShowTransactionHandler>());
    router->route("GET", "/instance/objects", std::make_shared<ShowObjectsHandler>());
    router->route("GET", "/instance/objects/{object_name}", std::make_shared<ShowObjectHandler>());
    router->route("GET", "/instance/files", std::make_shared<ShowFilesHandler>());
    router->route("GET", "/instance/memory", std::make_shared<ShowMemoryHandler>());
    router->route("GET", "/instance/memory/objects", std::make_shared<ShowMemoryObjectsHandler>());
    router->route("GET", "/instance/memory/allocations", std::make_shared<ShowMemoryAllocationsHandler>());
    router->route("POST", "/instance/flush", std::make_shared<ForceGlobalCheckpointHandler>());

    // variable
    router->route("GET", "/variables/global", std::make_shared<ShowGlobalVariablesHandler>());
    router->route("GET", "/variables/global/{variable_name}", std::make_shared<ShowGlobalVariableHandler>());
    router->route("SET", "/variables/global", std::make_shared<SetGlobalVariableHandler>());
    router->route("GET", "/variables/session", std::make_shared<ShowSessionVariablesHandler>());
    router->route("GET", "/variables/session/{variable_name}", std::make_shared<ShowSessionVariableHandler>());
    router->route("SET", "/variables/session", std::make_shared<SetSessionVariableHandler>());

    // admin
    router->route("POST", "/admin/node/current", std::make_shared<AdminSetNodeRoleHandler>());
    router->route("GET", "/admin/variables", std::make_shared<AdminShowNodeVariablesHandler>());
    router->route("GET", "/admin/configs", std::make_shared<AdminShowNodeConfigsHandler>());
    router->route("GET", "/admin/variables/{variable_name}", std::make_shared<AdminShowNodeVariableHandler>());
    router->route("GET", "/admin/logs", std::make_shared<AdminShowLogsHandler>());
    router->route("GET", "/admin/node/current", std::make_shared<AdminShowCurrentNodeHandler>());
    router->route("GET", "/admin/node/{node_name}", std::make_shared<AdminShowNodeByNameHandler>());
    router->route("GET", "/admin/nodes", std::make_shared<AdminListAllNodesHandler>());
    router->route("DELETE", "/admin/node/{node_name}", std::make_shared<AdminRemoveNodeHandler>());

    connection_provider_ = HttpConnectionProvider::createShared({ip_address, port, WebAddress::IP_4});
    connection_handler_ = HttpConnectionHandler::createShared(router);

    server_ = std::make_shared<WebServer>(connection_provider_, connection_handler_);

    fmt::print("HTTP server listen on {}: {}\n", ip_address, port);

    status_.store(HTTPServerStatus::kRunning);

    return std::thread([this] {
        server_->run();

        status_.store(HTTPServerStatus::kStopped);
        status_.notify_one();
    });
}

void HTTPServer::Shutdown() {
    {
        auto expected = HTTPServerStatus::kRunning;
        if (!status_.compare_exchange_strong(expected, HTTPServerStatus::kStopping)) {
            if (expected == HTTPServerStatus::kStopped) {
                return;
            } else {
                UnrecoverableError(fmt::format("Http server in unexpected state: {}", u8(expected)));
            }
        }
    }

    connection_provider_->stop();
    if (server_->getStatus() == WebServer::STATUS_RUNNING) {
        server_->stop();
    }
    connection_handler_->stop();

    WebEnvironment::destroy();

    status_.wait(HTTPServerStatus::kStopping);
}

} // namespace infinity
