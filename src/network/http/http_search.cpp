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

#include <cassert>
#include <string>
#include <vector>

module http_search;

import stl;
import third_party;
import parsed_expr;
import knn_expr;
import match_expr;
import search_expr;
import match_tensor_expr;
import match_sparse_expr;
import fusion_expr;
import column_expr;
import function_expr;
import constant_expr;
import defer_op;
import expr_parser;
import expression_parser_result;
import statement_common;
import query_result;
import data_block;
import value;
import physical_import;
import explain_statement;
import internal_types;
import select_statement;
import logical_type;

namespace infinity {

void SerializeErrorCode(rapidjson::Writer<rapidjson::StringBuffer> &writer, long code) {
    writer.Key("error_code");
    writer.Int64(code);
}

void SerializeErrorCode(rapidjson::Writer<rapidjson::StringBuffer> &writer, long code, const String &message) {
    writer.Key("error_code");
    writer.Int64(code);
    writer.Key("error_message");
    writer.String(message.c_str());
}

void HTTPSearch::Process(Infinity *infinity_ptr,
                         const String &db_name,
                         const String &table_name,
                         const String &input_json_str,
                         HTTPStatus &http_status,
                         rapidjson::Writer<rapidjson::StringBuffer> &writer) {
    http_status = HTTPStatus::CODE_500;
    try {
        simdjson::padded_string json_pad(input_json_str);
        simdjson::parser parser;
        simdjson::document doc = parser.iterate(json_pad);
        if (doc.type() != simdjson::json_type::object) {
            SerializeErrorCode(writer, (long)ErrorCode::kInvalidJsonFormat, "HTTP Body isn't json object");
        }
        UniquePtr<ParsedExpr> filter{};
        UniquePtr<ParsedExpr> limit{};
        UniquePtr<ParsedExpr> offset{};
        SearchExpr *search_expr{};
        Vector<ParsedExpr *> *output_columns{nullptr};
        Vector<ParsedExpr *> *highlight_columns{nullptr};
        Vector<OrderByExpr *> *order_by_list{nullptr};
        Vector<ParsedExpr *> *group_by_columns{nullptr};
        UniquePtr<ParsedExpr> having{};
        bool total_hits_count_flag{};

        DeferFn search_fn([&]() {
            if (search_expr != nullptr) {
                delete search_expr;
                search_expr = nullptr;
            }
        });

        DeferFn defer_fn([&]() {
            if (output_columns != nullptr) {
                for (auto &expr : *output_columns) {
                    delete expr;
                }
                delete output_columns;
                output_columns = nullptr;
            }
        });

        DeferFn defer_fn_highlight([&]() {
            if (highlight_columns != nullptr) {
                for (auto &expr : *highlight_columns) {
                    delete expr;
                }
                delete highlight_columns;
                highlight_columns = nullptr;
            }
        });

        DeferFn defer_fn_order([&]() {
            if (order_by_list != nullptr) {
                for (auto &expr : *order_by_list) {
                    delete expr;
                }
                delete order_by_list;
                order_by_list = nullptr;
            }
        });

        for (auto elem : doc.get_object()) {
            String key = String((std::string_view)elem.unescaped_key());
            auto value = elem.value();
            ToLower(key);
            if (IsEqual(key, "output")) {
                if (output_columns != nullptr) {
                    SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "More than one output field.");
                    return;
                }
                if (value.type() != simdjson::json_type::array) {
                    SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "Output field should be array");
                    return;
                }

                output_columns = ParseOutput(value.raw_json(), http_status, writer);
                if (output_columns == nullptr) {
                    return;
                }
            } else if (IsEqual(key, "highlight")) {
                if (value.type() != simdjson::json_type::array) {
                    SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "Output field should be array");
                    return;
                }

                highlight_columns = ParseOutput(value.raw_json(), http_status, writer);
                if (highlight_columns == nullptr) {
                    return;
                }
            } else if (IsEqual(key, "sort")) {
                if (order_by_list != nullptr) {
                    SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "More than one sort field.");
                    return;
                }

                if (value.type() != simdjson::json_type::array) {
                    SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "Sort field should be array");
                    return;
                }

                order_by_list = ParseSort(value.raw_json(), http_status, writer);
                if (order_by_list == nullptr) {
                    return;
                }
            } else if (IsEqual(key, "group_by")) {
                if (group_by_columns != nullptr) {
                    SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "More than one group by field.");
                    return;
                }

                group_by_columns = ParseOutput(value.raw_json(), http_status, writer);
                if (group_by_columns == nullptr) {
                    return;
                }
            } else if (IsEqual(key, "having")) {
                if (having != nullptr) {
                    SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "More than one having field.");
                    return;
                }
                having = ParseFilter(value.raw_json(), http_status, writer);
                if (having == nullptr) {
                    return;
                }
            } else if (IsEqual(key, "filter")) {

                if (filter) {
                    SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "More than one filter field.");
                    return;
                }
                filter = ParseFilter(value.raw_json(), http_status, writer);
                if (!filter) {
                    return;
                }
            } else if (IsEqual(key, "limit")) {

                if (limit) {
                    SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "More than one limit field.");
                    return;
                }
                limit = ParseFilter(value.raw_json(), http_status, writer);
                if (!limit) {
                    return;
                }
            } else if (IsEqual(key, "offset")) {

                if (offset) {
                    SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "More than one offset field.");
                    return;
                }
                offset = ParseFilter(value.raw_json(), http_status, writer);
                if (!offset) {
                    return;
                }
            } else if (IsEqual(key, "search")) {
                if (search_expr) {
                    SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "More than one search field.");
                    return;
                }
                search_expr = ParseSearchExpr(value.raw_json(), http_status, writer);
                if (!search_expr) {
                    return;
                }
            } else if (IsEqual(key, "option")) {
                if (value.type() != simdjson::json_type::object) {
                    SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "Option field should be object");
                    return;
                }

                for (auto option : value.get_object()) {
                    String option_key = String((std::string_view)option.unescaped_key());
                    auto option_value = option.value();
                    ToLower(option_key);
                    if (option_key == "total_hits_count") {
                        if (option_value.is_string()) {
                            String value_str = option_value.get<String>();
                            ToLower(value);
                            if (value_str == "true") {
                                total_hits_count_flag = true;
                            } else if (value_str == "false") {
                                total_hits_count_flag = false;
                            } else {
                                SerializeErrorCode(writer,
                                                   (long)ErrorCode::kInvalidExpression,
                                                   fmt::format("Unknown search option: {}, value: {}", option_key, value_str));
                                return;
                            }
                        } else if (option_value.type() == simdjson::json_type::boolean) {
                            total_hits_count_flag = option_value.get<bool>();
                        } else {
                            SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "Invalid total hits count type");
                            return;
                        }
                    }
                }
            } else {
                SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "Unknown expression: " + key);
                return;
            }
        }

        const QueryResult result = infinity_ptr->Search(db_name,
                                                        table_name,
                                                        search_expr,
                                                        filter.release(),
                                                        limit.release(),
                                                        offset.release(),
                                                        output_columns,
                                                        highlight_columns,
                                                        order_by_list,
                                                        group_by_columns,
                                                        having.release(),
                                                        total_hits_count_flag);

        search_expr = nullptr;
        output_columns = nullptr;
        highlight_columns = nullptr;
        order_by_list = nullptr;
        group_by_columns = nullptr;
        if (result.IsOk()) {
            SizeT block_rows = result.result_table_->DataBlockCount();
            writer.Key("output");
            writer.StartArray();
            for (SizeT block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                auto column_cnt = result.result_table_->ColumnCount();

                for (int row = 0; row < row_count; ++row) {
                    writer.StartArray();
                    for (SizeT col = 0; col < column_cnt; ++col) {
                        writer.StartObject();
                        Value value = data_block->GetValue(col, row);
                        const String &column_name = result.result_table_->GetColumnNameById(col);
                        switch (value.type().type()) {
                            case LogicalType::kTinyInt:
                            case LogicalType::kSmallInt:
                            case LogicalType::kInteger:
                            case LogicalType::kBigInt: {
                                writer.Key(column_name.c_str());
                                writer.Int64(value.ToInteger());
                                break;
                            }
                            case LogicalType::kFloat: {
                                writer.Key(column_name.c_str());
                                writer.Double(value.ToFloat());
                                break;
                            }
                            case LogicalType::kDouble: {
                                writer.Key(column_name.c_str());
                                writer.Double(value.ToDouble());
                                break;
                            }
                            default: {
                                writer.Key(column_name.c_str());
                                writer.String(value.ToString().c_str());
                                break;
                            }
                        }
                        writer.EndObject();
                    }
                    writer.EndArray();
                }
            }
            writer.EndArray();

            if (result.result_table_->total_hits_count_flag_) {
                writer.Key("total_hits_count");
                writer.Uint64(result.result_table_->total_hits_count_);
            }

            SerializeErrorCode(writer, 0);
            http_status = HTTPStatus::CODE_200;
        } else {
            SerializeErrorCode(writer, (long)result.ErrorCode(), result.ErrorMsg());
            http_status = HTTPStatus::CODE_500;
        }
    } catch (simdjson::simdjson_error &e) {
        SerializeErrorCode(writer, (long)ErrorCode::kInvalidJsonFormat, e.what());
    }
    return;
}

