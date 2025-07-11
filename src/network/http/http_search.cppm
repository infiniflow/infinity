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
import search_expr;
import select_statement;

namespace infinity {

export class HTTPSearch {
public:
    static void Process(Infinity *infinity_ptr,
                        const String &db_name,
                        const String &table_name,
                        const String &input_json,
                        HTTPStatus &http_status,
                        rapidjson::Writer<rapidjson::StringBuffer> &writer);
    static void Explain(Infinity *infinity_ptr,
                        const String &db_name,
                        const String &table_name,
                        const String &input_json,
                        HTTPStatus &http_status,
                        rapidjson::Writer<rapidjson::StringBuffer> &writer);

    static Vector<ParsedExpr *> *ParseOutput(std::string_view json_sv, HTTPStatus &http_status, rapidjson::Writer<rapidjson::StringBuffer> &writer);
    static Vector<OrderByExpr *> *ParseSort(std::string_view json_sv, HTTPStatus &http_status, rapidjson::Writer<rapidjson::StringBuffer> &writer);
    static UniquePtr<ParsedExpr> ParseFilter(std::string_view json_sv, HTTPStatus &http_status, rapidjson::Writer<rapidjson::StringBuffer> &writer);
    static SearchExpr *ParseSearchExpr(std::string_view json_sv, HTTPStatus &http_status, rapidjson::Writer<rapidjson::StringBuffer> &writer);
    static UniquePtr<FusionExpr> ParseFusion(std::string_view json_sv, HTTPStatus &http_status, rapidjson::Writer<rapidjson::StringBuffer> &writer);
    static UniquePtr<KnnExpr> ParseMatchDense(std::string_view json_sv, HTTPStatus &http_status, rapidjson::Writer<rapidjson::StringBuffer> &writer);
    static UniquePtr<MatchExpr> ParseMatchText(std::string_view json_sv, HTTPStatus &http_status, rapidjson::Writer<rapidjson::StringBuffer> &writer);
    static UniquePtr<MatchTensorExpr> ParseMatchTensor(std::string_view json_sv, HTTPStatus &http_status, rapidjson::Writer<rapidjson::StringBuffer> &writer);
    static UniquePtr<MatchSparseExpr> ParseMatchSparse(std::string_view json_sv, HTTPStatus &http_status, rapidjson::Writer<rapidjson::StringBuffer> &writer);
    static Tuple<i64, void *> ParseVector(std::string_view json_sv, EmbeddingDataType elem_type, HTTPStatus &http_status, rapidjson::Writer<rapidjson::StringBuffer> &writer);
    static UniquePtr<ConstantExpr> ParseSparseVector(std::string_view json_sv, HTTPStatus &http_status, rapidjson::Writer<rapidjson::StringBuffer> &writer);
};

} // namespace infinity
