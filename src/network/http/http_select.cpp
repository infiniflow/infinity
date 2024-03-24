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

#include <string>

module http_select;

import stl;
import third_party;
import parsed_expr;
import knn_expr;
import match_expr;
import column_expr;
import defer_op;
import expr_parser;
import expression_parser_result;
import statement_common;

namespace infinity {

void HTTPSelect::Process(const String &db_name,
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

        Vector<ParsedExpr *> *output_columns{nullptr};
        ParsedExpr *filter{nullptr};
        Vector<ParsedExpr *> *fusion_exprs{nullptr};
        KnnExpr *knn_expr{nullptr};
        MatchExpr *match_expr{nullptr};
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
            if (fusion_exprs != nullptr) {
                for (auto &expr : *fusion_exprs) {
                    delete expr;
                }
                delete fusion_exprs;
                fusion_exprs = nullptr;
            }
            if (knn_expr != nullptr) {
                delete knn_expr;
                knn_expr = nullptr;
            }
            if (match_expr != nullptr) {
                delete match_expr;
                match_expr = nullptr;
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
            } else if (IsEqual(key, "fusion")) {
                if (fusion_exprs != nullptr or knn_expr != nullptr or match_expr != nullptr) {
                    response["error_code"] = ErrorCode::kInvalidExpression;
                    response["error_message"] =
                        "There are more than one fusion expressions, Or fusion expression coexists with knn / match expression ";
                    return;
                }

            } else if (IsEqual(key, "knn")) {
                if (fusion_exprs != nullptr or knn_expr != nullptr or match_expr != nullptr) {
                    response["error_code"] = ErrorCode::kInvalidExpression;
                    response["error_message"] =
                        "There are more than one fusion expressions, Or fusion expression coexists with knn / match expression ";
                    return;
                }
            } else if (IsEqual(key, "match")) {
                if (fusion_exprs != nullptr or knn_expr != nullptr or match_expr != nullptr) {
                    response["error_code"] = ErrorCode::kInvalidExpression;
                    response["error_message"] =
                        "There are more than one fusion expressions, Or fusion expression coexists with knn / match expression ";
                    return;
                }

            } else {
                response["error_code"] = ErrorCode::kInvalidExpression;
                response["error_message"] = "Unknown expression: " + key;
                return;
            }
        }

        //        const auto &update_clause = input_json["update"];
    } catch (nlohmann::json::exception &e) {
        response["error_code"] = ErrorCode::kInvalidJsonFormat;
        response["error_message"] = e.what();
    }
    return;
}

ParsedExpr *HTTPSelect::ParseFilter(const nlohmann::json &json_object, HTTPStatus &http_status, nlohmann::json &response) {

    UniquePtr<ExpressionParserResult> expr_parsed_result = MakeUnique<ExpressionParserResult>();
    ExprParser expr_parser;
    expr_parser.Parse(json_object, expr_parsed_result.get());
    if (expr_parsed_result->IsError() || expr_parsed_result->exprs_ptr_->size() != 1) {
        response["error_code"] = ErrorCode::kInvalidExpression;
        response["error_message"] = fmt::format("Invalid expression: {}", json_object);
        return nullptr;
    }

    return expr_parsed_result->exprs_ptr_->at(0);
}