void HTTPSearch::Explain(Infinity *infinity_ptr,
                         const String &db_name,
                         const String &table_name,
                         const String &input_json_str,
                         HTTPStatus &http_status,
                         rapidjson::Writer<rapidjson::StringBuffer> &writer) {
    http_status = HTTPStatus::CODE_500;
    try {
        simdjson::padded_string json_pad(input_json_str);
        simdjson::parser parser;
        simdjson::document doc = parser.iterate(json_pad);
        if (doc.type() != simdjson::json_type::object) {
            SerializeErrorCode(writer, (long)ErrorCode::kInvalidJsonFormat, "HTTP Body isn't json object");
        }
        UniquePtr<ParsedExpr> filter{};
        UniquePtr<ParsedExpr> limit{};
        UniquePtr<ParsedExpr> offset{};
        SearchExpr *search_expr{};
        Vector<ParsedExpr *> *output_columns{nullptr};
        Vector<ParsedExpr *> *highlight_columns{nullptr};
        Vector<OrderByExpr *> *order_by_list{nullptr};
        Vector<ParsedExpr *> *group_by_columns{nullptr};
        UniquePtr<ParsedExpr> having{nullptr};
        DeferFn defer_fn([&]() {
            if (output_columns != nullptr) {
                for (auto &expr : *output_columns) {
                    delete expr;
                }
                delete output_columns;
                output_columns = nullptr;
            }
        });
        DeferFn defer_fn_highlight([&]() {
            if (highlight_columns != nullptr) {
                for (auto &expr : *highlight_columns) {
                    delete expr;
                }
                delete highlight_columns;
                highlight_columns = nullptr;
            }
        });

        DeferFn defer_fn_order([&]() {
            if (order_by_list != nullptr) {
                for (auto &expr : *order_by_list) {
                    delete expr;
                }
                delete order_by_list;
                order_by_list = nullptr;
            }
        });

        ExplainType explain_type = ExplainType::kInvalid;
        for (auto elem : doc.get_object()) {
            String key = String((std::string_view)elem.unescaped_key());
            auto value = elem.value();
            ToLower(key);
            if (IsEqual(key, "output")) {
                if (output_columns != nullptr) {
                    SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "More than one output field.");
                    return;
                }
                if (value.type() != simdjson::json_type::array) {
                    SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "Output field should be array");
                    return;
                }

                output_columns = ParseOutput(value.raw_json(), http_status, writer);
                if (output_columns == nullptr) {
                    return;
                }
            } else if (IsEqual(key, "highlight")) {
                if (value.type() != simdjson::json_type::array) {
                    SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "Output field should be array");
                    return;
                }

                highlight_columns = ParseOutput(value.raw_json(), http_status, writer);
                if (highlight_columns == nullptr) {
                    return;
                }
            } else if (IsEqual(key, "sort")) {
                if (order_by_list != nullptr) {
                    SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "More than one sort field.");
                    return;
                }

                if (value.type() != simdjson::json_type::array) {
                    SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "Sort field should be array");
                    return;
                }

                order_by_list = ParseSort(value.raw_json(), http_status, writer);
                if (order_by_list == nullptr) {
                    return;
                }
            } else if (IsEqual(key, "group_by")) {
                if (group_by_columns != nullptr) {
                    SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "More than one group by field.");
                    return;
                }

                group_by_columns = ParseOutput(value.raw_json(), http_status, writer);
                if (group_by_columns == nullptr) {
                    return;
                }
            } else if (IsEqual(key, "having")) {
                if (having != nullptr) {
                    SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "More than one having field.");
                    return;
                }
                having = ParseFilter(value.raw_json(), http_status, writer);
                if (having == nullptr) {
                    return;
                }
            } else if (IsEqual(key, "filter")) {

                if (filter != nullptr) {
                    SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "More than one output field.");
                    return;
                }
                filter = ParseFilter(value.raw_json(), http_status, writer);
                if (filter == nullptr) {
                    return;
                }
            } else if (IsEqual(key, "limit")) {

                if (limit) {
                    SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "More than one limit field.");
                    return;
                }
                limit = ParseFilter(value.raw_json(), http_status, writer);
                if (!limit) {
                    return;
                }
            } else if (IsEqual(key, "offset")) {

                if (offset) {
                    SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "More than one offset field.");
                    return;
                }
                offset = ParseFilter(value.raw_json(), http_status, writer);
                if (!offset) {
                    return;
                }
            } else if (IsEqual(key, "search")) {
                if (search_expr) {
                    SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "More than one search field.");
                    return;
                }
                search_expr = ParseSearchExpr(value.raw_json(), http_status, writer);
                if (!search_expr) {
                    return;
                }
            } else if (IsEqual(key, "explain_type")) {
                String type = value.get<String>();
                if (IsEqual(type, "analyze")) {
                    explain_type = ExplainType::kAnalyze;
                } else if (IsEqual(type, "ast")) {
                    explain_type = ExplainType::kAst;
                } else if (IsEqual(type, "physical")) {
                    explain_type = ExplainType::kPhysical;
                } else if (IsEqual(type, "pipeline")) {
                    explain_type = ExplainType::kPipeline;
                } else if (IsEqual(type, "unopt")) {
                    explain_type = ExplainType::kUnOpt;
                } else if (IsEqual(type, "opt")) {
                    explain_type = ExplainType::kOpt;
                } else if (IsEqual(type, "fragment")) {
                    explain_type = ExplainType::kFragment;
                } else {
                    explain_type = ExplainType::kInvalid;
                }
            } else {
                SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "Unknown expression: " + key);
                return;
            }
        }

        const QueryResult result = infinity_ptr->Explain(db_name,
                                                         table_name,
                                                         explain_type,
                                                         search_expr,
                                                         filter.release(),
                                                         limit.release(),
                                                         offset.release(),
                                                         output_columns,
                                                         highlight_columns,
                                                         order_by_list,
                                                         group_by_columns,
                                                         having.release());
        search_expr = nullptr;
        output_columns = nullptr;
        highlight_columns = nullptr;
        order_by_list = nullptr;
        group_by_columns = nullptr;
        if (result.IsOk()) {
            SizeT block_rows = result.result_table_->DataBlockCount();
            writer.Key("output");
            writer.StartArray();
            for (SizeT block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                auto column_cnt = result.result_table_->ColumnCount();

                for (int row = 0; row < row_count; ++row) {
                    writer.StartObject();
                    for (SizeT col = 0; col < column_cnt; ++col) {
                        Value value = data_block->GetValue(col, row);
                        const String &column_name = result.result_table_->GetColumnNameById(col);
                        const String &column_value = value.ToString();
                        writer.Key(column_name.c_str());
                        writer.String(column_value.c_str());
                    }
                    writer.EndObject();
                }
            }
            writer.EndArray();

            SerializeErrorCode(writer, 0);
            http_status = HTTPStatus::CODE_200;
        } else {
            SerializeErrorCode(writer, (long)result.ErrorCode(), result.ErrorMsg());
            http_status = HTTPStatus::CODE_500;
        }
    } catch (simdjson::simdjson_error &e) {
        SerializeErrorCode(writer, (long)ErrorCode::kInvalidJsonFormat, e.what());
    }
    return;
}

