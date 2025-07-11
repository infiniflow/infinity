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

#include <compare>

export module infinity_core:physical_top;

import :stl;

import :query_context;
import :operator_state;
import :physical_operator;
import :physical_operator_type;
import :base_expression;
import :expression_state;
import :data_table;
import :data_block;
import :column_vector;
import :load_meta;
import :infinity_exception;
import :physical_limit;
import internal_types;
import select_statement;
import data_type;

namespace infinity {

export class CompareTwoRowAndPreferLeft {
public:
    CompareTwoRowAndPreferLeft() = default;
    explicit CompareTwoRowAndPreferLeft(
        Vector<std::function<std::strong_ordering(const SharedPtr<ColumnVector> &, u32, const SharedPtr<ColumnVector> &, u32)>> &&sort_functions)
        : sort_functions_(std::move(sort_functions)) {
        sort_expr_count_ = sort_functions_.size();
    }
    ~CompareTwoRowAndPreferLeft() = default;
    bool Compare(const Vector<SharedPtr<ColumnVector>> &left, u32 left_id, const Vector<SharedPtr<ColumnVector>> &right, u32 right_id) const {
        for (u32 i = 0; i < sort_expr_count_; ++i) {
            auto compare_result = sort_functions_[i](left[i], left_id, right[i], right_id);
            if (compare_result != std::strong_ordering::equal) {
                return compare_result == std::strong_ordering::less;
            }
        }
        // Prefer left if all expressions are equal
        return true;
    }

private:
    u32 sort_expr_count_{};
    Vector<std::function<std::strong_ordering(const SharedPtr<ColumnVector> &, u32, const SharedPtr<ColumnVector> &, u32)>>
        sort_functions_; // sort functions
};

export class PhysicalTop : public PhysicalOperator {
public:
    explicit PhysicalTop(u64 id,
                         UniquePtr<PhysicalOperator> left,
                         u32 limit,
                         u32 offset,
                         Vector<SharedPtr<BaseExpression>> sort_expressions,
                         Vector<OrderType> order_by_types,
                         SharedPtr<Vector<LoadMeta>> load_metas,
                         bool total_hits_count_flag)
        : PhysicalOperator(PhysicalOperatorType::kTop, std::move(left), nullptr, id, load_metas), limit_(limit), offset_(offset),
          order_by_types_(std::move(order_by_types)), sort_expressions_(std::move(sort_expressions)), total_hits_count_flag_(total_hits_count_flag) {}

    ~PhysicalTop() override = default;

    void Init(QueryContext* query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return PhysicalCommonFunctionUsingLoadMeta::GetOutputNames(*this); }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return PhysicalCommonFunctionUsingLoadMeta::GetOutputTypes(*this); }

    SizeT TaskletCount() override { return left_->TaskletCount(); }

    // for OperatorState and Explain
    inline auto const &GetSortExpressions() const { return sort_expressions_; }

    // for Explain
    inline auto const &GetOrderbyTypes() const { return order_by_types_; }

    // for Explain
    inline auto GetLimit() const { return limit_; }

    // for Explain
    inline auto GetOffset() const { return offset_; }

    // for MergeTop
    inline auto const &GetInnerCompareFunction() const { return prefer_left_function_; }

    // for Top and MergeTop
    static void HandleOutputOffset(u32 total_row_cnt, u32 offset, Vector<UniquePtr<DataBlock>> &output_data_block_array);

    // for Top and MergeTop
    static Vector<Vector<SharedPtr<ColumnVector>>> GetEvalColumns(const Vector<SharedPtr<BaseExpression>> &expressions,
                                                                  Vector<SharedPtr<ExpressionState>> &expr_states,
                                                                  const Vector<UniquePtr<DataBlock>> &data_block_array);

    // for Top and Sort
    static std::function<std::strong_ordering(const SharedPtr<ColumnVector> &, u32, const SharedPtr<ColumnVector> &, u32)>
    GenerateSortFunction(OrderType compare_order, SharedPtr<BaseExpression> &sort_expression);

private:
    u32 limit_{};                                        // limit value
    u32 offset_{};                                       // offset value
    u32 sort_expr_count_{};                              // number of expressions to sort
    Vector<OrderType> order_by_types_;                   // ASC or DESC
    Vector<SharedPtr<BaseExpression>> sort_expressions_; // expressions to sort
    CompareTwoRowAndPreferLeft prefer_left_function_;    // compare function
    bool total_hits_count_flag_{};
    // TODO: save a common threshold value for all tasks
};

} // namespace infinity
