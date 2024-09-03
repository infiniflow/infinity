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

#include <expr/match_sparse_expr.h>
#include <string>

module http_search;

import stl;
import third_party;
import parsed_expr;
import knn_expr;
import match_tensor_expr;
import match_expr;
import search_expr;
import match_tensor_expr;
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

        SizeT match_expr_count = 0;
        SizeT fusion_expr_count = 0;
        Vector<ParsedExpr *> *output_columns{nullptr};
        Vector<ParsedExpr *> *search_exprs{nullptr};
        ParsedExpr *filter{nullptr};
        SearchExpr *search_expr{nullptr};
        DeferFn defer_fn([&]() {
            if (output_columns != nullptr) {
                for (auto &expr : *output_columns) {
                    delete expr;
                }
                delete output_columns;
                output_columns = nullptr;
            }
            if (filter != nullptr) {
                delete filter;
                filter = nullptr;
            }
            if (search_expr != nullptr) {
                delete search_expr;
                search_expr = nullptr;
            }
            if (search_exprs != nullptr) {
                for (auto &expr : *search_exprs) {
                    delete expr;
                    expr = nullptr;
                }
                delete search_exprs;
                search_exprs = nullptr;
            }
        });

        search_exprs = new Vector<ParsedExpr *>();
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
                    response["error_message"] = "More than one filter field.";
                    return;
                }

                auto &filter_json = elem.value();
                if (!filter_json.is_string()) {
                    response["error_code"] = ErrorCode::kInvalidExpression;
                    response["error_message"] = "Filter field should be string";
                    return;
                }

                filter = ParseFilter(filter_json, http_status, response);
                if (filter == nullptr) {
                    return;
                }
            } else if (IsEqual(key, "fusion")) {
                if (search_expr == nullptr) {
                    search_expr = new SearchExpr();
                }
                auto &fusion_json_list = elem.value();
                if (fusion_json_list.type() != nlohmann::json::value_t::array) {
                    response["error_code"] = ErrorCode::kInvalidExpression;
                    response["error_message"] = "Fusion field should be list";
                    return;
                }
                for (auto &fusion_json : fusion_json_list) {
                    const auto fusion_expr = ParseFusion(fusion_json, http_status, response);
                    if (fusion_expr == nullptr) {
                        return;
                    }
                    search_exprs->push_back(fusion_expr);
                    ++fusion_expr_count;
                }
            } else if (IsEqual(key, "match_dense")) {
                if (search_expr == nullptr) {
                    search_expr = new SearchExpr();
                }
                auto &knn_json = elem.value();
                const auto knn_expr = ParseKnn(knn_json, http_status, response);
                if (knn_expr == nullptr) {
                    return;
                }
                search_exprs->push_back(knn_expr);
                ++match_expr_count;
            } else if (IsEqual(key, "match_text")) {
                if (search_expr == nullptr) {
                    search_expr = new SearchExpr();
                }
                auto &match_json = elem.value();
                const auto match_expr = ParseMatch(match_json, http_status, response);
                if (match_expr == nullptr) {
                    return;
                }
                search_exprs->push_back(match_expr);
                ++match_expr_count;
            } else if (IsEqual(key, "match_tensor")) {
                if (search_expr == nullptr) {
                    search_expr = new SearchExpr();
                }
                auto &match_json = elem.value();
                const auto match_expr = ParseMatchTensor(match_json, http_status, response);
                if (match_expr == nullptr) {
                    return;
                }
                search_exprs->push_back(match_expr);
                ++match_expr_count;
            } else if (IsEqual(key, "match_sparse")) {
                if (search_expr == nullptr) {
                    search_expr = new SearchExpr();
                }
                auto &match_json = elem.value();
                const auto match_expr = ParseMatchSparse(match_json, http_status, response);
                if (match_expr == nullptr) {
                    return;
                }
                search_exprs->push_back(match_expr);
                ++match_expr_count;
            } else {
                response["error_code"] = ErrorCode::kInvalidExpression;
                response["error_message"] = "Unknown expression: " + key;
                return;
            }
        }

        if (match_expr_count > 1 && fusion_expr_count == 0) {
            response["error_code"] = ErrorCode::kInvalidExpression;
            response["error_message"] = "More than one knn or match experssion with no fusion experssion!";
            return;
        }

        if (search_exprs != nullptr && !search_exprs->empty()) {
            search_expr->SetExprs(search_exprs);
            search_exprs = nullptr;
        }
        const QueryResult result = infinity_ptr->Search(db_name, table_name, search_expr, filter, output_columns);

        output_columns = nullptr;
        filter = nullptr;
        search_expr = nullptr;
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

        SizeT match_expr_count = 0;
        SizeT fusion_expr_count = 0;
        Vector<ParsedExpr *> *output_columns{nullptr};
        Vector<ParsedExpr *> *search_exprs{nullptr};
        ParsedExpr *filter{nullptr};
        SearchExpr *search_expr{nullptr};
        DeferFn defer_fn([&]() {
            if (output_columns != nullptr) {
                for (auto &expr : *output_columns) {
                    delete expr;
                }
                delete output_columns;
                output_columns = nullptr;
            }
            if (filter != nullptr) {
                delete filter;
                filter = nullptr;
            }
            if (search_expr != nullptr) {
                delete search_expr;
                search_expr = nullptr;
            }
            if (search_exprs != nullptr) {
                for (auto &expr : *search_exprs) {
                    delete expr;
                    expr = nullptr;
                }
                delete search_exprs;
                search_exprs = nullptr;
            }
        });

        search_exprs = new Vector<ParsedExpr *>();
        ExplainType explain_type  = ExplainType::kInvalid;
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

                auto &filter_json = elem.value();
                if (!filter_json.is_string()) {
                    response["error_code"] = ErrorCode::kInvalidExpression;
                    response["error_message"] = "Filter field should be string";
                    return;
                }

                filter = ParseFilter(filter_json, http_status, response);
                if (filter == nullptr) {
                    return;
                }
            } else if (IsEqual(key, "fusion")) {
                if (search_expr == nullptr) {
                    search_expr = new SearchExpr();
                }
                auto &fusion_json_list = elem.value();
                if (fusion_json_list.type() != nlohmann::json::value_t::array) {
                    response["error_code"] = ErrorCode::kInvalidExpression;
                    response["error_message"] = "Fusion field should be list";
                    return;
                }
                for (auto &fusion_json : fusion_json_list) {
                    const auto fusion_expr = ParseFusion(fusion_json, http_status, response);
                    if (fusion_expr == nullptr) {
                        return;
                    }
                    search_exprs->push_back(fusion_expr);
                    ++fusion_expr_count;
                }
            } else if (IsEqual(key, "knn")) {
                if (search_expr == nullptr) {
                    search_expr = new SearchExpr();
                }
                auto &knn_json = elem.value();
                const auto knn_expr = ParseKnn(knn_json, http_status, response);
                if (knn_expr == nullptr) {
                    return;
                }
                search_exprs->push_back(knn_expr);
                ++match_expr_count;
            } else if (IsEqual(key, "match")) {
                if (search_expr == nullptr) {
                    search_expr = new SearchExpr();
                }
                auto &match_json = elem.value();
                const auto match_expr = ParseMatch(match_json, http_status, response);
                if (match_expr == nullptr) {
                    return;
                }
                search_exprs->push_back(match_expr);
                ++match_expr_count;
            } else if (IsEqual(key, "match_tensor")) {
                if (search_expr == nullptr) {
                    search_expr = new SearchExpr();
                }
                auto &match_json = elem.value();
                const auto match_expr = ParseMatchTensor(match_json, http_status, response);
                if (match_expr == nullptr) {
                    return;
                }
                search_exprs->push_back(match_expr);
                ++match_expr_count;
            } else if (IsEqual(key, "match_sparse")) {
                if (search_expr == nullptr) {
                    search_expr = new SearchExpr();
                }
                auto &match_json = elem.value();
                const auto match_expr = ParseMatchSparse(match_json, http_status, response);
                if (match_expr == nullptr) {
                    return;
                }
                search_exprs->push_back(match_expr);
                ++match_expr_count;
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

        if (match_expr_count > 1 && fusion_expr_count == 0) {
            response["error_code"] = ErrorCode::kInvalidExpression;
            response["error_message"] = "More than one knn or match experssion with no fusion experssion!";
            return;
        }

        if (search_exprs != nullptr && !search_exprs->empty()) {
            search_expr->SetExprs(search_exprs);
            search_exprs = nullptr;
        }
        const QueryResult result = infinity_ptr->Explain(db_name, table_name, explain_type, search_expr, filter, output_columns);

        output_columns = nullptr;
        filter = nullptr;
        search_expr = nullptr;
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

ParsedExpr *HTTPSearch::ParseFilter(const nlohmann::json &json_object, HTTPStatus &http_status, nlohmann::json &response) {

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
    return res;
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

        if(output_expr_str == "_row_id" or output_expr_str == "_similarity" or output_expr_str == "_distance" or output_expr_str == "_score") {
            auto parsed_expr = new FunctionExpr();
            if(output_expr_str == "_row_id") {
                parsed_expr->func_name_ = "row_id";
            } else if(output_expr_str == "_similarity") {
                parsed_expr->func_name_ = "similarity";
            } else if(output_expr_str == "_distance") {
                parsed_expr->func_name_ = "distance";
            } else if(output_expr_str == "_score") {
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
FusionExpr* HTTPSearch::ParseFusion(const nlohmann::json &fusion_json_object,
                             HTTPStatus &http_status,
                             nlohmann::json &response) {
    if (!fusion_json_object.is_object()) {
        response["error_code"] = ErrorCode::kInvalidExpression;
        response["error_message"] = fmt::format("Fusion expression must be a json object: {}", fusion_json_object);
        return nullptr;
    }

    u32 method_cnt = 0;

    UniquePtr<FusionExpr> fusion_expr = nullptr;
    for (const auto &expression : fusion_json_object.items()) {
        String key = expression.key();
        ToLower(key);
        if (IsEqual(key, "method")) {
            if (method_cnt++) {
                response["error_code"] = ErrorCode::kInvalidExpression;
                response["error_message"] = "Method is already given";
                return nullptr;
            }
            if (!fusion_expr) {
                fusion_expr = MakeUnique<FusionExpr>();
            }
            fusion_expr->method_ = expression.value();
        } else if (IsEqual(key, "option")) {
            if (!fusion_expr) {
                fusion_expr = MakeUnique<FusionExpr>();
            }
            fusion_expr->SetOptions(expression.value());
        } else if (IsEqual(key, "optional_match_tensor")) {
            if (!fusion_expr) {
                fusion_expr = MakeUnique<FusionExpr>();
            }
            auto &match_tensor_json = expression.value();
            const auto match_tensor_expr = ParseMatchTensor(match_tensor_json, http_status, response);
            if (match_tensor_expr == nullptr) {
                return nullptr;
            }
            fusion_expr->match_tensor_expr_.reset(match_tensor_expr);
        } else {
            response["error_code"] = ErrorCode::kInvalidExpression;
            response["error_message"] = "Error fusion clause";
            return nullptr;
        }
    }

    if (fusion_expr) {
        if (fusion_expr->method_.empty()) {
            response["error_code"] = ErrorCode::kInvalidExpression;
            response["error_message"] = "Error fusion clause : empty method";
            return nullptr;
        }
    }
    return fusion_expr.release();
}
KnnExpr *HTTPSearch::ParseMatchDense(const nlohmann::json &knn_json_object, HTTPStatus &http_status, nlohmann::json &response) {
    if (!knn_json_object.is_object()) {
        response["error_code"] = ErrorCode::kInvalidExpression;
        response["error_message"] = "KNN field should be object";
        return nullptr;
    }

    auto knn_expr = MakeUnique<KnnExpr>();

    for (auto &field_json_obj : knn_json_object.items()) {
        String key = field_json_obj.key();
        ToLower(key);
        if (IsEqual(key, "fields")) {
            auto column_expr = MakeUnique<ColumnExpr>();
            const auto &fields_value = field_json_obj.value();
            if (!fields_value.is_array()) {
                response["error_code"] = ErrorCode::kInvalidExpression;
                response["error_message"] = "Fields should be array";
                return nullptr;
            }
            for (SizeT i = 0; i < fields_value.size(); ++i) {
                column_expr->names_.emplace_back(fields_value[i]);
            }
            knn_expr->column_expr_ = column_expr.release();
        } else if (IsEqual(key, "query_vector")) {
            if (!knn_json_object.contains("element_type")) {
                response["error_code"] = ErrorCode::kInvalidExpression;
                response["error_message"] = "Missing element_type for query_vector";
                return nullptr;
            }
            String element_type = knn_json_object["element_type"];
            if (IsEqual(element_type, "float")) {
                knn_expr->embedding_data_type_ = EmbeddingDataType::kElemFloat;
            } else if(IsEqual(element_type, "float16")) {
                knn_expr->embedding_data_type_ = EmbeddingDataType::kElemFloat16;
            } else if(IsEqual(element_type, "bfloat16")) {
                knn_expr->embedding_data_type_ = EmbeddingDataType::kElemBFloat16;
            } else if(IsEqual(element_type, "double")) {
                knn_expr->embedding_data_type_ = EmbeddingDataType::kElemDouble;
            } else if(IsEqual(element_type, "integer")) {
                knn_expr->embedding_data_type_ = EmbeddingDataType::kElemInt32;
            } else if(IsEqual(element_type, "int8")) {
                knn_expr->embedding_data_type_ = EmbeddingDataType::kElemInt8;
            } else if(IsEqual(element_type, "uint8")) {
                knn_expr->embedding_data_type_ = EmbeddingDataType::kElemUInt8;
            } else {
                response["error_code"] = ErrorCode::kInvalidExpression;
                response["error_message"] = fmt::format("Not supported vector element type: {}", element_type);
                return nullptr;
            }

            auto [dimension, embedding_ptr] = ParseVector(field_json_obj.value(), knn_expr->embedding_data_type_, http_status, response);
            knn_expr->dimension_ = dimension;
            knn_expr->embedding_data_ptr_ = embedding_ptr;
        } else if (IsEqual(key, "element_type")) {
            ;
        } else if (IsEqual(key, "top_k")) {
            if (field_json_obj.value().is_number_unsigned()) {
                knn_expr->topn_ = field_json_obj.value();
            } else {
                response["error_code"] = ErrorCode::kInvalidTopKType;
                response["error_message"] = "Top K field should be integer";
                return nullptr;
            }
        } else if (IsEqual(key, "metric_type")) {
            String metric_type = field_json_obj.value();
            ToLower(metric_type);
            if (IsEqual(metric_type, "l2")) {
                knn_expr->distance_type_ = KnnDistanceType::kL2;
            } else if (IsEqual(metric_type, "inner_product") or IsEqual(metric_type, "ip")) {
                knn_expr->distance_type_ = KnnDistanceType::kInnerProduct;
            } else if (IsEqual(metric_type, "cosine") or IsEqual(metric_type, "cos")) {
                knn_expr->distance_type_ = KnnDistanceType::kCosine;
            } else if (IsEqual(metric_type, "hamming")) {
                knn_expr->distance_type_ = KnnDistanceType::kHamming;
            } else {
                response["error_code"] = ErrorCode::kInvalidExpression;
                response["error_message"] = fmt::format("Unknown knn distance: {}", metric_type);
                return nullptr;
            }
        } else {
            if (knn_expr->opt_params_ == nullptr) {
                knn_expr->opt_params_ = new Vector<InitParameter *>();
            }
            if(key  == "index_name") {
                knn_expr->index_name_ = field_json_obj.value();
            }
            if(key == "ignore_index" && field_json_obj.value() == "true") {
                knn_expr->ignore_index_ = true;
            }

            auto parameter = MakeUnique<InitParameter>();
            parameter->param_name_ = key;
            parameter->param_value_ = field_json_obj.value();
            knn_expr->opt_params_->emplace_back(parameter.release());
        }
    }

    return knn_expr.release();
}

MatchExpr *HTTPSearch::ParseMatchText(const nlohmann::json &json_object, HTTPStatus &http_status, nlohmann::json &response) {
    if (!json_object.is_object()) {
        response["error_code"] = ErrorCode::kInvalidExpression;
        response["error_message"] = "MatchText field should be object";
        return nullptr;
    }
    auto match_expr = MakeUnique<MatchExpr>();
    i64 topn = -1;
    String extra_params{};
    // must have: "fields", "matching_text", "topn"
    // may have: "params"
    constexpr std::array possible_keys{"fields", "matching_text", "topn", "params"};
    std::set<String> possible_keys_set(possible_keys.begin(), possible_keys.end());
    for (auto &field_json_obj : json_object.items()) {
        String key = field_json_obj.key();
        ToLower(key);
        if (!possible_keys_set.erase(key)) {
            response["error_code"] = ErrorCode::kInvalidExpression;
            response["error_message"] = fmt::format("Unknown MatchText expression key: {}", key);
            return nullptr;
        }
        if (IsEqual(key, "fields")) {
            match_expr->fields_ = field_json_obj.value();
        } else if (IsEqual(key, "matching_text")) {
            match_expr->matching_text_ = field_json_obj.value();
        } else if (IsEqual(key, "topn")) {
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
    return match_expr.release();
}

MatchTensorExpr *HTTPSearch::ParseMatchTensor(const nlohmann::json &json_object, HTTPStatus &http_status, nlohmann::json &response) {
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
    // must have: "fields", "query_tensor", "element_type", "topn"
    // may have: "params"
    constexpr std::array possible_keys{"fields", "query_tensor", "element_type", "topn", "params"};
    std::set<String> possible_keys_set(possible_keys.begin(), possible_keys.end());
    for (auto &field_json_obj : json_object.items()) {
        String key = field_json_obj.key();
        ToLower(key);
        if (!possible_keys_set.erase(key)) {
            response["error_code"] = ErrorCode::kInvalidExpression;
            response["error_message"] = fmt::format("Unknown MatchTensor expression key: {}", key);
            return nullptr;
        }
        if (IsEqual(key, "fields")) {
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
            tensor_expr = BuildConstantExprFromJson(field_json_obj.value());
        } else if (IsEqual(key, "element_type")) {
            element_type = field_json_obj.value();
        } else if (IsEqual(key, "topn")) {
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
    match_tensor_expr->SetQueryTensorStr(std::move(element_type), tensor_expr.get());
    return match_tensor_expr.release();
}

MatchSparseExpr *HTTPSearch::ParseMatchSparse(const nlohmann::json &json_object, HTTPStatus &http_status, nlohmann::json &response) {
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
    // must have: "fields", "query_vector", "metric_type", "topn"
    // may have: "params"
    constexpr std::array possible_keys{"fields", "query_vector", "metric_type", "topn", "params"};
    std::set<String> possible_keys_set(possible_keys.begin(), possible_keys.end());
    for (auto &field_json_obj : json_object.items()) {
        String key = field_json_obj.key();
        ToLower(key);
        if (!possible_keys_set.erase(key)) {
            response["error_code"] = ErrorCode::kInvalidExpression;
            response["error_message"] = fmt::format("Unknown MatchSparse expression key: {}", key);
            return nullptr;
        }
        if (IsEqual(key, "fields")) {
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
            match_sparse_expr->SetMetricType(field_json_obj.value().get<String>());
        } else if (IsEqual(key, "topn")) {
            topn = field_json_obj.value().get<i64>();
        } else if (IsEqual(key, "params")) {
            const auto &params = field_json_obj.value();
            if (!params.is_object()) {
                response["error_code"] = ErrorCode::kInvalidExpression;
                response["error_message"] = "MatchSparse params should be object";
                return nullptr;
            }
            for (auto &param : params.items()) {
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
    return match_sparse_expr.release();
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
    for (const auto &[sparse_k, sparse_v] : json_object.items()) {
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
            float16_t *embedding_data_ptr = new float16_t[dimension];
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

            float16_t *res = embedding_data_ptr;
            embedding_data_ptr = nullptr;
            return {dimension, res};
        }
        case EmbeddingDataType::kElemBFloat16: {
            bfloat16_t *embedding_data_ptr = new bfloat16_t[dimension];
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

            bfloat16_t *res = embedding_data_ptr;
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