UniquePtr<ParsedExpr> HTTPSearch::ParseFilter(std::string_view json_sv, HTTPStatus &http_status, rapidjson::Writer<rapidjson::StringBuffer> &writer) {
    simdjson::padded_string json_pad(json_sv);
    simdjson::parser parser;
    simdjson::document doc = parser.iterate(json_pad);

    if (doc.type() != simdjson::json_type::string) {
        SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "Filter field should be string");
        return nullptr;
    }
    UniquePtr<ExpressionParserResult> expr_parsed_result = MakeUnique<ExpressionParserResult>();
    ExprParser expr_parser;
    expr_parser.Parse(doc.get<String>(), expr_parsed_result.get());
    if (expr_parsed_result->IsError() || expr_parsed_result->exprs_ptr_->size() != 1) {
        SerializeErrorCode(writer,
                           (long)ErrorCode::kInvalidExpression,
                           fmt::format("Invalid expression: {}", String((std::string_view)doc.raw_json())));
        return nullptr;
    }

    ParsedExpr *res = expr_parsed_result->exprs_ptr_->at(0);
    expr_parsed_result->exprs_ptr_->at(0) = nullptr;
    UniquePtr<ParsedExpr> filter_expr(res);
    return filter_expr;
}

Vector<ParsedExpr *> *HTTPSearch::ParseOutput(std::string_view json_sv, HTTPStatus &http_status, rapidjson::Writer<rapidjson::StringBuffer> &writer) {
    simdjson::padded_string json_pad(json_sv);
    simdjson::parser parser;
    simdjson::document doc = parser.iterate(json_pad);

    Vector<ParsedExpr *> *output_columns = new Vector<ParsedExpr *>();
    DeferFn free_output_columns([&]() {
        if (output_columns != nullptr) {
            for (auto &expr : *output_columns) {
                delete expr;
                expr = nullptr;
            }
            delete output_columns;
            output_columns = nullptr;
        }
    });

    Vector<String> output_exprs;
    if (doc.type() == simdjson::json_type::array) {
        output_columns->reserve(doc.count_elements());
        for (auto output_expr : doc.get_array()) {
            if (!output_expr.is_string()) {
                SerializeErrorCode(writer,
                                   (long)ErrorCode::kInvalidExpression,
                                   fmt::format("Invalid expression: {}", String((std::string_view)output_expr.raw_json())));
                return nullptr;
            }
            output_exprs.push_back(output_expr.get<String>());
        }
    } else if (doc.type() == simdjson::json_type::string) {
        output_columns->reserve(1);
        output_exprs.push_back(doc.get<String>());
    } else {
        SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, fmt::format("Invalid expression: {}", json_sv));
        return nullptr;
    }

    for (auto output_expr : output_exprs) {
        String output_expr_str = output_expr;
        if (output_expr_str == "_row_id" or output_expr_str == "_similarity" or output_expr_str == "_distance" or output_expr_str == "_score") {
            auto parsed_expr = new FunctionExpr();
            if (output_expr_str == "_row_id") {
                parsed_expr->func_name_ = "row_id";
            } else if (output_expr_str == "_similarity") {
                parsed_expr->func_name_ = "similarity";
            } else if (output_expr_str == "_distance") {
                parsed_expr->func_name_ = "distance";
            } else if (output_expr_str == "_score") {
                parsed_expr->func_name_ = "score";
            }
            output_columns->emplace_back(parsed_expr);
            parsed_expr = nullptr;
        } else {
            UniquePtr<ExpressionParserResult> expr_parsed_result = MakeUnique<ExpressionParserResult>();
            ExprParser expr_parser;
            expr_parser.Parse(output_expr_str, expr_parsed_result.get());
            if (expr_parsed_result->IsError() || expr_parsed_result->exprs_ptr_->size() == 0) {
                SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, fmt::format("Invalid expression: {}", output_expr_str));
                return nullptr;
            }

            auto &expr_ptr = expr_parsed_result->exprs_ptr_->at(0);
            if (expr_ptr->type_ == ParsedExprType::kColumn) {
                if (output_expr_str == "*") {
                    auto column_expr = static_cast<ColumnExpr *>(expr_ptr);
                    column_expr->star_ = true;
                }
            }

            output_columns->emplace_back(expr_ptr);
            expr_ptr = nullptr;
        }
    }

    // Avoiding DeferFN auto free the output expressions
    Vector<ParsedExpr *> *res = output_columns;
    output_columns = nullptr;
    return res;
}

Vector<OrderByExpr *> *HTTPSearch::ParseSort(std::string_view json_sv, HTTPStatus &http_status, rapidjson::Writer<rapidjson::StringBuffer> &writer) {
    simdjson::padded_string json_pad(json_sv);
    simdjson::parser parser;
    simdjson::document doc = parser.iterate(json_pad);

    Vector<OrderByExpr *> *order_by_list = new Vector<OrderByExpr *>();
    DeferFn defer_fn([&]() {
        if (order_by_list != nullptr) {
            for (auto &expr : *order_by_list) {
                delete expr;
            }
            delete order_by_list;
            order_by_list = nullptr;
        }
    });

    for (auto order_expr : doc.get_array()) {
        for (auto expression : order_expr.get_object()) {
            String key = String((std::string_view)expression.unescaped_key());
            ToLower(key);
            auto order_by_expr = MakeUnique<OrderByExpr>();
            if (key == "_row_id" or key == "_similarity" or key == "_distance" or key == "_score") {
                auto parsed_expr = new FunctionExpr();
                if (key == "_row_id") {
                    parsed_expr->func_name_ = "row_id";
                } else if (key == "_similarity") {
                    parsed_expr->func_name_ = "similarity";
                } else if (key == "_distance") {
                    parsed_expr->func_name_ = "distance";
                } else if (key == "_score") {
                    parsed_expr->func_name_ = "score";
                }
                order_by_expr->expr_ = parsed_expr;
                parsed_expr = nullptr;
            } else {
                UniquePtr<ExpressionParserResult> expr_parsed_result = MakeUnique<ExpressionParserResult>();
                ExprParser expr_parser;
                expr_parser.Parse(key, expr_parsed_result.get());
                if (expr_parsed_result->IsError() || expr_parsed_result->exprs_ptr_->size() == 0) {
                    SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, fmt::format("Invalid expression: {}", key));
                    return nullptr;
                }

                order_by_expr->expr_ = expr_parsed_result->exprs_ptr_->at(0);
                expr_parsed_result->exprs_ptr_->at(0) = nullptr;
            }

            String value = expression.value().get<String>();
            ToLower(value);
            if (value == "asc") {
                order_by_expr->type_ = OrderType::kAsc;
            } else if (value == "desc") {
                order_by_expr->type_ = OrderType::kDesc;
            } else {
                SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, fmt::format("Invalid expression: {}", value));
                return nullptr;
            }

            order_by_list->emplace_back(order_by_expr.release());
        }
    }

    // Avoiding DeferFN auto free the output expressions
    Vector<OrderByExpr *> *res = order_by_list;
    order_by_list = nullptr;
    return res;
}