Vector<ParsedExpr *> *HTTPSelect::ParseOutput(const nlohmann::json &output_list, HTTPStatus &http_status, nlohmann::json &response) {

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

    for (const auto &output_expr_str : output_list) {
        if (!output_expr_str.is_string()) {
            response["error_code"] = ErrorCode::kInvalidExpression;
            response["error_message"] = fmt::format("Invalid expression: {}", output_expr_str);
            return nullptr;
        }

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

    // Avoiding DeferFN auto free the output expressions
    Vector<ParsedExpr *> *res = output_columns;
    output_columns = nullptr;
    return res;
}
Vector<ParsedExpr *> *HTTPSelect::ParseFusion(const nlohmann::json &funsion_json_object, HTTPStatus &http_status, nlohmann::json &response) {
    if (!funsion_json_object.is_object()) {
        response["error_code"] = ErrorCode::kInvalidExpression;
        response["error_message"] = fmt::format("Fusion expression must be a json object: {}", funsion_json_object);
        return nullptr;
    }

    return nullptr;
}
KnnExpr *HTTPSelect::ParseKnn(const nlohmann::json &knn_json_object, HTTPStatus &http_status, nlohmann::json &response) {

    KnnExpr *knn_expr = new KnnExpr();
    DeferFn defer_fn([&]() {
        if (knn_expr != nullptr) {
            delete knn_expr;
            knn_expr = nullptr;
        }
    });

    for (auto &field_json_obj : knn_json_object.items()) {
        String key = field_json_obj.key();
        ToLower(key);
        if (IsEqual(key, "fields")) {
            ColumnExpr *column_expr = new ColumnExpr();
            column_expr->names_.emplace_back(field_json_obj.value());
            knn_expr->column_expr_ = column_expr;
            column_expr = nullptr;
        } else if (IsEqual(key, "query_vector")) {
            auto [embedding_data_type, dimension, embedding_ptr] = ParseVector(field_json_obj.value(), http_status, response);
            knn_expr->embedding_data_type_ = embedding_data_type;
            knn_expr->dimension_ = dimension;
            knn_expr->embedding_data_ptr_ = embedding_ptr;
        } else if (IsEqual(key, "top_k")) {
            knn_expr->topn_ = field_json_obj.value();
        } else if (IsEqual(key, "metric_type")) {
            String metric_type = field_json_obj.value();
            ToLower(metric_type);
            if (IsEqual(metric_type, "l2")) {
                knn_expr->distance_type_ = KnnDistanceType::kL2;
            } else if (IsEqual(metric_type, "inner_product")) {
                knn_expr->distance_type_ = KnnDistanceType::kInnerProduct;
            } else if (IsEqual(metric_type, "cosine")) {
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
            InitParameter* parameter = new InitParameter();
            parameter->param_name_ = key;
            parameter->param_value_ = field_json_obj.value();
            knn_expr->opt_params_->emplace_back(parameter);
        }
    }

    KnnExpr *res = knn_expr;
    knn_expr = nullptr;
    return res;
}
MatchExpr *HTTPSelect::ParseMatch(const nlohmann::json &match_json_object, HTTPStatus &http_status, nlohmann::json &response) {
    MatchExpr *match_expr = new MatchExpr();
    DeferFn defer_fn([&]() {
        if (match_expr != nullptr) {
            delete match_expr;
            match_expr = nullptr;
        }
    });

    for (auto &field_json_obj : match_json_object.items()) {
        String key = field_json_obj.key();
        ToLower(key);
        if (IsEqual(key, "operator")) {
            match_expr->options_text_ = field_json_obj.value();
        } else if (IsEqual(key, "fields")) {
            match_expr->fields_ = field_json_obj.value();
        } else if (IsEqual(key, "query")) {
            match_expr->matching_text_ = field_json_obj.value();
        } else {
            response["error_code"] = ErrorCode::kInvalidExpression;
            response["error_message"] = fmt::format("Unknown match expression key: {}", key);
            return nullptr;
        }
    }

    if (match_expr->options_text_.empty() || match_expr->matching_text_.empty()) {
        response["error_code"] = ErrorCode::kInvalidExpression;
        response["error_message"] = fmt::format("Invalid match expression");
        return nullptr;
    }

    MatchExpr *res = match_expr;
    match_expr = nullptr;
    return res;
}

Tuple<EmbeddingDataType, i64, void *> HTTPSelect::ParseVector(const nlohmann::json &json_object, HTTPStatus &http_status, nlohmann::json &response) {
    if (!json_object.is_array()) {
        response["error_code"] = ErrorCode::kInvalidExpression;
        response["error_message"] = fmt::format("Can't recognize embedding/vector.");
        return {EmbeddingDataType::kElemInvalid, 0, nullptr};
    }
    SizeT dimension = json_object.size();
    if (dimension == 0) {
        response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
        response["error_message"] = fmt::format("Empty embedding data");
        return {EmbeddingDataType::kElemInvalid, 0, nullptr};
    }

    auto first_elem = json_object[0];
    auto first_elem_type = first_elem.type();
    if (first_elem_type == nlohmann::json::value_t::number_integer or first_elem_type == nlohmann::json::value_t::number_unsigned) {
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
                case nlohmann::json::value_t::number_integer: {
                    embedding_data_ptr[idx] = value_ref.template get<i64>();
                    break;
                }
                case nlohmann::json::value_t::number_unsigned: {
                    embedding_data_ptr[idx] = value_ref.template get<u64>();
                    break;
                }
                default: {
                    response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                    response["error_message"] = fmt::format("Embedding element type should be integer");
                    return {EmbeddingDataType::kElemInvalid, 0, nullptr};
                }
            }
        }

        i32 *res = embedding_data_ptr;
        embedding_data_ptr = nullptr;
        return {EmbeddingDataType::kElemInt32, dimension, res};

    } else if (first_elem_type == nlohmann::json::value_t::number_float) {

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
                default: {
                    response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
                    response["error_message"] = fmt::format("Embedding element type should be float");
                    return {EmbeddingDataType::kElemInvalid, 0, nullptr};
                }
            }
        }

        f32 *res = embedding_data_ptr;
        embedding_data_ptr = nullptr;
        return {EmbeddingDataType::kElemInt32, dimension, res};

    } else {
        response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
        response["error_message"] = fmt::format("Embedding element type can only be integer or float");
        return {EmbeddingDataType::kElemInvalid, 0, nullptr};
    }
}

} // namespace infinity
