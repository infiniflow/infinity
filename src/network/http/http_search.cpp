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

namespace infinity {

void HTTPSearch::Process(Infinity *infinity_ptr,
                         const String &db_name,
                         const String &table_name,
                         const String &input_json_str,
                         HTTPStatus &http_status,
                         nlohmann::json &response) {
    http_status = HTTPStatus::CODE_500;
    try {
        nlohmann::json input_json = nlohmann::json::parse(input_json_str);
        if (!input_json.is_object()) {
            response["error_code"] = ErrorCode::kInvalidJsonFormat;
            response["error_message"] = "HTTP Body isn't json object";
        }
        UniquePtr<ParsedExpr> filter{};
        UniquePtr<ParsedExpr> limit{};
        UniquePtr<ParsedExpr> offset{};
        UniquePtr<SearchExpr> search_expr{};
        Vector<ParsedExpr *> *output_columns{nullptr};
        Vector<OrderByExpr *> *order_by_list{nullptr};
        DeferFn defer_fn([&]() {
            if (output_columns != nullptr) {
                for (auto &expr : *output_columns) {
                    delete expr;
                }
                delete output_columns;
                output_columns = nullptr;
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

        for (const auto &elem : input_json.items()) {
            String key = elem.key();
            ToLower(key);
            if (IsEqual(key, "output")) {
                if (output_columns != nullptr) {
                    response["error_code"] = ErrorCode::kInvalidExpression;
                    response["error_message"] = "More than one output field.";
                    return;
                }
                auto &output_list = elem.value();
                if (!output_list.is_array()) {
                    response["error_code"] = ErrorCode::kInvalidExpression;
                    response["error_message"] = "Output field should be array";
                    return;
                }

                output_columns = ParseOutput(output_list, http_status, response);
                if (output_columns == nullptr) {
                    return;
                }
            } else if (IsEqual(key, "sort")) {
                if (order_by_list != nullptr) {
                    response["error_code"] = ErrorCode::kInvalidExpression;
                    response["error_message"] = "More than one sort field.";
                    return;
                }

                order_by_list = ParseSort(elem.value(), http_status, response);
                if (order_by_list == nullptr) {
                    return;
                }
            } else if (IsEqual(key, "filter")) {

                if (filter) {
                    response["error_code"] = ErrorCode::kInvalidExpression;
                    response["error_message"] = "More than one filter field.";
                    return;
                }
                filter = ParseFilter(elem.value(), http_status, response);
                if (!filter) {
                    return;
                }
            } else if (IsEqual(key, "limit")) {

                if (limit) {
                    response["error_code"] = ErrorCode::kInvalidExpression;
                    response["error_message"] = "More than one limit field.";
                    return;
                }
                limit = ParseFilter(elem.value(), http_status, response);
                if (!limit) {
                    return;
                }
            } else if (IsEqual(key, "offset")) {

                if (offset) {
                    response["error_code"] = ErrorCode::kInvalidExpression;
                    response["error_message"] = "More than one offset field.";
                    return;
                }
                offset = ParseFilter(elem.value(), http_status, response);
                if (!offset) {
                    return;
                }
            } else if (IsEqual(key, "search")) {
                if (search_expr) {
                    response["error_code"] = ErrorCode::kInvalidExpression;
                    response["error_message"] = "More than one search field.";
                    return;
                }
                search_expr = ParseSearchExpr(elem.value(), http_status, response);
                if (!search_expr) {
                    return;
                }
            } else {
                response["error_code"] = ErrorCode::kInvalidExpression;
                response["error_message"] = "Unknown expression: " + key;
                return;
            }
        }

        const QueryResult result =
            infinity_ptr->Search(db_name, table_name, search_expr.release(), filter.release(), limit.release(), offset.release(), output_columns, order_by_list);

        output_columns = nullptr;
        order_by_list = nullptr;
        if (result.IsOk()) {
            SizeT block_rows = result.result_table_->DataBlockCount();
            for (SizeT block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                auto column_cnt = result.result_table_->ColumnCount();

                for (int row = 0; row < row_count; ++row) {
                    nlohmann::json json_result_row;
                    for (SizeT col = 0; col < column_cnt; ++col) {
                        Value value = data_block->GetValue(col, row);
                        const String &column_name = result.result_table_->GetColumnNameById(col);
                        const String &column_value = value.ToString();
                        json_result_row[column_name] = column_value;
                    }
                    response["output"].push_back(json_result_row);
                }
            }

            response["error_code"] = 0;
            http_status = HTTPStatus::CODE_200;
        } else {
            response["error_code"] = result.ErrorCode();
            response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
    } catch (nlohmann::json::exception &e) {
        response["error_code"] = ErrorCode::kInvalidJsonFormat;
        response["error_message"] = e.what();
    }
    return;
}

void HTTPSearch::Explain(Infinity *infinity_ptr,
                         const String &db_name,
                         const String &table_name,
                         const String &input_json_str,
                         HTTPStatus &http_status,
                         nlohmann::json &response) {
    http_status = HTTPStatus::CODE_500;
    try {
        nlohmann::json input_json = nlohmann::json::parse(input_json_str);
        if (!input_json.is_object()) {
            response["error_code"] = ErrorCode::kInvalidJsonFormat;
            response["error_message"] = "HTTP Body isn't json object";
        }
        UniquePtr<ParsedExpr> filter{};
        UniquePtr<ParsedExpr> limit{};
        UniquePtr<ParsedExpr> offset{};
        UniquePtr<SearchExpr> search_expr{};
        Vector<ParsedExpr *> *output_columns{nullptr};
        DeferFn defer_fn([&]() {
            if (output_columns != nullptr) {
                for (auto &expr : *output_columns) {
                    delete expr;
                }
                delete output_columns;
                output_columns = nullptr;
            }
        });
        ExplainType explain_type = ExplainType::kInvalid;
        for (const auto &elem : input_json.items()) {
            String key = elem.key();
            ToLower(key);
            if (IsEqual(key, "output")) {
                if (output_columns != nullptr) {
                    response["error_code"] = ErrorCode::kInvalidExpression;
                    response["error_message"] = "More than one output field.";
                    return;
                }
                auto &output_list = elem.value();
                if (!output_list.is_array()) {
                    response["error_code"] = ErrorCode::kInvalidExpression;
                    response["error_message"] = "Output field should be array";
                    return;
                }

                output_columns = ParseOutput(output_list, http_status, response);
                if (output_columns == nullptr) {
                    return;
                }
            } else if (IsEqual(key, "filter")) {

                if (filter != nullptr) {
                    response["error_code"] = ErrorCode::kInvalidExpression;
                    response["error_message"] = "More than one output field.";
                    return;
                }
                filter = ParseFilter(elem.value(), http_status, response);
                if (filter == nullptr) {
                    return;
                }
            } else if (IsEqual(key, "limit")) {

                if (limit) {
                    response["error_code"] = ErrorCode::kInvalidExpression;
                    response["error_message"] = "More than one limit field.";
                    return;
                }
                limit = ParseFilter(elem.value(), http_status, response);
                if (!limit) {
                    return;
                }
            } else if (IsEqual(key, "offset")) {

                if (offset) {
                    response["error_code"] = ErrorCode::kInvalidExpression;
                    response["error_message"] = "More than one offset field.";
                    return;
                }
                offset = ParseFilter(elem.value(), http_status, response);
                if (!offset) {
                    return;
                }
            } else if (IsEqual(key, "search")) {
                if (search_expr) {
                    response["error_code"] = ErrorCode::kInvalidExpression;
                    response["error_message"] = "More than one search field.";
                    return;
                }
                search_expr = ParseSearchExpr(elem.value(), http_status, response);
                if (!search_expr) {
                    return;
                }
            } else if (IsEqual(key, "explain_type")) {
                String type = elem.value();
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
                response["error_code"] = ErrorCode::kInvalidExpression;
                response["error_message"] = "Unknown expression: " + key;
                return;
            }
        }

        const QueryResult result = infinity_ptr->Explain(db_name,
                                                         table_name,
                                                         explain_type,
                                                         search_expr.release(),
                                                         filter.release(),
                                                         limit.release(),
                                                         offset.release(),
                                                         output_columns);

        output_columns = nullptr;
        if (result.IsOk()) {
            SizeT block_rows = result.result_table_->DataBlockCount();
            for (SizeT block_id = 0; block_id < block_rows; ++block_id) {
                DataBlock *data_block = result.result_table_->GetDataBlockById(block_id).get();
                auto row_count = data_block->row_count();
                auto column_cnt = result.result_table_->ColumnCount();

                for (int row = 0; row < row_count; ++row) {
                    nlohmann::json json_result_row;
                    for (SizeT col = 0; col < column_cnt; ++col) {
                        Value value = data_block->GetValue(col, row);
                        const String &column_name = result.result_table_->GetColumnNameById(col);
                        const String &column_value = value.ToString();
                        json_result_row[column_name] = column_value;
                    }
                    response["output"].push_back(json_result_row);
                }
            }

            response["error_code"] = 0;
            http_status = HTTPStatus::CODE_200;
        } else {
            response["error_code"] = result.ErrorCode();
            response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
    } catch (nlohmann::json::exception &e) {
        response["error_code"] = ErrorCode::kInvalidJsonFormat;
        response["error_message"] = e.what();
    }
    return;
}

UniquePtr<ParsedExpr> HTTPSearch::ParseFilter(const nlohmann::json &json_object, HTTPStatus &http_status, nlohmann::json &response) {
    if (!json_object.is_string()) {
        response["error_code"] = ErrorCode::kInvalidExpression;
        response["error_message"] = "Filter field should be string";
        return nullptr;
    }
    UniquePtr<ExpressionParserResult> expr_parsed_result = MakeUnique<ExpressionParserResult>();
    ExprParser expr_parser;
    expr_parser.Parse(json_object, expr_parsed_result.get());
    if (expr_parsed_result->IsError() || expr_parsed_result->exprs_ptr_->size() != 1) {
        response["error_code"] = ErrorCode::kInvalidExpression;
        response["error_message"] = fmt::format("Invalid expression: {}", json_object);
        return nullptr;
    }

    ParsedExpr *res = expr_parsed_result->exprs_ptr_->at(0);
    expr_parsed_result->exprs_ptr_->at(0) = nullptr;
    UniquePtr<ParsedExpr> filter_expr(res);
    return filter_expr;
}

Vector<ParsedExpr *> *HTTPSearch::ParseOutput(const nlohmann::json &output_list, HTTPStatus &http_status, nlohmann::json &response) {

    Vector<ParsedExpr *> *output_columns = new Vector<ParsedExpr *>();
    DeferFn defer_fn([&]() {
        if (output_columns != nullptr) {
            for (auto &expr : *output_columns) {
                delete expr;
            }
            delete output_columns;
            output_columns = nullptr;
        }
    });

    output_columns->reserve(output_list.size());
    for (const auto &output_expr_str : output_list) {
        if (!output_expr_str.is_string()) {
            response["error_code"] = ErrorCode::kInvalidExpression;
            response["error_message"] = fmt::format("Invalid expression: {}", output_expr_str);
            return nullptr;
        }

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
                response["error_code"] = ErrorCode::kInvalidExpression;
                response["error_message"] = fmt::format("Invalid expression: {}", output_expr_str);
                return nullptr;
            }

            output_columns->emplace_back(expr_parsed_result->exprs_ptr_->at(0));
            expr_parsed_result->exprs_ptr_->at(0) = nullptr;
        }
    }

    // Avoiding DeferFN auto free the output expressions
    Vector<ParsedExpr *> *res = output_columns;
    output_columns = nullptr;
    return res;
}

Vector<OrderByExpr *> *HTTPSearch::ParseSort(const nlohmann::json &json_object, HTTPStatus &http_status, nlohmann::json &response) {
    if (!json_object.is_object()) {
        response["error_code"] = ErrorCode::kInvalidExpression;
        response["error_message"] = fmt::format("Sort expression must be a json object: {}", json_object);
        return nullptr;
    }

    Vector<ParsedExpr *> *order_by_list = new Vector<OrderByExpr *>();
    DeferFn defer_fn([&]() {
        if (order_by_list != nullptr) {
            for (auto &expr : *order_by_list) {
                delete expr;
            }
            delete order_by_list;
            order_by_list = nullptr;
        }
    });

    order_by_list->reserve(json_object.items().size());

    for (const auto &expression : json_object.items()) {
        String key = expression.key();
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
                response["error_code"] = ErrorCode::kInvalidExpression;
                response["error_message"] = fmt::format("Invalid expression: {}", key);
                return nullptr;
            }

            order_by_expr->expr_ = expr_parsed_result->exprs_ptr_->at(0);
            expr_parsed_result->exprs_ptr_->at(0) = nullptr;
        }

        String value = expression.value();
        ToLower(value);
        if (value == "asc") {
            order_by_expr->order_ = OrderType::kAsc;
        } else if (value == "desc") {
            order_by_expr->order_ = OrderType::kDesc;
        } else {
            response["error_code"] = ErrorCode::kInvalidExpression;
            response["error_message"] = fmt::format("Invalid expression: {}", value);
            return nullptr;
        }

        order_by_list->emplace_back(order_by_expr.release());
    }

    // Avoiding DeferFN auto free the output expressions
    Vector<OrderByExpr *> *res = order_by_list;
    order_by_list = nullptr;
    return res;
}

UniquePtr<SearchExpr> HTTPSearch::ParseSearchExpr(const nlohmann::json &json_object, HTTPStatus &http_status, nlohmann::json &response) {
    if (json_object.type() != nlohmann::json::value_t::array) {
        response["error_code"] = ErrorCode::kInvalidExpression;
        response["error_message"] = "Search field should be list";
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
    child_expr->reserve(json_object.size());
    for (const auto &sub_search_it : json_object.items()) {
        const auto &search_obj = sub_search_it.value();
        if (!search_obj.is_object()) {
            response["error_code"] = ErrorCode::kInvalidExpression;
            response["error_message"] = "Search field should be list of objects";
            return {};
        }
        if (search_obj.contains("fusion_method") && search_obj.contains("match_method")) {
            response["error_code"] = ErrorCode::kInvalidExpression;
            response["error_message"] = "Every single search expression should not contain both fusion_method and match_method fields";
            return {};
        }
        if (search_obj.contains("fusion_method")) {
            auto fusion_expr = ParseFusion(search_obj, http_status, response);
            if (!fusion_expr) {
                return {};
            }
            child_expr->push_back(fusion_expr.release());
        } else {
            // match type
            String match_method = search_obj.at("match_method");
            ToLower(match_method);
            if (match_method == "dense") {
                auto match_dense_expr = ParseMatchDense(search_obj, http_status, response);
                if (!match_dense_expr) {
                    return {};
                }
                child_expr->push_back(match_dense_expr.release());
            } else if (match_method == "sparse") {
                auto match_sparse_expr = ParseMatchSparse(search_obj, http_status, response);
                if (!match_sparse_expr) {
                    return {};
                }
                child_expr->push_back(match_sparse_expr.release());
            } else if (match_method == "text") {
                auto match_text_expr = ParseMatchText(search_obj, http_status, response);
                if (!match_text_expr) {
                    return {};
                }
                child_expr->push_back(match_text_expr.release());
            } else if (match_method == "tensor") {
                auto match_tensor_expr = ParseMatchTensor(search_obj, http_status, response);
                if (!match_tensor_expr) {
                    return {};
                }
                child_expr->push_back(match_tensor_expr.release());
            } else {
                response["error_code"] = ErrorCode::kInvalidExpression;
                response["error_message"] = fmt::format("Unknown match method: {}", match_method);
                return {};
            }
        }
    }
    auto search_expr = MakeUnique<SearchExpr>();
    try {
        search_expr->SetExprs(child_expr);
        child_expr = nullptr;
    } catch (std::exception &e) {
        response["error_code"] = ErrorCode::kInvalidExpression;
        response["error_message"] = fmt::format("Invalid Search expression, error info: {}", e.what());
        return nullptr;
    }
    return search_expr;
}

UniquePtr<FusionExpr> HTTPSearch::ParseFusion(const nlohmann::json &json_object, HTTPStatus &http_status, nlohmann::json &response) {
    if (!json_object.is_object()) {
        response["error_code"] = ErrorCode::kInvalidExpression;
        response["error_message"] = fmt::format("Fusion expression must be a json object: {}", json_object);
        return nullptr;
    }
    i64 topn = -1;
    nlohmann::json fusion_params;
    auto fusion_expr = MakeUnique<FusionExpr>();
    // must have: "fusion_method", "topn"
    // may have: "params"
    constexpr std::array possible_keys{"fusion_method", "topn", "params"};
    std::set<String> possible_keys_set(possible_keys.begin(), possible_keys.end());
    for (const auto &expression : json_object.items()) {
        String key = expression.key();
        ToLower(key);
        if (!possible_keys_set.erase(key)) {
            response["error_code"] = ErrorCode::kInvalidExpression;
            response["error_message"] = fmt::format("Unknown Fusion expression key: {}", key);
            return nullptr;
        }
        if (IsEqual(key, "fusion_method")) {
            String method_str = expression.value();
            ToLower(method_str);
            fusion_expr->method_ = std::move(method_str);
        } else if (IsEqual(key, "topn")) {
            if (!expression.value().is_number_integer()) {
                response["error_code"] = ErrorCode::kInvalidExpression;
                response["error_message"] = "Fusion expression topn field should be integer";
                return nullptr;
            }
            topn = expression.value().get<i64>();
        } else if (IsEqual(key, "params")) {
            const auto &params = expression.value();
            if (!params.is_object()) {
                response["error_code"] = ErrorCode::kInvalidExpression;
                response["error_message"] = "Fusion params should be object";
                return nullptr;
            }
            fusion_params = params;
        }
    }
    // "params" is optional
    possible_keys_set.erase("params");
    // check if all required fields are set
    if (!possible_keys_set.empty()) {
        response["error_code"] = ErrorCode::kInvalidExpression;
        response["error_message"] =
            fmt::format("Invalid Fusion expression: following fields are required but not found: {}", fmt::join(possible_keys_set, ", "));
        return nullptr;
    }
    if (topn <= 0) {
        response["error_code"] = ErrorCode::kInvalidTopKType;
        response["error_message"] = "Fusion expression topn field should be positive integer";
        return nullptr;
    }
    String extra_params_str{};
    if (fusion_expr->method_ == "match_tensor") {
        nlohmann::json match_tensor_json = fusion_params;
        match_tensor_json["match_method"] = "tensor";
        if (match_tensor_json.contains("topn")) {
            response["error_code"] = ErrorCode::kInvalidExpression;
            response["error_message"] = "Fusion expression topn field should not be set in params";
            return nullptr;
        }
        match_tensor_json["topn"] = topn;
        auto match_tensor_expr = ParseMatchTensor(match_tensor_json, http_status, response);
        if (!match_tensor_expr) {
            return nullptr;
        }
        fusion_expr->match_tensor_expr_ = std::move(match_tensor_expr);
    } else {
        for (auto &param : fusion_params.items()) {
            String param_k = param.key(), param_v = param.value();
            extra_params_str += fmt::format(";{}={}", param_k, param_v);
        }
    }
    fusion_expr->SetOptions(fmt::format("topn={}{}", topn, extra_params_str));
    return fusion_expr;
}

UniquePtr<KnnExpr> HTTPSearch::ParseMatchDense(const nlohmann::json &json_object, HTTPStatus &http_status, nlohmann::json &response) {
    if (!json_object.is_object()) {
        response["error_code"] = ErrorCode::kInvalidExpression;
        response["error_message"] = "MatchDense field should be object";
        return nullptr;
    }
    auto knn_expr = MakeUnique<KnnExpr>();
    i64 topn = -1;
    nlohmann::json query_vector_json;
    // must have: "match_method", "fields", "query_vector", "element_type", "metric_type", "topn"
    // may have: "params"
    constexpr std::array possible_keys{"match_method", "fields", "query_vector", "element_type", "metric_type", "topn", "params"};
    std::set<String> possible_keys_set(possible_keys.begin(), possible_keys.end());
    for (auto &field_json_obj : json_object.items()) {
        String key = field_json_obj.key();
        ToLower(key);
        if (!possible_keys_set.erase(key)) {
            response["error_code"] = ErrorCode::kInvalidExpression;
            response["error_message"] = fmt::format("Unknown or duplicate MatchDense expression key: {}", key);
            return nullptr;
        }
        if (IsEqual(key, "match_method")) {
            String match_method = field_json_obj.value();
            ToLower(match_method);
            if (!IsEqual(match_method, "dense")) {
                response["error_code"] = ErrorCode::kInvalidExpression;
                response["error_message"] = fmt::format("MatchDense expression match_method should be dense, but got: {}", match_method);
                return nullptr;
            }
        } else if (IsEqual(key, "fields")) {
            auto column_expr = MakeUnique<ColumnExpr>();
            auto column_str = field_json_obj.value().get<String>();
            column_expr->names_.push_back(std::move(column_str));
            knn_expr->column_expr_ = column_expr.release();
        } else if (IsEqual(key, "query_vector")) {
            query_vector_json = field_json_obj.value();
        } else if (IsEqual(key, "element_type")) {
            String element_type = field_json_obj.value();
            ToUpper(element_type);
            try {
                knn_expr->embedding_data_type_ = EmbeddingT::String2EmbeddingDataType(element_type);
            } catch (std::exception &e) {
                response["error_code"] = ErrorCode::kInvalidExpression;
                response["error_message"] = fmt::format("Not supported vector element type: {}", element_type);
                return nullptr;
            }
        } else if (IsEqual(key, "metric_type")) {
            String metric_type = field_json_obj.value();
            ToLower(metric_type);
            if (!knn_expr->InitDistanceType(metric_type.c_str())) {
                response["error_code"] = ErrorCode::kInvalidExpression;
                response["error_message"] = fmt::format("Unknown knn distance: {}", metric_type);
                return nullptr;
            }
        } else if (IsEqual(key, "topn")) {
            if (!field_json_obj.value().is_number_integer()) {
                response["error_code"] = ErrorCode::kInvalidExpression;
                response["error_message"] = "MatchDense topn field should be integer";
                return nullptr;
            }
            topn = field_json_obj.value().get<i64>();
        } else if (IsEqual(key, "params")) {
            const auto &params = field_json_obj.value();
            if (!params.is_object()) {
                response["error_code"] = ErrorCode::kInvalidExpression;
                response["error_message"] = "MatchDense params should be object";
                return nullptr;
            }
            for (const auto &param_it : params.items()) {
                const auto &param_k = param_it.key();
                const auto &param_v = param_it.value();
                if (param_k == "filter") {
                    if (knn_expr->filter_expr_) {
                        response["error_code"] = ErrorCode::kInvalidExpression;
                        response["error_message"] = "More than one filter field in match dense params.";
                        return nullptr;
                    }
                    knn_expr->filter_expr_ = ParseFilter(param_v, http_status, response);
                    if (!knn_expr->filter_expr_) {
                        return nullptr;
                    }
                    // do not put it into opt_params_
                    continue;
                }
                if (param_k == "index_name") {
                    knn_expr->index_name_ = param_v;
                }
                if (param_k == "ignore_index" && param_v.get<String>() == "true") {
                    knn_expr->ignore_index_ = true;
                }
                if (knn_expr->opt_params_ == nullptr) {
                    knn_expr->opt_params_ = new Vector<InitParameter *>();
                }
                auto parameter = MakeUnique<InitParameter>();
                parameter->param_name_ = param_k;
                parameter->param_value_ = param_v;
                knn_expr->opt_params_->emplace_back(parameter.release());
            }
        }
    }
    // "params" is optional
    possible_keys_set.erase("params");
    // check if all required fields are set
    if (!possible_keys_set.empty()) {
        response["error_code"] = ErrorCode::kInvalidExpression;
        response["error_message"] =
            fmt::format("Invalid MatchDense expression: following fields are required but not found: {}", fmt::join(possible_keys_set, ", "));
        return nullptr;
    }
    if (topn <= 0) {
        response["error_code"] = ErrorCode::kInvalidTopKType;
        response["error_message"] = "MatchDense expression topn field should be positive integer";
        return nullptr;
    }
    knn_expr->topn_ = topn;
    const auto [dimension, embedding_ptr] = ParseVector(query_vector_json, knn_expr->embedding_data_type_, http_status, response);
    if (embedding_ptr == nullptr) {
        return nullptr;
    }
    knn_expr->dimension_ = dimension;
    knn_expr->embedding_data_ptr_ = embedding_ptr;
    return knn_expr;
}

UniquePtr<MatchExpr> HTTPSearch::ParseMatchText(const nlohmann::json &json_object, HTTPStatus &http_status, nlohmann::json &response) {
    if (!json_object.is_object()) {
        response["error_code"] = ErrorCode::kInvalidExpression;
        response["error_message"] = "MatchText field should be object";
        return nullptr;
    }
    auto match_expr = MakeUnique<MatchExpr>();
    i64 topn = -1;
    String extra_params{};
    // must have: "match_method", "fields", "matching_text", "topn"
    // may have: "params"
    constexpr std::array possible_keys{"match_method", "fields", "matching_text", "topn", "params"};
    std::set<String> possible_keys_set(possible_keys.begin(), possible_keys.end());
    for (auto &field_json_obj : json_object.items()) {
        String key = field_json_obj.key();
        ToLower(key);
        if (!possible_keys_set.erase(key)) {
            response["error_code"] = ErrorCode::kInvalidExpression;
            response["error_message"] = fmt::format("Unknown or duplicate MatchText expression key: {}", key);
            return nullptr;
        }
        if (IsEqual(key, "match_method")) {
            String match_method = field_json_obj.value();
            ToLower(match_method);
            if (!IsEqual(match_method, "text")) {
                response["error_code"] = ErrorCode::kInvalidExpression;
                response["error_message"] = fmt::format("MatchText expression match_method should be text, but got: {}", match_method);
                return nullptr;
            }
        } else if (IsEqual(key, "fields")) {
            match_expr->fields_ = field_json_obj.value();
        } else if (IsEqual(key, "matching_text")) {
            match_expr->matching_text_ = field_json_obj.value();
        } else if (IsEqual(key, "topn")) {
            if (!field_json_obj.value().is_number_integer()) {
                response["error_code"] = ErrorCode::kInvalidExpression;
                response["error_message"] = "MatchText topn field should be integer";
                return nullptr;
            }
            topn = field_json_obj.value().get<i64>();
        } else if (IsEqual(key, "params")) {
            const auto &params = field_json_obj.value();
            if (!params.is_object()) {
                response["error_code"] = ErrorCode::kInvalidExpression;
                response["error_message"] = "MatchText params should be object";
                return nullptr;
            }
            for (auto &param : params.items()) {
                String param_k = param.key(), param_v = param.value();
                if (param_k == "filter") {
                    if (match_expr->filter_expr_) {
                        response["error_code"] = ErrorCode::kInvalidExpression;
                        response["error_message"] = "More than one filter field in match text params.";
                        return nullptr;
                    }
                    match_expr->filter_expr_ = ParseFilter(param.value(), http_status, response);
                    if (!match_expr->filter_expr_) {
                        return nullptr;
                    }
                    // do not put it into extra_params
                    continue;
                }
                extra_params += fmt::format(";{}={}", param_k, param_v);
            }
        }
    }
    // "params" is optional
    possible_keys_set.erase("params");
    // check if all required fields are set
    if (!possible_keys_set.empty()) {
        response["error_code"] = ErrorCode::kInvalidExpression;
        response["error_message"] =
            fmt::format("Invalid MatchText expression: following fields are required but not found: {}", fmt::join(possible_keys_set, ", "));
        return nullptr;
    }
    if (topn <= 0) {
        response["error_code"] = ErrorCode::kInvalidTopKType;
        response["error_message"] = "MatchText expression topn field should be positive integer";
        return nullptr;
    }
    match_expr->options_text_ = fmt::format("topn={}{}", topn, extra_params);
    return match_expr;
}

UniquePtr<MatchTensorExpr> HTTPSearch::ParseMatchTensor(const nlohmann::json &json_object, HTTPStatus &http_status, nlohmann::json &response) {
    if (!json_object.is_object()) {
        response["error_code"] = ErrorCode::kInvalidExpression;
        response["error_message"] = "MatchTensor field should be object";
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
    for (auto &field_json_obj : json_object.items()) {
        String key = field_json_obj.key();
        ToLower(key);
        if (!possible_keys_set.erase(key)) {
            response["error_code"] = ErrorCode::kInvalidExpression;
            response["error_message"] = fmt::format("Unknown or duplicate MatchTensor expression key: {}", key);
            return nullptr;
        }
        if (IsEqual(key, "match_method")) {
            String match_method = field_json_obj.value();
            ToLower(match_method);
            if (!IsEqual(match_method, "tensor")) {
                response["error_code"] = ErrorCode::kInvalidExpression;
                response["error_message"] = fmt::format("MatchTensor expression match_method should be tensor, but got: {}", match_method);
                return nullptr;
            }
        } else if (IsEqual(key, "field")) {
            auto column_str = field_json_obj.value().get<String>();
            auto column_expr = MakeUnique<ColumnExpr>();
            column_expr->names_.push_back(std::move(column_str));
            match_tensor_expr->column_expr_ = std::move(column_expr);
        } else if (IsEqual(key, "query_tensor")) {
            if (!json_object.contains("element_type")) {
                response["error_code"] = ErrorCode::kInvalidExpression;
                response["error_message"] = "Missing element_type for query_tensor";
                return nullptr;
            }
            try {
                tensor_expr = BuildConstantExprFromJson(field_json_obj.value());
            } catch (std::exception &e) {
                response["error_code"] = ErrorCode::kInvalidExpression;
                response["error_message"] = fmt::format("Invalid query_tensor, error info: {}", e.what());
                return nullptr;
            }
        } else if (IsEqual(key, "element_type")) {
            element_type = field_json_obj.value();
        } else if (IsEqual(key, "topn")) {
            if (!field_json_obj.value().is_number_integer()) {
                response["error_code"] = ErrorCode::kInvalidExpression;
                response["error_message"] = "MatchTensor topn field should be integer";
                return nullptr;
            }
            topn = field_json_obj.value().get<i64>();
        } else if (IsEqual(key, "params")) {
            const auto &params = field_json_obj.value();
            if (!params.is_object()) {
                response["error_code"] = ErrorCode::kInvalidExpression;
                response["error_message"] = "MatchTensor params should be object";
                return nullptr;
            }
            for (auto &param : params.items()) {
                String param_k = param.key(), param_v = param.value();
                if (param_k == "filter") {
                    if (match_tensor_expr->filter_expr_) {
                        response["error_code"] = ErrorCode::kInvalidExpression;
                        response["error_message"] = "More than one filter field in match tensor params.";
                        return nullptr;
                    }
                    match_tensor_expr->filter_expr_ = ParseFilter(param.value(), http_status, response);
                    if (!match_tensor_expr->filter_expr_) {
                        return nullptr;
                    }
                    // do not put it into extra_params
                    continue;
                }
                extra_params += fmt::format(";{}={}", param_k, param_v);
            }
        }
    }
    // "params" is optional
    possible_keys_set.erase("params");
    // check if all required fields are set
    if (!possible_keys_set.empty()) {
        response["error_code"] = ErrorCode::kInvalidExpression;
        response["error_message"] =
            fmt::format("Invalid MatchTensor expression: following fields are required but not found: {}", fmt::join(possible_keys_set, ", "));
        return nullptr;
    }
    if (topn <= 0) {
        response["error_code"] = ErrorCode::kInvalidTopKType;
        response["error_message"] = "MatchTensor expression topn field should be positive integer";
        return nullptr;
    }
    match_tensor_expr->options_text_ = fmt::format("topn={}{}", topn, extra_params);
    try {
        match_tensor_expr->SetQueryTensorStr(std::move(element_type), tensor_expr.get());
    } catch (std::exception &e) {
        response["error_code"] = ErrorCode::kInvalidExpression;
        response["error_message"] = fmt::format("Invalid query_tensor, error info: {}", e.what());
        return nullptr;
    }
    return match_tensor_expr;
}

UniquePtr<MatchSparseExpr> HTTPSearch::ParseMatchSparse(const nlohmann::json &json_object, HTTPStatus &http_status, nlohmann::json &response) {
    if (!json_object.is_object()) {
        response["error_code"] = ErrorCode::kInvalidExpression;
        response["error_message"] = "MatchSparse field should be object";
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
    for (auto &field_json_obj : json_object.items()) {
        String key = field_json_obj.key();
        ToLower(key);
        if (!possible_keys_set.erase(key)) {
            response["error_code"] = ErrorCode::kInvalidExpression;
            response["error_message"] = fmt::format("Unknown or duplicate MatchSparse expression key: {}", key);
            return nullptr;
        }
        if (IsEqual(key, "match_method")) {
            String match_method = field_json_obj.value();
            ToLower(match_method);
            if (!IsEqual(match_method, "sparse")) {
                response["error_code"] = ErrorCode::kInvalidExpression;
                response["error_message"] = fmt::format("MatchSparse expression match_method should be sparse, but got: {}", match_method);
                return nullptr;
            }
        } else if (IsEqual(key, "fields")) {
            auto column_expr = MakeUnique<ColumnExpr>();
            auto column_str = field_json_obj.value().get<String>();
            column_expr->names_.push_back(std::move(column_str));
            match_sparse_expr->column_expr_ = std::move(column_expr);
        } else if (IsEqual(key, "query_vector")) {
            UniquePtr<ConstantExpr> query_sparse_expr = ParseSparseVector(field_json_obj.value(), http_status, response);
            if (!query_sparse_expr) {
                return nullptr;
            }
            ConstantExpr *const_sparse_expr = query_sparse_expr.release();
            match_sparse_expr->SetQuerySparse(const_sparse_expr);
            assert(const_sparse_expr == nullptr);
        } else if (IsEqual(key, "metric_type")) {
            try {
                match_sparse_expr->SetMetricType(field_json_obj.value().get<String>());
            } catch (std::exception &e) {
                response["error_code"] = ErrorCode::kInvalidExpression;
                response["error_message"] = fmt::format("Invalid metric_type: {}, error info: {}", field_json_obj.value().get<String>(), e.what());
                return nullptr;
            }
        } else if (IsEqual(key, "topn")) {
            if (!field_json_obj.value().is_number_integer()) {
                response["error_code"] = ErrorCode::kInvalidExpression;
                response["error_message"] = "MatchSparse topn field should be integer";
                return nullptr;
            }
            topn = field_json_obj.value().get<i64>();
        } else if (IsEqual(key, "params")) {
            const auto &params = field_json_obj.value();
            if (!params.is_object()) {
                response["error_code"] = ErrorCode::kInvalidExpression;
                response["error_message"] = "MatchSparse params should be object";
                return nullptr;
            }
            for (auto &param : params.items()) {
                if (param.key() == "filter") {
                    if (match_sparse_expr->filter_expr_) {
                        response["error_code"] = ErrorCode::kInvalidExpression;
                        response["error_message"] = "More than one filter field in match sparse params.";
                        return nullptr;
                    }
                    match_sparse_expr->filter_expr_ = ParseFilter(param.value(), http_status, response);
                    if (!match_sparse_expr->filter_expr_) {
                        return nullptr;
                    }
                    // do not put it into opt_params_ptr
                    continue;
                }
                auto *init_parameter = new InitParameter();
                init_parameter->param_name_ = param.key();
                init_parameter->param_value_ = param.value();
                opt_params_ptr->emplace_back(init_parameter);
            }
        }
    }
    // "params" is optional
    possible_keys_set.erase("params");
    // check if all required fields are set
    if (!possible_keys_set.empty()) {
        response["error_code"] = ErrorCode::kInvalidExpression;
        response["error_message"] =
            fmt::format("Invalid MatchSparse expression: following fields are required but not found: {}", fmt::join(possible_keys_set, ", "));
        return nullptr;
    }
    if (topn <= 0) {
        response["error_code"] = ErrorCode::kInvalidTopKType;
        response["error_message"] = "MatchSparse expression topn field should be positive integer";
        return nullptr;
    }
    match_sparse_expr->SetOptParams(topn, opt_params_ptr);
    opt_params_ptr = nullptr;
    return match_sparse_expr;
}

UniquePtr<ConstantExpr> HTTPSearch::ParseSparseVector(const nlohmann::json &json_object, HTTPStatus &http_status, nlohmann::json &response) {
    if (!json_object.is_object()) {
        response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
        response["error_message"] = "Sparse vector should be object";
        return nullptr;
    }
    if (json_object.size() == 0) {
        response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
        response["error_message"] = "Empty sparse vector, cannot decide type";
        return nullptr;
    }
    UniquePtr<ConstantExpr> const_sparse_expr{};
    switch (json_object.begin().value().type()) {
        case nlohmann::json::value_t::number_unsigned:
        case nlohmann::json::value_t::number_integer: {
            const_sparse_expr = MakeUnique<ConstantExpr>(LiteralType::kLongSparseArray);
            break;
        }
        case nlohmann::json::value_t::number_float: {
            const_sparse_expr = MakeUnique<ConstantExpr>(LiteralType::kDoubleSparseArray);
            break;
        }
        default: {
            response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
            response["error_message"] = "Sparse value element type error";
            return nullptr;
        }
    }
    HashSet<i64> key_set;
    for (const auto &sparse_it : json_object.items()) {
        const auto &sparse_k = sparse_it.key();
        const auto &sparse_v = sparse_it.value();
        i64 key_val = {};
        try {
            key_val = std::stoll(sparse_k);
        } catch (std::exception &e) {
            response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
            response["error_message"] = fmt::format("Error when try to cast sparse key '{}' to integer, error info: {}", sparse_k, e.what());
            return nullptr;
        }
        if (const auto [_, insert_ok] = key_set.insert(key_val); !insert_ok) {
            response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
            response["error_message"] = fmt::format("Duplicate key {} in sparse array!", key_val);
            return nullptr;
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
            response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
            response["error_message"] = "Sparse value element type error";
            return nullptr;
        }
    }
    return const_sparse_expr;
}

Tuple<i64, void *>
HTTPSearch::ParseVector(const nlohmann::json &json_object, EmbeddingDataType elem_type, HTTPStatus &http_status, nlohmann::json &response) {
    if (!json_object.is_array()) {
        response["error_code"] = ErrorCode::kInvalidExpression;
        response["error_message"] = fmt::format("Can't recognize embedding/vector.");
        return {0, nullptr};
    }
    SizeT dimension = json_object.size();
    if (dimension == 0) {
        response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
        response["error_message"] = fmt::format("Empty embedding data");
        return {0, nullptr};
    }

    switch (elem_type) {
        case EmbeddingDataType::kElemInt32: {
            i32 *embedding_data_ptr = new i32[dimension];
            DeferFn defer_free_embedding([&]() {
                if (embedding_data_ptr != nullptr) {
                    delete[] embedding_data_ptr;
                    embedding_data_ptr = nullptr;
                }
            });
            for (SizeT idx = 0; idx < dimension; ++idx) {
                const auto &value_ref = json_object[idx];
                const auto &value_type = value_ref.type();
                switch (value_type) {
                    case nlohmann::json::value_t::number_integer:
                    case nlohmann::json::value_t::number_unsigned: {
                        embedding_data_ptr[idx] = value_ref.template get<i64>();
                        break;
                    }
                    default: {
                        response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                        response["error_message"] = fmt::format("Embedding element type should be integer");
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
            for (SizeT idx = 0; idx < dimension; ++idx) {
                const auto &value_ref = json_object[idx];
                const auto &value_type = value_ref.type();
                switch (value_type) {
                    case nlohmann::json::value_t::number_integer:
                    case nlohmann::json::value_t::number_unsigned: {
                        embedding_data_ptr[idx] = value_ref.template get<i64>();
                        break;
                    }
                    default: {
                        response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                        response["error_message"] = fmt::format("Embedding element type should be integer");
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
            for (SizeT idx = 0; idx < dimension; ++idx) {
                const auto &value_ref = json_object[idx];
                const auto &value_type = value_ref.type();
                switch (value_type) {
                    case nlohmann::json::value_t::number_integer:
                    case nlohmann::json::value_t::number_unsigned: {
                        embedding_data_ptr[idx] = value_ref.template get<i64>();
                        break;
                    }
                    default: {
                        response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                        response["error_message"] = fmt::format("Embedding element type should be integer");
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
            for (SizeT idx = 0; idx < dimension; ++idx) {
                const auto &value_ref = json_object[idx];
                const auto &value_type = value_ref.type();
                switch (value_type) {
                    case nlohmann::json::value_t::number_float: {
                        embedding_data_ptr[idx] = value_ref.template get<double>();
                        break;
                    }
                    case nlohmann::json::value_t::number_integer:
                    case nlohmann::json::value_t::number_unsigned: {
                        embedding_data_ptr[idx] = value_ref.template get<i64>();
                        break;
                    }
                    default: {
                        response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                        response["error_message"] = fmt::format("Embedding element type should be float");
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
            for (SizeT idx = 0; idx < dimension; ++idx) {
                const auto &value_ref = json_object[idx];
                const auto &value_type = value_ref.type();
                switch (value_type) {
                    case nlohmann::json::value_t::number_float: {
                        embedding_data_ptr[idx] = value_ref.template get<double>();
                        break;
                    }
                    case nlohmann::json::value_t::number_integer:
                    case nlohmann::json::value_t::number_unsigned: {
                        embedding_data_ptr[idx] = value_ref.template get<i64>();
                        break;
                    }
                    default: {
                        response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                        response["error_message"] = fmt::format("Embedding element type should be float");
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
            for (SizeT idx = 0; idx < dimension; ++idx) {
                const auto &value_ref = json_object[idx];
                const auto &value_type = value_ref.type();
                switch (value_type) {
                    case nlohmann::json::value_t::number_float: {
                        embedding_data_ptr[idx] = value_ref.template get<double>();
                        break;
                    }
                    case nlohmann::json::value_t::number_integer:
                    case nlohmann::json::value_t::number_unsigned: {
                        embedding_data_ptr[idx] = value_ref.template get<i64>();
                        break;
                    }
                    default: {
                        response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                        response["error_message"] = fmt::format("Embedding element type should be float");
                        return {0, nullptr};
                    }
                }
            }

            BFloat16T *res = embedding_data_ptr;
            embedding_data_ptr = nullptr;
            return {dimension, res};
        }
        default: {
            response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
            response["error_message"] = fmt::format("Only support float as the embedding data type");
            return {0, nullptr};
        }
    }
}

} // namespace infinity
