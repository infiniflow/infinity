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

export module http_search;

import stl;
import status;
import third_party;
import parsed_expr;
import knn_expr;
import match_expr;
import fusion_expr;
import match_tensor_expr;
import match_sparse_expr;
import infinity;
import internal_types;
import constant_expr;

namespace infinity {

export class HTTPSearch {
public:
    static void Process(Infinity *infinity_ptr,
                        const String &db_name,
                        const String &table_name,
                        const String &input_json,
                        HTTPStatus &http_status,
                        nlohmann::json &response);
    static void Explain(Infinity *infinity_ptr,
                        const String &db_name,
                        const String &table_name,
                        const String &input_json,
                        HTTPStatus &http_status,
                        nlohmann::json &response);

    static ParsedExpr *ParseFilter(const nlohmann::json &json_object, HTTPStatus &http_status, nlohmann::json &response);
    static Vector<ParsedExpr *> *ParseOutput(const nlohmann::json &json_object, HTTPStatus &http_status, nlohmann::json &response);
    static FusionExpr *ParseFusion(const nlohmann::json &json_object, HTTPStatus &http_status, nlohmann::json &response);
    static KnnExpr *ParseMatchDense(const nlohmann::json &json_object, HTTPStatus &http_status, nlohmann::json &response);
    static MatchExpr *ParseMatchText(const nlohmann::json &json_object, HTTPStatus &http_status, nlohmann::json &response);
    static MatchTensorExpr *ParseMatchTensor(const nlohmann::json &json_object, HTTPStatus &http_status, nlohmann::json &response);
    static MatchSparseExpr *ParseMatchSparse(const nlohmann::json &json_object, HTTPStatus &http_status, nlohmann::json &response);
    static Tuple<i64, void *>
    ParseVector(const nlohmann::json &json_object, EmbeddingDataType elem_type, HTTPStatus &http_status, nlohmann::json &response);
    static UniquePtr<ConstantExpr> ParseSparseVector(const nlohmann::json &json_object, HTTPStatus &http_status, nlohmann::json &response);
};

} // namespace infinity