SearchExpr *HTTPSearch::ParseSearchExpr(std::string_view json_sv, HTTPStatus &http_status, rapidjson::Writer<rapidjson::StringBuffer> &writer) {
    simdjson::padded_string json_pad(json_sv);
    simdjson::parser parser;
    simdjson::document doc = parser.iterate(json_pad);

    if (doc.type() != simdjson::json_type::array) {
        SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "Search field should be list");
        return {};
    }
    auto child_expr = new std::vector<ParsedExpr *>();
    DeferFn defer_fn([&] {
        if (child_expr) {
            for (const auto expr : *child_expr) {
                delete expr;
            }
            delete child_expr;
            child_expr = nullptr;
        }
    });
    child_expr->reserve(doc.count_elements());
    for (auto search_obj : doc.get_array()) {
        std::string_view search_obj_sv = search_obj.raw_json();
        simdjson::padded_string sub_pad(search_obj_sv);
        simdjson::parser sub_parser;
        simdjson::document sub_doc = sub_parser.iterate(sub_pad);

        if (sub_doc.type() != simdjson::json_type::object) {
            if (search_obj.type() != simdjson::json_type::object) {
                SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "Search field should be list of objects");
                return {};
            }

            auto fusion = sub_doc["fusion_method"].get<String>();
            auto match = sub_doc["match_method"].get<String>();

            if (fusion.error() == simdjson::SUCCESS && match.error() == simdjson::SUCCESS) {
                SerializeErrorCode(writer,
                                   (long)ErrorCode::kInvalidExpression,
                                   "Every single search expression should not contain both fusion_method and match_method fields");
                return {};
            }
            if (fusion.error() == simdjson::SUCCESS) {
                auto fusion_expr = ParseFusion(search_obj_sv, http_status, writer);
                if (!fusion_expr) {
                    return {};
                }
                child_expr->push_back(fusion_expr.release());
            } else {
                // match type
                String match_method = match.value();
                ToLower(match_method);
                if (match_method == "dense") {
                    auto match_dense_expr = ParseMatchDense(search_obj_sv, http_status, writer);
                    if (!match_dense_expr) {
                        return {};
                    }
                    child_expr->push_back(match_dense_expr.release());
                } else if (match_method == "sparse") {
                    auto match_sparse_expr = ParseMatchSparse(search_obj_sv, http_status, writer);
                    if (!match_sparse_expr) {
                        return {};
                    }
                    child_expr->push_back(match_sparse_expr.release());
                } else if (match_method == "text") {
                    auto match_text_expr = ParseMatchText(search_obj_sv, http_status, writer);
                    if (!match_text_expr) {
                        return {};
                    }
                    child_expr->push_back(match_text_expr.release());
                } else if (match_method == "tensor") {
                    auto match_tensor_expr = ParseMatchTensor(search_obj_sv, http_status, writer);
                    if (!match_tensor_expr) {
                        return {};
                    }
                    child_expr->push_back(match_tensor_expr.release());
                } else {
                    SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, fmt::format("Unknown match method: {}", match_method));
                    return {};
                }
            }
        }
    }
    auto search_expr = new SearchExpr();
    try {
        search_expr->SetExprs(child_expr);
        child_expr = nullptr;
    } catch (std::exception &e) {
        delete search_expr;
        search_expr = nullptr;
        SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, fmt::format("Invalid Search expression, error info: {}", e.what()));
        return nullptr;
    }
    return search_expr;
}

UniquePtr<FusionExpr> HTTPSearch::ParseFusion(std::string_view json_sv, HTTPStatus &http_status, rapidjson::Writer<rapidjson::StringBuffer> &writer) {
    simdjson::padded_string json_pad(json_sv);
    simdjson::parser parser;
    simdjson::document doc = parser.iterate(json_pad);

    if (doc.type() != simdjson::json_type::object) {
        SerializeErrorCode(writer,
                           (long)ErrorCode::kInvalidExpression,
                           fmt::format("Fusion expression must be a json object: {}", String((std::string_view)doc.raw_json())));
        return nullptr;
    }
    i64 topn = -1;
    std::string_view fusion_params;
    auto fusion_expr = MakeUnique<FusionExpr>();
    // must have: "fusion_method", "topn"
    // may have: "params"
    constexpr std::array possible_keys{"fusion_method", "topn", "params"};
    std::set<String> possible_keys_set(possible_keys.begin(), possible_keys.end());
    for (auto expression : doc.get_object()) {
        String key = String((std::string_view)expression.unescaped_key());
        auto params = expression.value();
        ToLower(key);
        if (!possible_keys_set.erase(key)) {
            SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, fmt::format("Unknown Fusion expression key: {}", key));
            return nullptr;
        }
        if (IsEqual(key, "fusion_method")) {
            String method_str = params.get<String>();
            ToLower(method_str);
            fusion_expr->method_ = std::move(method_str);
        } else if (IsEqual(key, "topn")) {
            if (!params.is_integer()) {
                SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "Fusion expression topn field should be integer");
                return nullptr;
            }
            topn = params.get<i64>();
        } else if (IsEqual(key, "params")) {
            if (params.type() != simdjson::json_type::object) {
                SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "Fusion params should be object");
                return nullptr;
            }
            fusion_params = params.raw_json();
        }
    }
    // "params" is optional
    possible_keys_set.erase("params");
    // check if all required fields are set
    if (!possible_keys_set.empty()) {
        SerializeErrorCode(
            writer,
            (long)ErrorCode::kInvalidExpression,
            fmt::format("Invalid Fusion expression: following fields are required but not found: {}", fmt::join(possible_keys_set, ", ")));
        return nullptr;
    }
    if (topn <= 0) {
        SerializeErrorCode(writer, (long)ErrorCode::kInvalidTopKType, "Fusion expression topn field should be positive integer");
        return nullptr;
    }
    String extra_params_str{};
    if (fusion_expr->method_ == "match_tensor") {
        rapidjson::Document params_doc;
        params_doc.Parse(String(fusion_params).c_str());
        params_doc.AddMember("match_method", "tensor", params_doc.GetAllocator());
        if (params_doc.FindMember("topn") != params_doc.MemberEnd()) {
            SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "Fusion expression topn field should not be set in params");
            return nullptr;
        }
        params_doc.AddMember("topn", topn, params_doc.GetAllocator());
        rapidjson::StringBuffer sb;
        {
            rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
            params_doc.Accept(writer);
        }
        auto match_tensor_expr = ParseMatchTensor(sb.GetString(), http_status, writer);
        if (!match_tensor_expr) {
            return nullptr;
        }
        fusion_expr->match_tensor_expr_ = std::move(match_tensor_expr);
    } else {
        if (fusion_params.length()) {
            simdjson::padded_string fusion_pad(fusion_params);
            simdjson::document fusion_doc = parser.iterate(fusion_pad);
            for (auto param : fusion_doc.get_object()) {
                String param_k = String((std::string_view)param.unescaped_key());
                String param_v = param.value().get<String>();
                extra_params_str += fmt::format(";{}={}", param_k, param_v);
            }
        }
    }
    fusion_expr->SetOptions(fmt::format("topn={}{}", topn, extra_params_str));
    return fusion_expr;
}

