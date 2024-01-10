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

export module physical_top;

import stl;
import parser;
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import base_expression;
import expression_state;
import data_table;
import data_block;
import column_vector;
import load_meta;
import infinity_exception;
import physical_limit;

namespace infinity {

export enum class OrderBySingleResult : i8 {
    kEqual = 0,
    kPreferLeft = 1,
    kPreferRight = -1,
};

export class PhysicalTop : public PhysicalOperator {
public:
    explicit PhysicalTop(u64 id,
                         UniquePtr<PhysicalOperator> left,
                         u32 limit,
                         u32 offset,
                         Vector<SharedPtr<BaseExpression>> sort_expressions,
                         Vector<OrderType> order_by_types,
                         SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kTop, std::move(left), nullptr, id, load_metas), limit_(limit), offset_(offset),
          order_by_types_(std::move(order_by_types)), sort_expressions_(std::move(sort_expressions)) {}

    ~PhysicalTop() override = default;

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return left_->GetOutputNames(); }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return left_->GetOutputTypes(); }

    SizeT TaskletCount() override { return left_->TaskletCount(); }

    // for OperatorState
    inline auto const &GetSortExpressions() const { return sort_expressions_; }

    // for MergeTop
    inline auto const &GetInnerSortFunctions() const { return sort_functions_; }

    // for Top and MergeTop
    static void HandleOutputOffset(u32 total_row_cnt, u32 offset, Vector<UniquePtr<DataBlock>> &output_data_block_array);

    // for Top and MergeTop
    static Vector<Vector<SharedPtr<ColumnVector>>> GetEvalColumns(const Vector<SharedPtr<BaseExpression>> &expressions,
                                                                  Vector<SharedPtr<ExpressionState>> &expr_states,
                                                                  const Vector<UniquePtr<DataBlock>> &data_block_array);

private:
    u32 limit_{};                                                                       // limit value
    u32 offset_{};                                                                      // offset value
    u32 sort_expr_count_{};                                                             // number of expressions to sort
    Vector<OrderType> order_by_types_;                                                  // ASC or DESC
    Vector<SharedPtr<BaseExpression>> sort_expressions_;                                // expressions to sort
    Vector<StdFunction<OrderBySingleResult(void *, u32, void *, u32)>> sort_functions_; // sort functions
    // TODO: save a common threshold value for all tasks
};

} // namespace infinity
