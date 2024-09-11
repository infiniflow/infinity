// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
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

export module search_expression;

import stl;
import base_expression;
import fusion_expression;
import match_expression;
import knn_expression;
import match_tensor_expression;
import match_sparse_expression;
import logical_type;
import internal_types;
import data_type;

namespace infinity {

export class SearchExpression final : public BaseExpression {
public:
    SearchExpression(Vector<SharedPtr<BaseExpression>> &match_exprs, Vector<SharedPtr<FusionExpression>> &fusion_exprs);

    inline DataType Type() const override { return DataType(LogicalType::kFloat); }

    String ToString() const override;

public:
    // Eash match_expr shall be one of MatchExpression, KnnExpression, MatchTensorExpression, MatchSparseExpression
    Vector<SharedPtr<BaseExpression>> match_exprs_{};
    Vector<SharedPtr<FusionExpression>> fusion_exprs_{};
    bool have_filter_in_subsearch_{false};
};

} // namespace infinity