UniquePtr<KnnExpr>
HTTPSearch::ParseMatchDense(std::string_view json_sv, HTTPStatus &http_status, rapidjson::Writer<rapidjson::StringBuffer> &writer) {
    simdjson::padded_string json_pad(json_sv);
    simdjson::parser parser;
    simdjson::document doc = parser.iterate(json_pad);

    if (doc.type() != simdjson::json_type::object) {
        SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "MatchDense field should be object");
        return nullptr;
    }
    auto knn_expr = MakeUnique<KnnExpr>();
    i64 topn = -1;
    std::string_view query_vector_json;
    // must have: "match_method", "fields", "query_vector", "element_type", "metric_type", "topn"
    // may have: "params"
    constexpr std::array possible_keys{"match_method", "fields", "query_vector", "element_type", "metric_type", "topn", "params"};
    std::set<String> possible_keys_set(possible_keys.begin(), possible_keys.end());
    for (auto field_json_obj : doc.get_object()) {
        String key = String((std::string_view)field_json_obj.unescaped_key());
        auto value = field_json_obj.value();
        ToLower(key);
        if (!possible_keys_set.erase(key)) {
            SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, fmt::format("Unknown or duplicate MatchDense expression key: {}", key));
            return nullptr;
        }
        if (IsEqual(key, "match_method")) {
            String match_method = value.get<String>();
            ToLower(match_method);
            if (!IsEqual(match_method, "dense")) {
                SerializeErrorCode(writer,
                                   (long)ErrorCode::kInvalidExpression,
                                   fmt::format("MatchDense expression match_method should be dense, but got: {}", match_method));
                return nullptr;
            }
        } else if (IsEqual(key, "fields")) {
            auto column_expr = MakeUnique<ColumnExpr>();
            auto column_str = value.get<String>();
            column_expr->names_.push_back(std::move(column_str));
            knn_expr->column_expr_ = column_expr.release();
        } else if (IsEqual(key, "query_vector")) {
            query_vector_json = value.raw_json();
        } else if (IsEqual(key, "element_type")) {
            String element_type = value.get<String>();
            ToUpper(element_type);
            try {
                knn_expr->embedding_data_type_ = EmbeddingT::String2EmbeddingDataType(element_type);
            } catch (std::exception &e) {
                SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, fmt::format("Not supported vector element type: {}", element_type));
                return nullptr;
            }
        } else if (IsEqual(key, "metric_type")) {
            String metric_type = value.get<String>();
            ToLower(metric_type);
            if (!knn_expr->InitDistanceType(metric_type.c_str())) {
                SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, fmt::format("Unknown knn distance: {}", metric_type));
                return nullptr;
            }
        } else if (IsEqual(key, "topn")) {
            if (!value.is_integer()) {
                SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "MatchDense topn field should be integer");
                return nullptr;
            }
            topn = value.get<i64>();
        } else if (IsEqual(key, "params")) {
            if (value.type() != simdjson::json_type::object) {
                SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "MatchDense params should be object");
                return nullptr;
            }
            for (auto param_it : value.get_object()) {
                String param_k = String((std::string_view)param_it.unescaped_key());
                auto param_v = param_it.value();
                if (param_k == "filter") {
                    if (knn_expr->filter_expr_) {
                        SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "More than one filter field in match dense params.");
                        return nullptr;
                    }
                    knn_expr->filter_expr_ = ParseFilter(param_v.raw_json(), http_status, writer);
                    if (!knn_expr->filter_expr_) {
                        return nullptr;
                    }
                    // do not put it into opt_params_
                    continue;
                }
                if (param_k == "index_name") {
                    knn_expr->index_name_ = param_v.get<String>();
                    continue;
                }
                if (param_k == "ignore_index" && (String)param_v.get<String>() == "true") {
                    knn_expr->ignore_index_ = true;
                    continue;
                }
                if (knn_expr->opt_params_ == nullptr) {
                    knn_expr->opt_params_ = new Vector<InitParameter *>();
                }
                auto parameter = MakeUnique<InitParameter>();
                parameter->param_name_ = param_k;
                parameter->param_value_ = param_v.get<String>();
                knn_expr->opt_params_->emplace_back(parameter.release());
            }
        }
    }
    // "params" is optional
    possible_keys_set.erase("params");
    // check if all required fields are set
    if (!possible_keys_set.empty()) {
        SerializeErrorCode(
            writer,
            (long)ErrorCode::kInvalidExpression,
            fmt::format("Invalid MatchDense expression: following fields are required but not found: {}", fmt::join(possible_keys_set, ", ")));
        return nullptr;
    }
    if (topn <= 0) {
        SerializeErrorCode(writer, (long)ErrorCode::kInvalidTopKType, "MatchDense expression topn field should be positive integer");
        return nullptr;
    }
    knn_expr->topn_ = topn;
    const auto [dimension, embedding_ptr] = ParseVector(query_vector_json, knn_expr->embedding_data_type_, http_status, writer);
    if (embedding_ptr == nullptr) {
        return nullptr;
    }
    knn_expr->dimension_ = dimension;
    knn_expr->embedding_data_ptr_ = embedding_ptr;
    return knn_expr;
}

UniquePtr<MatchExpr>
HTTPSearch::ParseMatchText(std::string_view json_sv, HTTPStatus &http_status, rapidjson::Writer<rapidjson::StringBuffer> &writer) {
    simdjson::padded_string json_pad(json_sv);
    simdjson::parser parser;
    simdjson::document doc = parser.iterate(json_pad);

    if (doc.type() != simdjson::json_type::object) {
        SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "MatchText field should be object");
        return nullptr;
    }
    auto match_expr = MakeUnique<MatchExpr>();
    i64 topn = -1;
    String extra_params{};
    // must have: "match_method", "fields", "matching_text", "topn"
    // may have: "params"
    constexpr std::array possible_keys{"match_method", "fields", "matching_text", "topn", "params"};
    std::set<String> possible_keys_set(possible_keys.begin(), possible_keys.end());
    for (auto field_json_obj : doc.get_object()) {
        String key = String((std::string_view)field_json_obj.unescaped_key());
        auto value = field_json_obj.value();
        ToLower(key);
        if (!possible_keys_set.erase(key)) {
            SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, fmt::format("Unknown or duplicate MatchText expression key: {}", key));
            return nullptr;
        }
        if (IsEqual(key, "match_method")) {
            String match_method = value.get<String>();
            ToLower(match_method);
            if (!IsEqual(match_method, "text")) {
                SerializeErrorCode(writer,
                                   (long)ErrorCode::kInvalidExpression,
                                   fmt::format("MatchText expression match_method should be text, but got: {}", match_method));
                return nullptr;
            }
        } else if (IsEqual(key, "fields")) {
            match_expr->fields_ = value.get<String>();
        } else if (IsEqual(key, "matching_text")) {
            match_expr->matching_text_ = value.get<String>();
        } else if (IsEqual(key, "topn")) {
            if (!value.is_integer()) {
                SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "MatchText topn field should be integer");
                return nullptr;
            }
            topn = value.get<i64>();
        } else if (IsEqual(key, "params")) {
            if (value.type() != simdjson::json_type::object) {
                SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "MatchText params should be object");
                return nullptr;
            }
            for (auto param : value.get_object()) {
                String param_k = String((std::string_view)param.unescaped_key());
                auto param_v = param.value();
                if (param_k == "index_names") {
                    match_expr->index_names_ = param_v.get<String>();
                    continue;
                }
                if (param_k == "filter") {
                    if (match_expr->filter_expr_) {
                        SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "More than one filter field in match text params.");
                        return nullptr;
                    }
                    match_expr->filter_expr_ = ParseFilter(param_v.raw_json(), http_status, writer);
                    if (!match_expr->filter_expr_) {
                        return nullptr;
                    }
                    // do not put it into extra_params
                    continue;
                }
                extra_params += fmt::format(";{}={}", param_k, (String)param_v.get<String>());
            }
        }
    }
    // "params" is optional
    possible_keys_set.erase("params");
    // check if all required fields are set
    if (!possible_keys_set.empty()) {
        SerializeErrorCode(
            writer,
            (long)ErrorCode::kInvalidExpression,
            fmt::format("Invalid MatchText expression: following fields are required but not found: {}", fmt::join(possible_keys_set, ", ")));
        return nullptr;
    }
    if (topn <= 0) {
        SerializeErrorCode(writer, (long)ErrorCode::kInvalidTopKType, "MatchText expression topn field should be positive integer");
        return nullptr;
    }
    match_expr->options_text_ = fmt::format("topn={}{}", topn, extra_params);
    return match_expr;
}

