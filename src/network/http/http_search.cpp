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

module http_search;

import stl;
import third_party;
import parsed_expr;
import knn_expr;
import match_expr;
import search_expr;
import fusion_expr;
import column_expr;
import defer_op;
import expr_parser;
import expression_parser_result;
import statement_common;
import query_result;
import data_block;
import value;

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

        Vector<ParsedExpr *> *output_columns{nullptr};
        Vector<ParsedExpr *> *search_exprs{nullptr};
        ParsedExpr *filter{nullptr};
        FusionExpr *fusion_expr{nullptr};
        KnnExpr *knn_expr{nullptr};
        MatchExpr *match_expr{nullptr};
        // SearchExpr *search_expr = new SearchExpr();
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
            if (fusion_expr != nullptr) {
                delete fusion_expr;
                fusion_expr = nullptr;
            }
            if (knn_expr != nullptr) {
                delete knn_expr;
                knn_expr = nullptr;
            }
            if (match_expr != nullptr) {
                delete match_expr;
                match_expr = nullptr;
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
                if (fusion_expr != nullptr or knn_expr != nullptr or match_expr != nullptr) {
                    response["error_code"] = ErrorCode::kInvalidExpression;
                    response["error_message"] =
                        "There are more than one fusion expressions, Or fusion expression coexists with knn / match expression ";
                    return;
                }
                search_expr = new SearchExpr();
                auto &fusion_children = elem.value();
                for (const auto &expression : fusion_children.items()) {
                    String key = expression.key();
                    ToLower(key);

                    if (IsEqual(key, "knn")) {
                        auto &knn_json = expression.value();
                        if (!knn_json.is_object()) {
                            response["error_code"] = ErrorCode::kInvalidExpression;
                            response["error_message"] = "KNN field should be object";
                            return;
                        }
                        knn_expr = ParseKnn(knn_json, http_status, response);
                        if (knn_expr == nullptr) {
                            return;
                        }
                        search_exprs->push_back(knn_expr);
                        knn_expr = nullptr;
                    } else if (IsEqual(key, "match")) {
                        auto &match_json = expression.value();
                        match_expr = ParseMatch(match_json, http_status, response);
                        if (match_expr == nullptr) {
                            return;
                        }
                        search_exprs->push_back(match_expr);
                        match_expr = nullptr;
                    } else if (IsEqual(key, "method")) {
                        if (fusion_expr != nullptr && !fusion_expr->method_.empty()) {
                            response["error_code"] = ErrorCode::kInvalidExpression;
                            response["error_message"] = "Method is already given";
                            return;
                        }
                        fusion_expr = new FusionExpr();
                        fusion_expr->method_ = expression.value();
                        search_exprs->push_back(fusion_expr);
                        fusion_expr = nullptr;
                    } else {
                        response["error_code"] = ErrorCode::kInvalidExpression;
                        response["error_message"] = "Error fusion clause";
                        return;
                    }
                }
            } else if (IsEqual(key, "knn")) {
                search_expr = new SearchExpr();
                if (fusion_expr != nullptr or knn_expr != nullptr or match_expr != nullptr) {
                    response["error_code"] = ErrorCode::kInvalidExpression;
                    response["error_message"] =
                        "There are more than one fusion expressions, Or fusion expression coexists with knn / match expression ";
                    return;
                }
                auto &knn_json = elem.value();
                if (!knn_json.is_object()) {
                    response["error_code"] = ErrorCode::kInvalidExpression;
                    response["error_message"] = "KNN field should be object";
                    return;
                }
                knn_expr = ParseKnn(knn_json, http_status, response);
                if (knn_expr == nullptr) {
                    return;
                }
                search_exprs->push_back(knn_expr);
                knn_expr = nullptr;
            } else if (IsEqual(key, "match")) {
                if (fusion_expr != nullptr or knn_expr != nullptr or match_expr != nullptr) {
                    response["error_code"] = ErrorCode::kInvalidExpression;
                    response["error_message"] =
                        "There are more than one fusion expressions, Or fusion expression coexists with knn / match expression ";
                    return;
                }
                auto &match_json = elem.value();
                match_expr = ParseMatch(match_json, http_status, response);
                if (match_expr == nullptr) {
                    return;
                }
                search_exprs->push_back(match_expr);
                match_expr = nullptr;
            } else {
                response["error_code"] = ErrorCode::kInvalidExpression;
                response["error_message"] = "Unknown expression: " + key;
                return;
            }
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
Vector<ParsedExpr *> *HTTPSearch::ParseFusion(const nlohmann::json &funsion_json_object, HTTPStatus &http_status, nlohmann::json &response) {
    if (!funsion_json_object.is_object()) {
        response["error_code"] = ErrorCode::kInvalidExpression;
        response["error_message"] = fmt::format("Fusion expression must be a json object: {}", funsion_json_object);
        return nullptr;
    }

    return nullptr;
}
KnnExpr *HTTPSearch::ParseKnn(nlohmann::json &knn_json_object, HTTPStatus &http_status, nlohmann::json &response) {

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
            const auto &fields_value = field_json_obj.value();
            SizeT fields_count = fields_value.size();
            for (SizeT i = 0; i < fields_count; i++) {
                column_expr->names_.emplace_back(fields_value[i]);
            }
            knn_expr->column_expr_ = column_expr;
            column_expr = nullptr;
        } else if (IsEqual(key, "query_vector")) {
            String element_type = knn_json_object["element_type"];
            if (IsEqual(element_type, "float")) {
                knn_expr->embedding_data_type_ = EmbeddingDataType::kElemFloat;
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
            InitParameter *parameter = new InitParameter();
            parameter->param_name_ = key;
            parameter->param_value_ = field_json_obj.value();
            knn_expr->opt_params_->emplace_back(parameter);
        }
    }

    KnnExpr *res = knn_expr;
    knn_expr = nullptr;
    return res;
}
MatchExpr *HTTPSearch::ParseMatch(const nlohmann::json &match_json_object, HTTPStatus &http_status, nlohmann::json &response) {
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

Tuple<i64, void *>
HTTPSearch::ParseVector(nlohmann::json &json_object, EmbeddingDataType elem_type, HTTPStatus &http_status, nlohmann::json &response) {
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
        default: {
            response["error_code"] = ErrorCode::kInvalidEmbeddingDataType;
            response["error_message"] = fmt::format("Only support float as the embedding data type");
            return {0, nullptr};
        }
    }
}

} // namespace infinity
