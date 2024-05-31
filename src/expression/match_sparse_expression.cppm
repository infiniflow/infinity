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

export module match_sparse_expression;

import stl;
import base_expression;
import data_type;
import expression_type;
import internal_types;
import statement_common;
import match_sparse_expr;
import sparse_info;

namespace infinity {

export class MatchSparseExpression final : public BaseExpression {
public:
    MatchSparseExpression(Vector<SharedPtr<BaseExpression>> search_column,
                          SparseRefT query_sparse,
                          SharedPtr<SparseInfo> sparse_info,
                          SparseMetricType metric_type,
                          SizeT topn,
                          const Vector<UniquePtr<InitParameter>> &opt_params)
        : BaseExpression(ExpressionType::kMatchSparse, std::move(search_column)), query_sparse_(query_sparse), sparse_info_(sparse_info),
          metric_type_(metric_type), topn_(topn), opt_params_(opt_params) {}

    DataType Type() const override;

    String ToString() const override;

public:
    SparseRefT query_sparse_;
    SharedPtr<SparseInfo> sparse_info_;

    SparseMetricType metric_type_;
    SizeT topn_;

    const Vector<UniquePtr<InitParameter>> &opt_params_;
};

} // namespace infinity