UniquePtr<MatchTensorExpr>
HTTPSearch::ParseMatchTensor(std::string_view json_sv, HTTPStatus &http_status, rapidjson::Writer<rapidjson::StringBuffer> &writer) {
    simdjson::padded_string json_pad(json_sv);
    simdjson::parser parser;
    simdjson::document doc = parser.iterate(json_pad);

    if (doc.type() != simdjson::json_type::object) {
        SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "MatchTensor field should be object");
        return nullptr;
    }
    auto match_tensor_expr = MakeUnique<MatchTensorExpr>();
    match_tensor_expr->SetSearchMethodStr("maxsim");
    String element_type{};
    SharedPtr<ConstantExpr> tensor_expr{};
    i64 topn = -1;
    String extra_params{};
    // must have: "match_method", "fields", "query_tensor", "element_type", "topn"
    // may have: "params"
    constexpr std::array possible_keys{"match_method", "field", "query_tensor", "element_type", "topn", "params"};
    std::set<String> possible_keys_set(possible_keys.begin(), possible_keys.end());
    for (auto field_json_obj : doc.get_object()) {
        String key = String((std::string_view)field_json_obj.unescaped_key());
        auto value = field_json_obj.value();
        ToLower(key);
        if (!possible_keys_set.erase(key)) {
            SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, fmt::format("Unknown or duplicate MatchTensor expression key: {}", key));
            return nullptr;
        }
        if (IsEqual(key, "match_method")) {
            String match_method = value.get<String>();
            ToLower(match_method);
            if (!IsEqual(match_method, "tensor")) {
                SerializeErrorCode(writer,
                                   (long)ErrorCode::kInvalidExpression,
                                   fmt::format("MatchTensor expression match_method should be tensor, but got: {}", match_method));
                return nullptr;
            }
        } else if (IsEqual(key, "field")) {
            auto column_str = value.get<String>();
            auto column_expr = MakeUnique<ColumnExpr>();
            column_expr->names_.push_back(std::move(column_str));
            match_tensor_expr->column_expr_ = std::move(column_expr);
        } else if (IsEqual(key, "query_tensor")) {
            simdjson::padded_string find_pad(json_sv);
            simdjson::document find_doc = parser.iterate(find_pad);
            if (simdjson::value val; find_doc["element_type"].get(val) != simdjson::SUCCESS) {
                SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "Missing element_type for query_tensor");
                return nullptr;
            }
            try {
                tensor_expr = BuildConstantExprFromJson(value.raw_json());
            } catch (std::exception &e) {
                SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, fmt::format("Invalid query_tensor, error info: {}", e.what()));
                return nullptr;
            }
        } else if (IsEqual(key, "element_type")) {
            element_type = value.get<String>();
        } else if (IsEqual(key, "topn")) {
            if (!value.is_integer()) {
                SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "MatchTensor topn field should be integer");
                return nullptr;
            }
            topn = value.get<i64>();
        } else if (IsEqual(key, "params")) {
            if (value.type() != simdjson::json_type::object) {
                SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "MatchTensor params should be object");
                return nullptr;
            }
            for (auto param : value.get_object()) {
                String param_k = String((std::string_view)param.unescaped_key());
                auto param_v = param.value();
                if (param_k == "index_name") {
                    match_tensor_expr->index_name_ = param_v.get<String>();
                    continue;
                }
                if (param_k == "ignore_index" && (String)param_v.get<String>() == "true") {
                    match_tensor_expr->ignore_index_ = true;
                    continue;
                }
                if (param_k == "filter") {
                    if (match_tensor_expr->filter_expr_) {
                        SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "More than one filter field in match tensor params.");
                        return nullptr;
                    }
                    match_tensor_expr->filter_expr_ = ParseFilter(param_v.raw_json(), http_status, writer);
                    if (!match_tensor_expr->filter_expr_) {
                        return nullptr;
                    }
                    // do not put it into extra_params
                    continue;
                }
                extra_params += fmt::format(";{}={}", param_k, (String)param_v.get<String>());
            }
        }
    }
    // "params" is optional
    possible_keys_set.erase("params");
    // check if all required fields are set
    if (!possible_keys_set.empty()) {
        SerializeErrorCode(
            writer,
            (long)ErrorCode::kInvalidExpression,
            fmt::format("Invalid MatchTensor expression: following fields are required but not found: {}", fmt::join(possible_keys_set, ", ")));
        return nullptr;
    }
    if (topn <= 0) {
        SerializeErrorCode(writer, (long)ErrorCode::kInvalidTopKType, "MatchTensor expression topn field should be positive integer");
        return nullptr;
    }
    match_tensor_expr->options_text_ = fmt::format("topn={}{}", topn, extra_params);
    try {
        match_tensor_expr->SetQueryTensorStr(std::move(element_type), tensor_expr.get());
    } catch (std::exception &e) {
        SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, fmt::format("Invalid query_tensor, error info: {}", e.what()));
        return nullptr;
    }
    return match_tensor_expr;
}

UniquePtr<MatchSparseExpr>
HTTPSearch::ParseMatchSparse(std::string_view json_sv, HTTPStatus &http_status, rapidjson::Writer<rapidjson::StringBuffer> &writer) {
    simdjson::padded_string json_pad(json_sv);
    simdjson::parser parser;
    simdjson::document doc = parser.iterate(json_pad);

    if (doc.type() != simdjson::json_type::object) {
        SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "MatchSparse field should be object");
        return nullptr;
    }
    auto match_sparse_expr = MakeUnique<MatchSparseExpr>();
    auto *opt_params_ptr = new Vector<InitParameter *>();
    DeferFn release_opt([&]() {
        if (opt_params_ptr != nullptr) {
            for (auto &params_ptr : *opt_params_ptr) {
                delete params_ptr;
            }
            delete opt_params_ptr;
            opt_params_ptr = nullptr;
        }
    });
    i64 topn = -1;
    // must have: "match_method", "fields", "query_vector", "metric_type", "topn"
    // may have: "params"
    constexpr std::array possible_keys{"match_method", "fields", "query_vector", "metric_type", "topn", "params"};
    std::set<String> possible_keys_set(possible_keys.begin(), possible_keys.end());
    for (auto field_json_obj : doc.get_object()) {
        String key = String((std::string_view)field_json_obj.unescaped_key());
        auto value = field_json_obj.value();
        ToLower(key);
        if (!possible_keys_set.erase(key)) {
            SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, fmt::format("Unknown or duplicate MatchSparse expression key: {}", key));
            return nullptr;
        }
        if (IsEqual(key, "match_method")) {
            String match_method = value.get<String>();
            ToLower(match_method);
            if (!IsEqual(match_method, "sparse")) {
                SerializeErrorCode(writer,
                                   (long)ErrorCode::kInvalidExpression,
                                   fmt::format("MatchSparse expression match_method should be sparse, but got: {}", match_method));
                return nullptr;
            }
        } else if (IsEqual(key, "fields")) {
            auto column_expr = MakeUnique<ColumnExpr>();
            auto column_str = value.get<String>();
            column_expr->names_.push_back(std::move(column_str));
            match_sparse_expr->column_expr_ = std::move(column_expr);
        } else if (IsEqual(key, "query_vector")) {
            UniquePtr<ConstantExpr> query_sparse_expr = ParseSparseVector(value.raw_json(), http_status, writer);
            if (!query_sparse_expr) {
                return nullptr;
            }
            ConstantExpr *const_sparse_expr = query_sparse_expr.release();
            match_sparse_expr->SetQuerySparse(const_sparse_expr);
            assert(const_sparse_expr == nullptr);
        } else if (IsEqual(key, "metric_type")) {
            try {
                match_sparse_expr->SetMetricType(value.get<String>());
            } catch (std::exception &e) {
                SerializeErrorCode(writer,
                                   (long)ErrorCode::kInvalidExpression,
                                   fmt::format("Invalid metric_type: {}, error info: {}", (String)value.get<String>(), e.what()));
                return nullptr;
            }
        } else if (IsEqual(key, "topn")) {
            if (!value.is_integer()) {
                SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "MatchSparse topn field should be integer");
                return nullptr;
            }
            topn = value.get<i64>();
        } else if (IsEqual(key, "params")) {
            if (value.type() != simdjson::json_type::object) {
                SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "MatchSparse params should be object");
                return nullptr;
            }
            for (auto param : value.get_object()) {
                String param_k = String((std::string_view)param.unescaped_key());
                auto param_v = param.value();
                if (param_k == "filter") {
                    if (match_sparse_expr->filter_expr_) {
                        SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, "More than one filter field in match sparse params.");
                        return nullptr;
                    }
                    match_sparse_expr->filter_expr_ = ParseFilter(param_v.raw_json(), http_status, writer);
                    if (!match_sparse_expr->filter_expr_) {
                        return nullptr;
                    }
                    // do not put it into opt_params_ptr
                    continue;
                }
                if (param_k == "index_name") {
                    match_sparse_expr->index_name_ = param_v.get<String>();
                    continue;
                }
                if (param_k == "ignore_index" && (String)param_v.get<String>() == "true") {
                    match_sparse_expr->ignore_index_ = true;
                    continue;
                }
                auto *init_parameter = new InitParameter();
                init_parameter->param_name_ = param_k;
                init_parameter->param_value_ = param_v.get<String>();
                opt_params_ptr->emplace_back(init_parameter);
            }
        }
    }
    // "params" is optional
    possible_keys_set.erase("params");
    // check if all required fields are set
    if (!possible_keys_set.empty()) {
        SerializeErrorCode(
            writer,
            (long)ErrorCode::kInvalidExpression,
            fmt::format("Invalid MatchSparse expression: following fields are required but not found: {}", fmt::join(possible_keys_set, ", ")));
        return nullptr;
    }
    if (topn <= 0) {
        SerializeErrorCode(writer, (long)ErrorCode::kInvalidTopKType, "MatchSparse expression topn field should be positive integer");
        return nullptr;
    }
    match_sparse_expr->SetOptParams(topn, opt_params_ptr);
    opt_params_ptr = nullptr;
    return match_sparse_expr;
}

