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

export module infinity_core:http_search;

import :status;

import std;
import third_party;

import parsed_expr;
import knn_expr;
import match_expr;
import fusion_expr;
import match_tensor_expr;
import match_sparse_expr;
import function_expr;
import internal_types;
import constant_expr;
import search_expr;
import select_statement;

namespace infinity {
class Infinity;

export class HTTPSearch {
public:
    static void Process(Infinity *infinity_ptr,
                        const std::string &db_name,
                        const std::string &table_name,
                        const std::string &input_json,
                        HTTPStatus &http_status,
                        nlohmann::json &response);
    static void Explain(Infinity *infinity_ptr,
                        const std::string &db_name,
                        const std::string &table_name,
                        const std::string &input_json,
                        HTTPStatus &http_status,
                        nlohmann::json &response);

    static std::vector<ParsedExpr *> *ParseOutput(std::string_view json_sv, HTTPStatus &http_status, nlohmann::json &response);
    static std::vector<OrderByExpr *> *ParseSort(std::string_view json_sv, HTTPStatus &http_status, nlohmann::json &response);
    static std::unique_ptr<ParsedExpr> ParseFilter(std::string_view json_sv, HTTPStatus &http_status, nlohmann::json &response);
    static SearchExpr *ParseSearchExpr(std::string_view json_sv, HTTPStatus &http_status, nlohmann::json &response);
    static std::unique_ptr<FusionExpr> ParseFusion(std::string_view json_sv, HTTPStatus &http_status, nlohmann::json &response);
    static std::unique_ptr<KnnExpr> ParseMatchDense(std::string_view json_sv, HTTPStatus &http_status, nlohmann::json &response);
    static std::unique_ptr<MatchExpr> ParseMatchText(std::string_view json_sv, HTTPStatus &http_status, nlohmann::json &response);
    static std::unique_ptr<MatchTensorExpr> ParseMatchTensor(std::string_view json_sv, HTTPStatus &http_status, nlohmann::json &response);
    static std::unique_ptr<MatchSparseExpr> ParseMatchSparse(std::string_view json_sv, HTTPStatus &http_status, nlohmann::json &response);
    static std::tuple<i64, void *>
    ParseVector(std::string_view json_sv, EmbeddingDataType elem_type, HTTPStatus &http_status, nlohmann::json &response);
    static std::unique_ptr<ConstantExpr> ParseSparseVector(std::string_view json_sv, HTTPStatus &http_status, nlohmann::json &response);
    static std::unique_ptr<FunctionExpr>
    ParseFDEFunction(std::string_view json_sv, EmbeddingDataType elem_type, HTTPStatus &http_status, nlohmann::json &response);
};

} // namespace infinity
