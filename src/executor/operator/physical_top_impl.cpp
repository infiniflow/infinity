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

module infinity_core:physical_top.impl;

import :physical_top;
import :stl;
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

class TopSolver {
public:
    explicit TopSolver(u32 limit, const CompareTwoRowAndPreferLeft &prefer_left_function)
        : limit_(limit), prefer_left_function_(prefer_left_function) {
        Init();
    }
    u32 WriteTopResultsToOutput(const std::vector<std::vector<std::shared_ptr<ColumnVector>>> &eval_columns,
                                const std::vector<std::unique_ptr<DataBlock>> &input_data_block_array,
                                std::vector<std::unique_ptr<DataBlock>> &output_data_block_array) {
        ResetInput(eval_columns);
        SolveTop();
        WriteToOutput(input_data_block_array, output_data_block_array);
        return size_;
    }

private:
    u32 size_{};
    u32 limit_{};
    const CompareTwoRowAndPreferLeft &prefer_left_function_; // sort functions
    std::unique_ptr<std::pair<u32, u32>[]> candidate_local_row_ids_;
    std::pair<u32, u32> *row_ids_ptr_ = nullptr; // with offset, start from 1, for heap sort
    const std::vector<std::vector<std::shared_ptr<ColumnVector>>> *input_data_ = nullptr;
    void Init() {
        candidate_local_row_ids_ = std::make_unique_for_overwrite<std::pair<u32, u32>[]>(limit_);
        row_ids_ptr_ = candidate_local_row_ids_.get() - 1;
    }
    void ResetInput(const std::vector<std::vector<std::shared_ptr<ColumnVector>>> &eval_columns) {
        size_ = 0;
        input_data_ = &eval_columns;
    }
    void HeapifyDown(u32 index, auto compare) {
        if (index == 0 || (index << 1) > size_) {
            return;
        }
        auto tmp_id = row_ids_ptr_[index];
        for (u32 sub; (sub = (index << 1)) <= size_; index = sub) {
            if (sub + 1 <= size_ && compare(row_ids_ptr_[sub + 1], row_ids_ptr_[sub])) {
                ++sub;
            }
            if (!compare(row_ids_ptr_[sub], tmp_id)) {
                break;
            }
            row_ids_ptr_[index] = row_ids_ptr_[sub];
        }
        row_ids_ptr_[index] = tmp_id;
    }
    void AddCandidate(std::pair<u32, u32> add_id, auto compare) {
        // when heap is full, only add candidate when compare(heap_top, add_id) is true
        if (size_ == limit_) {
            if (compare(row_ids_ptr_[1], add_id)) {
                row_ids_ptr_[1] = add_id;
                HeapifyDown(1, compare);
            }
        } else {
            ++size_;
            row_ids_ptr_[size_] = add_id;
            if (size_ == limit_) {
                for (u32 index = size_ / 2; index > 0; --index) {
                    HeapifyDown(index, compare);
                }
            }
        }
    }
    void SortResult(auto compare) {
        auto size_backup = size_;
        if (size_ < limit_) {
            for (u32 index = size_ / 2; index > 0; --index) {
                HeapifyDown(index, compare);
            }
        }
        while (size_ > 1) {
            std::swap(row_ids_ptr_[size_], row_ids_ptr_[1]);
            --size_;
            HeapifyDown(1, compare);
        }
        size_ = size_backup;
    }
    void SolveTop() {
        // compare_id_for_heap: for heap sort
        // example: x = heap_top, y = candidate, return true if y should be put into heap
        auto compare_id_for_heap = [&](std::pair<u32, u32> x, std::pair<u32, u32> y) -> bool {
            return !prefer_left_function_.Compare((*input_data_)[x.first], x.second, (*input_data_)[y.first], y.second);
        };
        const u32 input_block_cnt = input_data_->size();
        for (u32 block_id = 0; block_id < input_block_cnt; ++block_id) {
            const u32 row_cnt = (*input_data_)[block_id][0]->Size();
            for (u32 row_id = 0; row_id < row_cnt; ++row_id) {
                AddCandidate({block_id, row_id}, compare_id_for_heap);
            }
        }
        SortResult(compare_id_for_heap);
    }
    void WriteToOutput(const std::vector<std::unique_ptr<DataBlock>> &input_data_block_array, std::vector<std::unique_ptr<DataBlock>> &output_data_block_array) {
        auto const &db_types = input_data_block_array[0]->types();
        {
            // prepare output blocks
            i64 row_cnt = size_;
            do {
                auto data_block = DataBlock::MakeUniquePtr();
                // u32 need_capacity = Min(DEFAULT_BLOCK_CAPACITY, row_cnt);
                u32 need_capacity = DEFAULT_BLOCK_CAPACITY; // need to assume full capacity for MergeTop
                data_block->Init(db_types, need_capacity);
                output_data_block_array.emplace_back(std::move(data_block));
                row_cnt -= need_capacity;
            } while (row_cnt > 0);
        }
        u32 output_block_id = 0;
        u32 output_block_row_id = 0;
        auto output_block_ptr = output_data_block_array[output_block_id].get();
        u32 next_candidate_id = 0;
        // handle one input block in one loop
        while (next_candidate_id < size_) {
            u32 input_block_id = candidate_local_row_ids_[next_candidate_id].first;
            auto input_block_ptr = input_data_block_array[input_block_id].get();
            // handle one input row in one loop
            while (next_candidate_id < size_) {
                auto [block_id, row_id] = candidate_local_row_ids_[next_candidate_id];
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
};

std::function<std::strong_ordering(const std::shared_ptr<ColumnVector> &, u32, const std::shared_ptr<ColumnVector> &, u32)>
InvalidPhysicalTopCompareType(const DataType &type_) {
    return [type_name = type_.ToString()](const std::shared_ptr<ColumnVector> &, u32, const std::shared_ptr<ColumnVector> &, u32) -> std::strong_ordering {
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

void PhysicalTop::Init(QueryContext* query_context) {
    // Initialize sort parameters
    sort_expr_count_ = order_by_types_.size();
    if (sort_expr_count_ != sort_expressions_.size()) {
        UnrecoverableError("order_by_types_.size() != sort_expressions_.size()");
    }
    std::vector<std::function<std::strong_ordering(const std::shared_ptr<ColumnVector> &, u32, const std::shared_ptr<ColumnVector> &, u32)>> sort_functions;
    sort_functions.reserve(sort_expr_count_);
    for (u32 i = 0; i < sort_expr_count_; ++i) {
        sort_functions.emplace_back(GenerateSortFunction(order_by_types_[i], sort_expressions_[i]));
    }
    prefer_left_function_ = CompareTwoRowAndPreferLeft(std::move(sort_functions));
}

// Behavior now: always sort the output results
bool PhysicalTop::Execute(QueryContext *, OperatorState *operator_state) {
    TopOperatorState *top_operator_state = (TopOperatorState *)operator_state;
    auto prev_op_state = top_operator_state->prev_op_state_;
    if ((offset_ != 0) and !(prev_op_state->Complete())) {
        UnrecoverableError("Only 1 PhysicalTop job but !(prev_op_state->Complete())");
    }
    auto &input_data_block_array = prev_op_state->data_block_array_;
    auto &output_data_block_array = top_operator_state->data_block_array_;

    size_t total_hits_row_count = std::accumulate(input_data_block_array.begin(), input_data_block_array.end(), 0, [](u32 x, const auto &y) -> u32 {
        return x + y->row_count();
    });
    // sometimes the input_data_block_array is empty, but the operator is not complete
    if (total_hits_row_count == 0) {
        input_data_block_array.clear();
        if (prev_op_state->Complete()) {
            if (total_hits_count_flag_) {
                top_operator_state->total_hits_count_flag_ = true;
                top_operator_state->total_hits_count_ = 0;
            }
            top_operator_state->SetComplete();
        }
        return true;
    }
    if (!output_data_block_array.empty()) {
        UnrecoverableError("output data_block_array_ is not empty");
    }
    auto eval_columns = GetEvalColumns(sort_expressions_, top_operator_state->expr_states_, input_data_block_array);
    TopSolver solve_top(limit_, prefer_left_function_);
    auto output_row_cnt = solve_top.WriteTopResultsToOutput(eval_columns, input_data_block_array, output_data_block_array);
    input_data_block_array.clear();
    HandleOutputOffset(output_row_cnt, offset_, output_data_block_array);
    if (prev_op_state->Complete()) {
        if (total_hits_count_flag_) {
            top_operator_state->total_hits_count_flag_ = true;
            top_operator_state->total_hits_count_ = total_hits_row_count;
        }
        top_operator_state->SetComplete();
    }
    return true;
}

void PhysicalTop::HandleOutputOffset(u32 total_row_cnt, u32 offset, std::vector<std::unique_ptr<DataBlock>> &output_data_block_array) {
    if (offset == 0) {
        return;
    }
    if (offset >= total_row_cnt) {
        output_data_block_array.clear();
        return;
    }
    if (offset % DEFAULT_BLOCK_CAPACITY == 0) {
        output_data_block_array.erase(output_data_block_array.begin(), output_data_block_array.begin() + offset / DEFAULT_BLOCK_CAPACITY);
        return;
    }
    std::unique_ptr<DataBlock> swap_block;
    if (offset >= DEFAULT_BLOCK_CAPACITY) {
        std::swap(output_data_block_array[0], swap_block);
    } else {
        u32 need_capacity = DEFAULT_BLOCK_CAPACITY; // need to assume full capacity for MergeTop
        swap_block = DataBlock::MakeUniquePtr();
        swap_block->Init(output_data_block_array[0]->types(), need_capacity);
    }
    u32 result_row_cnt = total_row_cnt - offset;
    u32 result_block_cnt = result_row_cnt / DEFAULT_BLOCK_CAPACITY + ((result_row_cnt % DEFAULT_BLOCK_CAPACITY) != 0);
    for (u32 i = 0; i < result_block_cnt; ++i) {
        swap_block->Reset();
        u32 copy_row_cnt = std::min(DEFAULT_BLOCK_CAPACITY, result_row_cnt - i * DEFAULT_BLOCK_CAPACITY);
        u32 start_row_id = offset + i * DEFAULT_BLOCK_CAPACITY;
        u32 start_block_id = start_row_id / DEFAULT_BLOCK_CAPACITY;
        u32 start_block_offset = start_row_id % DEFAULT_BLOCK_CAPACITY;
        u32 start_block_copy_cnt = std::min(copy_row_cnt, u32(DEFAULT_BLOCK_CAPACITY - start_block_offset));
        swap_block->AppendWith(output_data_block_array[start_block_id].get(), start_block_offset, start_block_copy_cnt);
        u32 next_block_copy_cnt = copy_row_cnt - start_block_copy_cnt;
        if (next_block_copy_cnt > 0) {
            swap_block->AppendWith(output_data_block_array[start_block_id + 1].get(), 0, next_block_copy_cnt);
        }
        swap_block->Finalize();
        std::swap(swap_block, output_data_block_array[i]);
    }
    output_data_block_array.resize(result_block_cnt);
}

std::vector<std::vector<std::shared_ptr<ColumnVector>>> PhysicalTop::GetEvalColumns(const std::vector<std::shared_ptr<BaseExpression>> &expressions,
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

} // namespace infinity