UniquePtr<ConstantExpr>
HTTPSearch::ParseSparseVector(std::string_view json_sv, HTTPStatus &http_status, rapidjson::Writer<rapidjson::StringBuffer> &writer) {
    simdjson::padded_string json_pad(json_sv);
    simdjson::parser parser;
    simdjson::document doc = parser.iterate(json_pad);

    if (doc.type() != simdjson::json_type::object) {
        SerializeErrorCode(writer, (long)ErrorCode::kInvalidEmbeddingDataType, "Sparse vector should be object");
        return nullptr;
    }
    if (doc.count_fields() == 0) {
        SerializeErrorCode(writer, (long)ErrorCode::kInvalidEmbeddingDataType, "Empty sparse vector, cannot decide type");
        return nullptr;
    }

    UniquePtr<ConstantExpr> const_sparse_expr{};
    {
        simdjson::padded_string find_pad(json_sv);
        simdjson::document find_doc = parser.iterate(find_pad);
        simdjson::field first_kv = *find_doc.get_object().begin();
        if (first_kv.value().type() != simdjson::json_type::number) {
            SerializeErrorCode(writer, (long)ErrorCode::kInvalidEmbeddingDataType, "Sparse value element type error");
            return nullptr;
        }
        switch (first_kv.value().get_number_type()) {
            case simdjson::number_type::unsigned_integer:
            case simdjson::number_type::signed_integer: {
                const_sparse_expr = MakeUnique<ConstantExpr>(LiteralType::kLongSparseArray);
                break;
            }
            case simdjson::number_type::floating_point_number: {
                const_sparse_expr = MakeUnique<ConstantExpr>(LiteralType::kDoubleSparseArray);
                break;
            }
            default: {
                SerializeErrorCode(writer, (long)ErrorCode::kInvalidEmbeddingDataType, "Sparse value element type error");
                return nullptr;
            }
        }
    }

    for (HashSet<i64> key_set; auto sparse_it : doc.get_object()) {
        String sparse_k = String((std::string_view)sparse_it.unescaped_key());
        auto sparse_v = sparse_it.value();
        i64 key_val = {};
        try {
            key_val = std::stoll(sparse_k);
        } catch (std::exception &e) {
            SerializeErrorCode(writer,
                               (long)ErrorCode::kInvalidEmbeddingDataType,
                               fmt::format("Error when try to cast sparse key '{}' to integer, error info: {}", sparse_k, e.what()));
            return nullptr;
        }
        if (const auto [_, insert_ok] = key_set.insert(key_val); !insert_ok) {
            SerializeErrorCode(writer, (long)ErrorCode::kInvalidEmbeddingDataType, fmt::format("Duplicate key {} in sparse array!", key_val));
            return nullptr;
        }
        bool good_v = false;
        switch (sparse_v.get_number_type()) {
            case simdjson::number_type::unsigned_integer:
            case simdjson::number_type::signed_integer: {
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
            SerializeErrorCode(writer, (long)ErrorCode::kInvalidEmbeddingDataType, "Sparse value element type error");
            return nullptr;
        }
    }
    return const_sparse_expr;
}

Tuple<i64, void *> HTTPSearch::ParseVector(std::string_view json_sv,
                                           EmbeddingDataType elem_type,
                                           HTTPStatus &http_status,
                                           rapidjson::Writer<rapidjson::StringBuffer> &writer) {
    simdjson::padded_string json_pad(json_sv);
    simdjson::parser parser;
    simdjson::document doc = parser.iterate(json_pad);

    if (doc.type() != simdjson::json_type::array) {
        SerializeErrorCode(writer, (long)ErrorCode::kInvalidExpression, fmt::format("Can't recognize embedding/vector."));
        return {0, nullptr};
    }
    SizeT dimension = doc.count_elements();
    if (dimension == 0) {
        SerializeErrorCode(writer, (long)ErrorCode::kInvalidEmbeddingDataType, fmt::format("Empty embedding data"));
        return {0, nullptr};
    }

    switch (elem_type) {
        case EmbeddingDataType::kElemBit: {
            if (dimension % 8 != 0) {
                SerializeErrorCode(writer,
                                   (long)ErrorCode::kInvalidEmbeddingDataType,
                                   fmt::format("bit embeddings should have dimension of times of 8"));
                return {0, nullptr};
            }
            u8 *embedding_data_ptr = new u8[dimension / 8];
            DeferFn defer_free_embedding([&]() {
                if (embedding_data_ptr != nullptr) {
                    delete[] embedding_data_ptr;
                    embedding_data_ptr = nullptr;
                }
            });
            for (SizeT i = 0; i < dimension / 8; ++i) {
                u8 unit = 0;
                for (SizeT bit_idx = 0; bit_idx < 8; bit_idx++) {
                    simdjson::padded_string sub_pad(json_sv);
                    simdjson::parser sub_parser;
                    simdjson::document sub_doc = sub_parser.iterate(sub_pad);
                    auto value_ref = sub_doc.at(i * 8 + bit_idx);
                    if (value_ref.type() != simdjson::json_type::number) {
                        SerializeErrorCode(writer,
                                           (long)ErrorCode::kInvalidEmbeddingDataType,
                                           fmt::format("Embedding element type should be integer"));
                        return {0, nullptr};
                    }
                    switch (value_ref.get_number_type()) {
                        case simdjson::number_type::signed_integer:
                        case simdjson::number_type::unsigned_integer: {
                            if (value_ref.get<i64>() > 0) {
                                unit |= (1 << bit_idx);
                            }
                            break;
                        }
                        default: {
                            SerializeErrorCode(writer,
                                               (long)ErrorCode::kInvalidEmbeddingDataType,
                                               fmt::format("Embedding element type should be integer"));
                            return {0, nullptr};
                        }
                    }
                }
                embedding_data_ptr[i] = unit;
            }
            u8 *res = embedding_data_ptr;
            embedding_data_ptr = nullptr;
            return {dimension, res};
        }
        case EmbeddingDataType::kElemInt32: {
            i32 *embedding_data_ptr = new i32[dimension];
            DeferFn defer_free_embedding([&]() {
                if (embedding_data_ptr != nullptr) {
                    delete[] embedding_data_ptr;
                    embedding_data_ptr = nullptr;
                }
            });
            for (SizeT idx = 0; auto value_ref : doc.get_array()) {
                if (value_ref.type() != simdjson::json_type::number) {
                    SerializeErrorCode(writer, (long)ErrorCode::kInvalidEmbeddingDataType, fmt::format("Embedding element type should be integer"));
                    return {0, nullptr};
                }
                switch (value_ref.get_number_type()) {
                    case simdjson::number_type::signed_integer:
                    case simdjson::number_type::unsigned_integer: {
                        embedding_data_ptr[idx++] = value_ref.get<i64>();
                        break;
                    }
                    default: {
                        SerializeErrorCode(writer,
                                           (long)ErrorCode::kInvalidEmbeddingDataType,
                                           fmt::format("Embedding element type should be integer"));
                        return {0, nullptr};
                    }
                }
            }

            i32 *res = embedding_data_ptr;
            embedding_data_ptr = nullptr;
            return {dimension, res};
        }
        case EmbeddingDataType::kElemInt8: {
            i8 *embedding_data_ptr = new i8[dimension];
            DeferFn defer_free_embedding([&]() {
                if (embedding_data_ptr != nullptr) {
                    delete[] embedding_data_ptr;
                    embedding_data_ptr = nullptr;
                }
            });
            for (SizeT idx = 0; auto value_ref : doc.get_array()) {
                if (value_ref.type() != simdjson::json_type::number) {
                    SerializeErrorCode(writer, (long)ErrorCode::kInvalidEmbeddingDataType, fmt::format("Embedding element type should be integer"));
                    return {0, nullptr};
                }
                switch (value_ref.get_number_type()) {
                    case simdjson::number_type::signed_integer:
                    case simdjson::number_type::unsigned_integer: {
                        embedding_data_ptr[idx++] = value_ref.get<i64>();
                        break;
                    }
                    default: {
                        SerializeErrorCode(writer,
                                           (long)ErrorCode::kInvalidEmbeddingDataType,
                                           fmt::format("Embedding element type should be integer"));
                        return {0, nullptr};
                    }
                }
            }

            i8 *res = embedding_data_ptr;
            embedding_data_ptr = nullptr;
            return {dimension, res};
        }
        case EmbeddingDataType::kElemUInt8: {
            u8 *embedding_data_ptr = new u8[dimension];
            DeferFn defer_free_embedding([&]() {
                if (embedding_data_ptr != nullptr) {
                    delete[] embedding_data_ptr;
                    embedding_data_ptr = nullptr;
                }
            });
            for (SizeT idx = 0; auto value_ref : doc.get_array()) {
                if (value_ref.type() != simdjson::json_type::number) {
                    SerializeErrorCode(writer, (long)ErrorCode::kInvalidEmbeddingDataType, fmt::format("Embedding element type should be integer"));
                    return {0, nullptr};
                }
                switch (value_ref.get_number_type()) {
                    case simdjson::number_type::signed_integer:
                    case simdjson::number_type::unsigned_integer: {
                        embedding_data_ptr[idx++] = value_ref.get<i64>();
                        break;
                    }
                    default: {
                        SerializeErrorCode(writer,
                                           (long)ErrorCode::kInvalidEmbeddingDataType,
                                           fmt::format("Embedding element type should be integer"));
                        return {0, nullptr};
                    }
                }
            }

            u8 *res = embedding_data_ptr;
            embedding_data_ptr = nullptr;
            return {dimension, res};
        }
        case EmbeddingDataType::kElemFloat: {
            f32 *embedding_data_ptr = new f32[dimension];
            DeferFn defer_free_embedding([&]() {
                if (embedding_data_ptr != nullptr) {
                    delete[] embedding_data_ptr;
                    embedding_data_ptr = nullptr;
                }
            });
            for (SizeT idx = 0; auto value_ref : doc.get_array()) {
                if (value_ref.type() != simdjson::json_type::number) {
                    SerializeErrorCode(writer, (long)ErrorCode::kInvalidEmbeddingDataType, fmt::format("Embedding element type should be float"));
                    return {0, nullptr};
                }
                switch (value_ref.get_number_type()) {
                    case simdjson::number_type::floating_point_number: {
                        embedding_data_ptr[idx++] = value_ref.get<double>();
                        break;
                    }
                    case simdjson::number_type::signed_integer:
                    case simdjson::number_type::unsigned_integer: {
                        embedding_data_ptr[idx++] = value_ref.get<i64>();
                        break;
                    }
                    default: {
                        SerializeErrorCode(writer, (long)ErrorCode::kInvalidEmbeddingDataType, fmt::format("Embedding element type should be float"));
                        return {0, nullptr};
                    }
                }
            }

            f32 *res = embedding_data_ptr;
            embedding_data_ptr = nullptr;
            return {dimension, res};
        }
        case EmbeddingDataType::kElemFloat16: {
            Float16T *embedding_data_ptr = new Float16T[dimension];
            DeferFn defer_free_embedding([&]() {
                if (embedding_data_ptr != nullptr) {
                    delete[] embedding_data_ptr;
                    embedding_data_ptr = nullptr;
                }
            });
            for (SizeT idx = 0; auto value_ref : doc.get_array()) {
                if (value_ref.type() != simdjson::json_type::number) {
                    SerializeErrorCode(writer, (long)ErrorCode::kInvalidEmbeddingDataType, fmt::format("Embedding element type should be float"));
                    return {0, nullptr};
                }
                switch (value_ref.get_number_type()) {
                    case simdjson::number_type::floating_point_number: {
                        embedding_data_ptr[idx++] = value_ref.get<double>();
                        break;
                    }
                    case simdjson::number_type::signed_integer:
                    case simdjson::number_type::unsigned_integer: {
                        embedding_data_ptr[idx++] = value_ref.get<i64>();
                        break;
                    }
                    default: {
                        SerializeErrorCode(writer, (long)ErrorCode::kInvalidEmbeddingDataType, fmt::format("Embedding element type should be float"));
                        return {0, nullptr};
                    }
                }
            }

            Float16T *res = embedding_data_ptr;
            embedding_data_ptr = nullptr;
            return {dimension, res};
        }
        case EmbeddingDataType::kElemBFloat16: {
            BFloat16T *embedding_data_ptr = new BFloat16T[dimension];
            DeferFn defer_free_embedding([&]() {
                if (embedding_data_ptr != nullptr) {
                    delete[] embedding_data_ptr;
                    embedding_data_ptr = nullptr;
                }
            });
            for (SizeT idx = 0; auto value_ref : doc.get_array()) {
                if (value_ref.type() != simdjson::json_type::number) {
                    SerializeErrorCode(writer, (long)ErrorCode::kInvalidEmbeddingDataType, fmt::format("Embedding element type should be float"));
                    return {0, nullptr};
                }
                switch (value_ref.get_number_type()) {
                    case simdjson::number_type::floating_point_number: {
                        embedding_data_ptr[idx++] = value_ref.get<double>();
                        break;
                    }
                    case simdjson::number_type::signed_integer:
                    case simdjson::number_type::unsigned_integer: {
                        embedding_data_ptr[idx++] = value_ref.get<i64>();
                        break;
                    }
                    default: {
                        SerializeErrorCode(writer, (long)ErrorCode::kInvalidEmbeddingDataType, fmt::format("Embedding element type should be float"));
                        return {0, nullptr};
                    }
                }
            }

            BFloat16T *res = embedding_data_ptr;
            embedding_data_ptr = nullptr;
            return {dimension, res};
        }
        default: {
            SerializeErrorCode(writer, (long)ErrorCode::kInvalidEmbeddingDataType, fmt::format("Only support float as the embedding data type"));
            return {0, nullptr};
        }
    }
}

} // namespace infinity
