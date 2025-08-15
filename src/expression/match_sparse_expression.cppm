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

export module infinity_core:match_sparse_expression;

import :stl;
import :base_expression;
import :expression_type;
import :column_expression;
import :base_expression;

import constant_expr;
import sparse_info;
import internal_types;
import statement_common;
import match_sparse_expr;
import data_type;

namespace infinity {

export class MatchSparseExpression final : public BaseExpression {
public:
    MatchSparseExpression(std::vector<std::shared_ptr<BaseExpression>> search_column,
                          std::shared_ptr<BaseExpression> query_sparse_expr,
                          SparseMetricType metric_type,
                          size_t query_n,
                          size_t topn,
                          std::vector<std::unique_ptr<InitParameter>> opt_params,
                          std::shared_ptr<BaseExpression> optional_filter,
                          std::string index_name,
                          bool ignore_index);

    DataType Type() const override;

    std::string ToString() const override;

    u64 Hash() const override;

    bool Eq(const BaseExpression &other) const override;

private:
    void MakeQuery(std::shared_ptr<BaseExpression> query_sparse_expr);

public:
    const ColumnExpression *column_expr_;
    std::shared_ptr<BaseExpression> query_sparse_expr_;

    SparseMetricType metric_type_;
    size_t query_n_;
    size_t topn_;

    std::vector<std::unique_ptr<InitParameter>> opt_params_;

    std::shared_ptr<BaseExpression> optional_filter_;

    const std::string index_name_;
    bool ignore_index_;
};

} // namespace infinity
