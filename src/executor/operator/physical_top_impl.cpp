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

module infinity_core:physical_top.impl;

import :physical_top;
import :logger;
import :query_context;
import :operator_state;
import :base_expression;
import :expression_state;
import :data_block;
import :value_expression;
import :column_vector;
import :infinity_exception;
import :expression_evaluator;
import :default_values;
import :expression_type;
import :status;

import std;
import third_party;

import logical_type;
import internal_types;
import select_statement;
import data_type;

namespace infinity {

std::vector<std::vector<std::shared_ptr<ColumnVector>>> TopSolver::GetEvalColumns(const std::vector<std::shared_ptr<BaseExpression>> &expressions,
                                                                                  std::vector<std::shared_ptr<ExpressionState>> &expr_states,
                                                                                  const std::vector<std::unique_ptr<DataBlock>> &data_block_array) {
    std::vector<std::vector<std::shared_ptr<ColumnVector>>> eval_columns;
    eval_columns.reserve(data_block_array.size());
    const u32 sort_expr_count = expressions.size();
    for (auto &data_block_ptr : data_block_array) {
        std::vector<std::shared_ptr<ColumnVector>> results;
        ExpressionEvaluator expr_evaluator;
        expr_evaluator.Init(data_block_ptr.get());
        results.reserve(sort_expr_count);
        for (u32 expr_id = 0; expr_id < sort_expr_count; ++expr_id) {
            auto &expr = expressions[expr_id];
            std::shared_ptr<ColumnVector> result_vector;
            if (expr->type() != ExpressionType::kReference) {
                // need to initialize the result vector
                result_vector = std::make_shared<ColumnVector>(std::make_shared<DataType>(expr->Type()));
                result_vector->Initialize();
            }
            expr_evaluator.Execute(expr, expr_states[expr_id], result_vector);
            results.emplace_back(std::move(result_vector));
        }
        eval_columns.emplace_back(std::move(results));
    }
    return eval_columns;
}

void TopSolver::Init() {
    for (size_t i = 0; i < expressions_.size(); i++) {
        auto expr_state = ExpressionState::CreateState(expressions_[i]);
        expr_states_.push_back(expr_state);
    }
}

void TopSolver::Solve(const std::vector<std::unique_ptr<DataBlock>> &input_data_block_array,
                      std::vector<std::unique_ptr<DataBlock>> &output_data_block_array) {
    std::vector<std::vector<std::shared_ptr<ColumnVector>>> eval_columns = GetEvalColumns(expressions_, expr_states_, input_data_block_array);
    // Using a lambda to compare elements with proper capture
    auto custom_less = [this, &eval_columns](const std::pair<u32, u32> &x, const std::pair<u32, u32> &y) -> bool {
        return prefer_left_function_.Compare(eval_columns[x.first], x.second, eval_columns[y.first], y.second);
    };
    std::vector<std::pair<u32, u32>> block_row_ids; //(block_idx, row_idx)
    std::priority_queue pq(block_row_ids.begin(), block_row_ids.begin(), custom_less);
    for (size_t i = 0; i < input_data_block_array.size(); i++) {
        for (size_t j = 0; j < input_data_block_array[i]->row_count(); j++) {
            std::pair<u32, u32> current_block_row_id = std::make_pair<u32, u32>(i, j);
            if (pq.size() < limit_) {
                pq.push(current_block_row_id);
            } else {
                if (custom_less(current_block_row_id, pq.top())) {
                    pq.pop();
                    pq.push(current_block_row_id);
                }
            }
        }
    }

    while (!pq.empty()) {
        std::pair<u32, u32> block_row_id = pq.top();
        pq.pop();
        block_row_ids.push_back(block_row_id);
    }
    std::reverse(block_row_ids.begin(), block_row_ids.end());

    WriteToOutput(input_data_block_array, block_row_ids, output_data_block_array);
}

void TopSolver::Finalize(std::vector<std::unique_ptr<DataBlock>> &output_data_block_array, u32 execution_count, u32 offset) {
    if (execution_count <= 1 && offset == 0)
        return;
    std::vector<std::unique_ptr<DataBlock>> input_data_block_array;
    std::swap(input_data_block_array, output_data_block_array);
    size_t total_hits_row_count = std::accumulate(input_data_block_array.begin(), input_data_block_array.end(), 0, [](u32 x, const auto &y) -> u32 {
        return x + y->row_count();
    });
    if (total_hits_row_count <= offset)
        return;
    std::vector<std::pair<u32, u32>> block_row_ids; //(block_idx, row_idx)
    if (execution_count <= 1) {
        u32 skipped = 0;
        size_t block_idx = 0;
        for (; block_idx < input_data_block_array.size(); block_idx++) {
            u32 block_row_cnt = input_data_block_array[block_idx]->row_count();
            if (skipped + block_row_cnt > offset)
                break;
            skipped += block_row_cnt;
        }
        if (block_idx < input_data_block_array.size()) {
            for (size_t i = offset - skipped; i < input_data_block_array[block_idx]->row_count(); i++)
                block_row_ids.push_back(std::make_pair<u32, u32>(block_idx, i));
            block_idx++;
            for (; block_idx < input_data_block_array.size(); block_idx++) {
                for (size_t i = 0; i < input_data_block_array[block_idx]->row_count(); i++)
                    block_row_ids.push_back(std::make_pair<u32, u32>(block_idx, i));
            }
        }
    } else {
        for (size_t block_idx = 0; block_idx < input_data_block_array.size(); block_idx++) {
            if (input_data_block_array[block_idx]->row_count() > 0) {
                block_row_ids.push_back(std::make_pair<u32, u32>(block_idx, 0));
            }
        }
        if (block_row_ids.size() == 1) {
            size_t block_idx = block_row_ids[0].first;
            u32 limit = std::min(limit_, (u32)input_data_block_array[block_idx]->row_count());
            block_row_ids.clear();
            for (size_t i = offset; i < limit; i++) {
                block_row_ids.push_back(std::make_pair<u32, u32>(block_idx, i));
            }
        } else {
            // Using a lambda to compare elements with proper capture
            // Need to get eval_columns first for Finalize comparison
            std::vector<std::vector<std::shared_ptr<ColumnVector>>> eval_columns = GetEvalColumns(expressions_, expr_states_, input_data_block_array);
            auto custom_less = [this, &eval_columns](const std::pair<u32, u32> &x, const std::pair<u32, u32> &y) -> bool {
                return !prefer_left_function_.Compare(eval_columns[x.first], x.second, eval_columns[y.first], y.second);
            };
            std::priority_queue pq(block_row_ids.begin(), block_row_ids.end(), custom_less);
            block_row_ids.clear();
            u32 skipped = 0;
            while (skipped + block_row_ids.size() < limit_ && !pq.empty()) {
                std::pair<u32, u32> block_row_id = pq.top();
                pq.pop();
                if (skipped < offset) {
                    skipped++;
                } else {
                    block_row_ids.push_back(block_row_id);
                }
                if (block_row_id.second + 1 < input_data_block_array[block_row_id.first]->row_count()) {
                    pq.push(std::make_pair(block_row_id.first, block_row_id.second + 1));
                }
            }
        }
    }
    WriteToOutput(input_data_block_array, block_row_ids, output_data_block_array);
}

void TopSolver::WriteToOutput(const std::vector<std::unique_ptr<DataBlock>> &input_data_block_array,
                              const std::vector<std::pair<u32, u32>> &selected_row_ids,
                              std::vector<std::unique_ptr<DataBlock>> &output_data_block_array) {
    auto const &db_types = input_data_block_array[0]->types();
    u32 output_block_id = output_data_block_array.size();
    u32 output_block_row_id = 0;
    {
        // prepare output blocks
        i64 row_cnt = selected_row_ids.size();
        do {
            auto data_block = DataBlock::MakeUniquePtr();
            // u32 need_capacity = Min(DEFAULT_BLOCK_CAPACITY, row_cnt);
            u32 need_capacity = DEFAULT_BLOCK_CAPACITY; // need to assume full capacity for MergeTop
            data_block->Init(db_types, need_capacity);
            output_data_block_array.emplace_back(std::move(data_block));
            row_cnt -= need_capacity;
        } while (row_cnt > 0);
    }
    auto output_block_ptr = output_data_block_array[output_block_id].get();
    u32 next_candidate_id = 0;
    // handle one input block in one loop
    while (next_candidate_id < selected_row_ids.size()) {
        u32 input_block_id = selected_row_ids[next_candidate_id].first;
        auto input_block_ptr = input_data_block_array[input_block_id].get();
        // handle one input row in one loop
        while (next_candidate_id < selected_row_ids.size()) {
            auto [block_id, row_id] = selected_row_ids[next_candidate_id];
            if (block_id != input_block_id) {
                break;
            }
            if (output_block_row_id == DEFAULT_BLOCK_CAPACITY) {
                output_block_ptr->Finalize();
                ++output_block_id;
                output_block_ptr = output_data_block_array[output_block_id].get();
                output_block_row_id = 0;
            }
            output_block_ptr->AppendWith(input_block_ptr, row_id, 1);
            ++output_block_row_id;
            ++next_candidate_id;
        }
    }
    output_block_ptr->Finalize();
}

std::function<std::strong_ordering(const std::shared_ptr<ColumnVector> &, u32, const std::shared_ptr<ColumnVector> &, u32)>
InvalidPhysicalTopCompareType(const DataType &type_) {
    return [type_name =
                type_.ToString()](const std::shared_ptr<ColumnVector> &, u32, const std::shared_ptr<ColumnVector> &, u32) -> std::strong_ordering {
        UnrecoverableError(fmt::format("OrderBy LogicalType {} not implemented.", type_name));
        return std::strong_ordering::equal;
    };
}

// general template for POD types without three-way comparison but with operator< and operator==
template <OrderType compare_order, BinaryGenerateBoolean T>
struct PhysicalTopCompareSingleValue {
    static std::strong_ordering
    Compare(const std::shared_ptr<ColumnVector> &left_col, u32 left_id, const std::shared_ptr<ColumnVector> &right_col, u32 right_id) {
        auto compare_prefer_left = [](const T &x, const T &y) -> bool {
            if constexpr (compare_order == OrderType::kAsc) {
                return x < y;
            } else {
                return y < x;
            }
        };
        auto left = (reinterpret_cast<T *>(left_col->data()))[left_id];
        auto right = (reinterpret_cast<T *>(right_col->data()))[right_id];
        if (compare_prefer_left(left, right)) {
            return std::strong_ordering::less;
        } else if (left == right) {
            return std::strong_ordering::equal;
        } else {
            return std::strong_ordering::greater;
        }
    }
};

template <typename T>
concept ThreeWayComparePOD = PODValueType<T> and requires(T a, T b) {
    { a <=> b } -> std::convertible_to<std::strong_ordering>;
};

// template for POD types with three-way comparison method
template <OrderType compare_order, BinaryGenerateBoolean T>
    requires ThreeWayComparePOD<T>
struct PhysicalTopCompareSingleValue<compare_order, T> {
    static std::strong_ordering
    Compare(const std::shared_ptr<ColumnVector> &left_col, u32 left_id, const std::shared_ptr<ColumnVector> &right_col, u32 right_id) {
        auto left = (reinterpret_cast<T *>(left_col->data()))[left_id];
        auto right = (reinterpret_cast<T *>(right_col->data()))[right_id];
        if constexpr (compare_order == OrderType::kAsc) {
            return left <=> right;
        } else {
            return right <=> left;
        }
    }
};

// template for VarcharT and BooleanT types with three-way comparison method
template <OrderType compare_order, BinaryGenerateBoolean T>
    requires IsAnyOf<T, VarcharT, BooleanT>
struct PhysicalTopCompareSingleValue<compare_order, T> {
    static std::strong_ordering
    Compare(const std::shared_ptr<ColumnVector> &left_col, u32 left_id, const std::shared_ptr<ColumnVector> &right_col, u32 right_id) {
        ColumnValueReader<T> left(left_col);
        ColumnValueReader<T> right(right_col);
        if constexpr (compare_order == OrderType::kAsc) {
            return ThreeWayCompareReaderValue(left.SetIndex(left_id), right.SetIndex(right_id));
        } else {
            return ThreeWayCompareReaderValue(right.SetIndex(right_id), left.SetIndex(left_id));
        }
    }
};

template <OrderType compare_order>
inline std::function<std::strong_ordering(const std::shared_ptr<ColumnVector> &, u32, const std::shared_ptr<ColumnVector> &, u32)>
GenerateSortFunctionTemplate(std::shared_ptr<BaseExpression> &sort_expression) {
    switch (auto switch_type = sort_expression->Type().type(); switch_type) {
        case LogicalType::kBoolean: {
            return PhysicalTopCompareSingleValue<compare_order, BooleanT>::Compare;
        }
        case LogicalType::kTinyInt: {
            return PhysicalTopCompareSingleValue<compare_order, TinyIntT>::Compare;
        }
        case LogicalType::kSmallInt: {
            return PhysicalTopCompareSingleValue<compare_order, SmallIntT>::Compare;
        }
        case LogicalType::kInteger: {
            return PhysicalTopCompareSingleValue<compare_order, IntegerT>::Compare;
        }
        case LogicalType::kBigInt: {
            return PhysicalTopCompareSingleValue<compare_order, BigIntT>::Compare;
        }
        case LogicalType::kHugeInt: {
            return PhysicalTopCompareSingleValue<compare_order, HugeIntT>::Compare;
        }
        case LogicalType::kFloat16: {
            return PhysicalTopCompareSingleValue<compare_order, Float16T>::Compare;
        }
        case LogicalType::kBFloat16: {
            return PhysicalTopCompareSingleValue<compare_order, BFloat16T>::Compare;
        }
        case LogicalType::kFloat: {
            return PhysicalTopCompareSingleValue<compare_order, FloatT>::Compare;
        }
        case LogicalType::kDouble: {
            return PhysicalTopCompareSingleValue<compare_order, DoubleT>::Compare;
        }
        case LogicalType::kVarchar: {
            return PhysicalTopCompareSingleValue<compare_order, VarcharT>::Compare;
        }
        case LogicalType::kDate: {
            return PhysicalTopCompareSingleValue<compare_order, DateT>::Compare;
        }
        case LogicalType::kTime: {
            return PhysicalTopCompareSingleValue<compare_order, TimeT>::Compare;
        }
        case LogicalType::kDateTime: {
            return PhysicalTopCompareSingleValue<compare_order, DateTimeT>::Compare;
        }
        case LogicalType::kTimestamp: {
            return PhysicalTopCompareSingleValue<compare_order, TimestampT>::Compare;
        }
        case LogicalType::kRowID: {
            return PhysicalTopCompareSingleValue<compare_order, RowID>::Compare;
        }
        default: {
            return InvalidPhysicalTopCompareType(sort_expression->Type());
        }
    }
}

std::function<std::strong_ordering(const std::shared_ptr<ColumnVector> &, u32, const std::shared_ptr<ColumnVector> &, u32)>
PhysicalTop::GenerateSortFunction(OrderType compare_order, std::shared_ptr<BaseExpression> &sort_expression) {
    switch (compare_order) {
        case OrderType::kAsc: {
            return GenerateSortFunctionTemplate<OrderType::kAsc>(sort_expression);
        }
        case OrderType::kDesc: {
            return GenerateSortFunctionTemplate<OrderType::kDesc>(sort_expression);
        }
    }
}

void PhysicalTop::Init(QueryContext *query_context) {
    // Initialize sort parameters
    sort_expr_count_ = order_by_types_.size();
    if (sort_expr_count_ != sort_expressions_.size()) {
        UnrecoverableError("order_by_types_.size() != sort_expressions_.size()");
    }
    std::vector<std::function<std::strong_ordering(const std::shared_ptr<ColumnVector> &, u32, const std::shared_ptr<ColumnVector> &, u32)>>
        sort_functions;
    sort_functions.reserve(sort_expr_count_);
    for (u32 i = 0; i < sort_expr_count_; ++i) {
        sort_functions.emplace_back(GenerateSortFunction(order_by_types_[i], sort_expressions_[i]));
    }
    prefer_left_function_ = CompareTwoRowAndPreferLeft(std::move(sort_functions));
}

// Behavior now: always sort the output results, and `PhysicalTop::Execute()` may run multiple times.
bool PhysicalTop::Execute(QueryContext *, OperatorState *operator_state) {
    TopOperatorState *top_operator_state = (TopOperatorState *)operator_state;
    auto prev_op_state = top_operator_state->prev_op_state_;
    auto &input_data_block_array = prev_op_state->data_block_array_;
    auto &output_data_block_array = top_operator_state->data_block_array_;
    auto &execution_count = top_operator_state->execution_count_;

    size_t total_hits_row_count = std::accumulate(input_data_block_array.begin(), input_data_block_array.end(), 0, [](u32 x, const auto &y) -> u32 {
        return x + y->row_count();
    });
    // PhysicalPlanner::BuildTop() initialized `limit_` as `offset + limit`
    TopSolver top_solver(limit_, prefer_left_function_, sort_expressions_, top_operator_state->expr_states_);
    top_solver.Solve(input_data_block_array, output_data_block_array);
    execution_count++;
    input_data_block_array.clear();
    if (total_hits_count_flag_) {
        top_operator_state->total_hits_count_flag_ = true;
        top_operator_state->total_hits_count_ += total_hits_row_count;
    }
    if (prev_op_state->Complete()) {
        top_solver.Finalize(output_data_block_array, execution_count, offset_);
        top_operator_state->SetComplete();
        return true;
    }
    return false;
}

} // namespace infinity
