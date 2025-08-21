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

module infinity_core:physical_merge_aggregate.impl;

import :physical_merge_aggregate;
import :query_context;
import :operator_state;
import :logger;
import :value;
import :data_block;
import :physical_aggregate;
import :aggregate_expression;
import :infinity_exception;
import :hash_table;
import :column_vector;

import std;
import third_party;

import internal_types;
import data_type;
import column_def;
import logical_type;

namespace infinity {

template <typename T>
using MathOperation = std::function<T(T, T)>;

void PhysicalMergeAggregate::Init(QueryContext *query_context) {}

bool PhysicalMergeAggregate::Execute(QueryContext *query_context, OperatorState *operator_state) {

    auto merge_aggregate_op_state = static_cast<MergeAggregateOperatorState *>(operator_state);
    auto agg_op = dynamic_cast<PhysicalAggregate *>(this->left());
    if (agg_op->groups_.size() == 0) {
        SimpleMergeAggregateExecute(merge_aggregate_op_state);
    } else {
        GroupByMergeAggregateExecute(merge_aggregate_op_state);
    }

    if (merge_aggregate_op_state->input_complete_) {

        LOG_TRACE("PhysicalMergeAggregate::Input is complete");
        for (auto &output_block : merge_aggregate_op_state->data_block_array_) {
            output_block->Finalize();
        }

        merge_aggregate_op_state->SetComplete();
        return true;
    }

    return false;
}

void PhysicalMergeAggregate::GroupByMergeAggregateExecute(MergeAggregateOperatorState *op_state) {
    auto *agg_op = static_cast<PhysicalAggregate *>(this->left());
    size_t group_count = agg_op->groups_.size();
    MergeHashTable &hash_table = op_state->hash_table_;

    auto &input_block = op_state->input_data_block_;
    if (!hash_table.Initialized()) {
        std::vector<std::shared_ptr<DataType>> groupby_types;
        groupby_types.reserve(group_count);
        for (i64 idx = 0; auto &expr : agg_op->groups_) {
            std::shared_ptr<ColumnDef> col_def =
                std::make_shared<ColumnDef>(idx, std::make_shared<DataType>(expr->Type()), expr->Name(), std::set<ConstraintType>());
            groupby_types.emplace_back(std::make_shared<DataType>(expr->Type()));
            ++idx;
        }

        hash_table.Init(groupby_types);
    }

    if (input_block == nullptr) {
        return;
    }

    std::vector<std::shared_ptr<ColumnVector>> input_groupby_columns(input_block->column_vectors.begin(),
                                                                     input_block->column_vectors.begin() + group_count);
    if (op_state->data_block_array_.empty()) {
        hash_table.Append(input_groupby_columns, 0, input_block->row_count());
        op_state->data_block_array_.emplace_back(std::move(input_block));
        LOG_TRACE("Physical MergeAggregate execute first block");
        return;
    }

    DataBlock *last_data_block = op_state->data_block_array_.back().get();
    for (size_t row_id = 0; row_id < input_block->row_count(); ++row_id) {
        if (last_data_block->available_capacity() == 0) {
            std::vector<std::shared_ptr<DataType>> types = last_data_block->types();
            op_state->data_block_array_.emplace_back(DataBlock::MakeUniquePtr());
            last_data_block = op_state->data_block_array_.back().get();
            last_data_block->Init(std::move(types), input_block->capacity());
        }
        std::pair<size_t, size_t> block_row_id = {op_state->data_block_array_.size() - 1, last_data_block->row_count()};
        bool found = hash_table.GetOrInsert(input_groupby_columns, row_id, block_row_id);
        if (!found) {
            last_data_block->AppendWith(input_block.get(), row_id, 1);
            continue;
        }
        size_t agg_count = agg_op->aggregates_.size();
        std::pair<size_t, size_t> input_block_row_id = {0, row_id};
        for (size_t col_idx = group_count; col_idx < group_count + agg_count; ++col_idx) {
            auto *agg_expression = static_cast<AggregateExpression *>(agg_op->aggregates_[col_idx - group_count].get());

            auto function_name = agg_expression->aggregate_function_.GetFuncName();

            auto func_return_type = agg_expression->aggregate_function_.return_type_;

            switch (func_return_type.type()) {
                LOG_TRACE("Physical MergeAggregate execute remain block");
                case LogicalType::kTinyInt: {
                    HandleAggregateFunction<TinyIntT>(function_name, op_state, col_idx, input_block_row_id, block_row_id);
                    break;
                }
                case LogicalType::kSmallInt: {
                    HandleAggregateFunction<SmallIntT>(function_name, op_state, col_idx, input_block_row_id, block_row_id);
                    break;
                }
                case LogicalType::kInteger: {
                    HandleAggregateFunction<IntegerT>(function_name, op_state, col_idx, input_block_row_id, block_row_id);
                    break;
                }
                case LogicalType::kBigInt: {
                    HandleAggregateFunction<BigIntT>(function_name, op_state, col_idx, input_block_row_id, block_row_id);
                    break;
                }
                case LogicalType::kFloat: {
                    HandleAggregateFunction<FloatT>(function_name, op_state, col_idx, input_block_row_id, block_row_id);
                    break;
                }
                case LogicalType::kDouble: {
                    HandleAggregateFunction<DoubleT>(function_name, op_state, col_idx, input_block_row_id, block_row_id);
                    break;
                }
                default: {
                    UnrecoverableError("Input value type not Implement");
                }
            }
        }
    }
}

void PhysicalMergeAggregate::SimpleMergeAggregateExecute(MergeAggregateOperatorState *op_state) {
    if (op_state->data_block_array_.empty()) {
        op_state->data_block_array_.emplace_back(std::move(op_state->input_data_block_));
        LOG_TRACE("Physical MergeAggregate execute first block");
    } else {
        auto agg_op = dynamic_cast<PhysicalAggregate *>(this->left());
        auto aggs_size = agg_op->aggregates_.size();
        for (size_t col_idx = 0; col_idx < aggs_size; ++col_idx) {
            auto agg_expression = static_cast<AggregateExpression *>(agg_op->aggregates_[col_idx].get());

            auto function_name = agg_expression->aggregate_function_.GetFuncName();

            auto func_return_type = agg_expression->aggregate_function_.return_type_;

            switch (func_return_type.type()) {
                LOG_TRACE("Physical MergeAggregate execute remain block");
                case LogicalType::kTinyInt: {
                    HandleAggregateFunction<TinyIntT>(function_name, op_state, col_idx);
                    break;
                }
                case LogicalType::kSmallInt: {
                    HandleAggregateFunction<SmallIntT>(function_name, op_state, col_idx);
                    break;
                }
                case LogicalType::kInteger: {
                    HandleAggregateFunction<IntegerT>(function_name, op_state, col_idx);
                    break;
                }
                case LogicalType::kBigInt: {
                    HandleAggregateFunction<BigIntT>(function_name, op_state, col_idx);
                    break;
                }
                case LogicalType::kFloat: {
                    HandleAggregateFunction<FloatT>(function_name, op_state, col_idx);
                    break;
                }
                case LogicalType::kDouble: {
                    HandleAggregateFunction<DoubleT>(function_name, op_state, col_idx);
                    break;
                }
                default: {
                    UnrecoverableError("Input value type not Implement");
                }
            }
        }
    }
}

template <typename T>
void PhysicalMergeAggregate::HandleAggregateFunction(const std::string &function_name,
                                                     MergeAggregateOperatorState *op_state,
                                                     size_t col_idx,
                                                     const std::pair<size_t, size_t> &input_block_row_id,
                                                     const std::pair<size_t, size_t> &output_block_row_id) {
    LOG_TRACE(function_name);
    if (function_name == "COUNT") {
        LOG_TRACE("COUNT");
        HandleCount<T>(op_state, col_idx, input_block_row_id, output_block_row_id);
    } else if (function_name == "MIN") {
        HandleMin<T>(op_state, col_idx, input_block_row_id, output_block_row_id);
    } else if (function_name == "MAX") {
        HandleMax<T>(op_state, col_idx, input_block_row_id, output_block_row_id);
    } else if (function_name == "SUM") {
        HandleSum<T>(op_state, col_idx, input_block_row_id, output_block_row_id);
    } else if (function_name == "COUNT_STAR") {
        // no action for "COUNT_STAR"
    } else {
        UnrecoverableError(fmt::format("Function type {} not Implement.", function_name));
    }
}

template <typename T>
void PhysicalMergeAggregate::HandleMin(MergeAggregateOperatorState *op_state,
                                       size_t col_idx,
                                       const std::pair<size_t, size_t> &input_block_row_id,
                                       const std::pair<size_t, size_t> &output_block_row_id) {
    MathOperation<T> minOperation = [](T a, T b) -> T { return (a < b) ? a : b; };
    UpdateData<T>(op_state, minOperation, col_idx, input_block_row_id, output_block_row_id);
}

template <typename T>
void PhysicalMergeAggregate::HandleMax(MergeAggregateOperatorState *op_state,
                                       size_t col_idx,
                                       const std::pair<size_t, size_t> &input_block_row_id,
                                       const std::pair<size_t, size_t> &output_block_row_id) {
    MathOperation<T> maxOperation = [](T a, T b) -> T { return (a > b) ? a : b; };
    UpdateData<T>(op_state, maxOperation, col_idx, input_block_row_id, output_block_row_id);
}

template <typename T>
void PhysicalMergeAggregate::HandleCount(MergeAggregateOperatorState *op_state,
                                         size_t col_idx,
                                         const std::pair<size_t, size_t> &input_block_row_id,
                                         const std::pair<size_t, size_t> &output_block_row_id) {
    MathOperation<T> countOperation = [](T a, T b) -> T { return a + b; };
    UpdateData<T>(op_state, countOperation, col_idx, input_block_row_id, output_block_row_id);
}

template <typename T>
void PhysicalMergeAggregate::HandleSum(MergeAggregateOperatorState *op_state,
                                       size_t col_idx,
                                       const std::pair<size_t, size_t> &input_block_row_id,
                                       const std::pair<size_t, size_t> &output_block_row_id) {
    MathOperation<T> sumOperation = [](T a, T b) -> T { return a + b; };
    UpdateData<T>(op_state, sumOperation, col_idx, input_block_row_id, output_block_row_id);
}

template <typename T>
T PhysicalMergeAggregate::GetInputData(MergeAggregateOperatorState *op_state, size_t block_index, size_t col_idx, size_t row_idx) {
    Value value = op_state->input_data_block_->GetValue(col_idx, row_idx);
    return value.GetValue<T>();
}

template <typename T>
T PhysicalMergeAggregate::GetOutputData(MergeAggregateOperatorState *op_state, size_t block_index, size_t col_idx, size_t row_idx) {
    Value value = op_state->data_block_array_[block_index]->GetValue(col_idx, row_idx);
    return value.GetValue<T>();
}

template <typename T>
void PhysicalMergeAggregate::WriteValueAtPosition(MergeAggregateOperatorState *op_state,
                                                  size_t block_index,
                                                  size_t col_idx,
                                                  size_t row_idx,
                                                  T value) {
    op_state->data_block_array_[block_index]->SetValue(col_idx, row_idx, CreateValue(value));
}

template <typename T>
void PhysicalMergeAggregate::UpdateData(MergeAggregateOperatorState *op_state,
                                        MathOperation<T> operation,
                                        size_t col_idx,
                                        const std::pair<size_t, size_t> &input_block_row_id,
                                        const std::pair<size_t, size_t> &output_block_row_id) {
    const auto &[input_block_id, input_row_id] = input_block_row_id;
    const auto &[output_block_id, output_row_id] = output_block_row_id;
    T input = GetInputData<T>(op_state, input_block_id, col_idx, input_row_id);
    T output = GetOutputData<T>(op_state, output_block_id, col_idx, output_row_id);
    T new_value = operation(input, output);
    WriteValueAtPosition<T>(op_state, output_block_id, col_idx, output_row_id, new_value);
}

} // namespace infinity
