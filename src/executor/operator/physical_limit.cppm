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

import stl;
import parser;
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import base_expression;
import data_table;

export module physical_limit;

namespace infinity {

export class PhysicalLimit : public PhysicalOperator {
public:
    explicit PhysicalLimit(u64 id, SharedPtr<PhysicalOperator> left, SharedPtr<BaseExpression> limit_expr, SharedPtr<BaseExpression> offset_expr)
        : PhysicalOperator(PhysicalOperatorType::kLimit, Move(left), nullptr, id), limit_expr_(Move(limit_expr)),
          offset_expr_(Move(offset_expr)) {}

    ~PhysicalLimit() override = default;

    void Init() override;

    void Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return left_->GetOutputNames(); }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return left_->GetOutputTypes(); }

    inline const SharedPtr<BaseExpression> &limit_expr() const { return limit_expr_; }

    inline const SharedPtr<BaseExpression> &offset_expr() const { return offset_expr_; }

    static SharedPtr<DataTable> GetLimitOutput(const SharedPtr<DataTable> &input_table, i64 limit, i64 offset);

private:
    SharedPtr<BaseExpression> limit_expr_{};
    SharedPtr<BaseExpression> offset_expr_{};

    SharedPtr<DataTable> input_table_{};
    u64 input_table_index_{};
};

} // namespace infinity
