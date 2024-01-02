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

#include <string>
#include <vector>
import stl;
import query_context;
import operator_state;
import infinity_exception;
import logger;
import value;
import data_block;
import parser;
import physical_aggregate;
import aggregate_expression;

module physical_merge_aggregate;

namespace infinity {

template <typename T>
using MathOperation = StdFunction<T(T, T)>;

void PhysicalMergeAggregate::Init() {}

bool PhysicalMergeAggregate::Execute(QueryContext *query_context, OperatorState *operator_state) {
    LOG_TRACE("PhysicalMergeAggregate::Execute:: mark");
    auto merge_aggregate_op_state = static_cast<MergeAggregateOperatorState *>(operator_state);

    SimpleMergeAggregateExecute(merge_aggregate_op_state);

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

void PhysicalMergeAggregate::SimpleMergeAggregateExecute(MergeAggregateOperatorState *op_state) {
    if (op_state->data_block_array_.empty()) {
        op_state->data_block_array_.emplace_back(Move(op_state->input_data_block_));
    } else {
        auto agg_op = dynamic_cast<PhysicalAggregate *>(this->left());
        auto aggs_size = agg_op->aggregates_.size();
        for (SizeT col_idx = 0; col_idx < aggs_size; ++col_idx) {
            auto agg_expression = static_cast<AggregateExpression *>(agg_op->aggregates_[col_idx].get());

            auto function_name = agg_expression->aggregate_function_.GetFuncName();

            auto func_return_type = agg_expression->aggregate_function_.return_type_;

            switch (func_return_type.type()) {
                case kInteger: {
                    HandleAggregateFunction<IntegerT>(function_name, op_state, col_idx);
                    break;
                }
                case kBigInt: {
                    HandleAggregateFunction<BigIntT>(function_name, op_state, col_idx);
                    break;
                }
                case kFloat: {
                    HandleAggregateFunction<FloatT>(function_name, op_state, col_idx);
                    break;
                }
                case kDouble: {
                    HandleAggregateFunction<DoubleT>(function_name, op_state, col_idx);
                    break;
                }
                default:
                    Error<NotImplementException>("input_value_type not Implement");
            }
        }
    }
}

template <typename T>
void PhysicalMergeAggregate::HandleAggregateFunction(const String &function_name, MergeAggregateOperatorState *op_state, SizeT col_idx) {
    if (String(function_name) == String("COUNT")) {
        LOG_TRACE("COUNT");
        HandleCount<T>(op_state, col_idx);
    } else if (String(function_name) == String("MIN")) {
        LOG_TRACE("MIN");
        HandleMin<T>(op_state, col_idx);
    } else if (String(function_name) == String("MAX")) {
        LOG_TRACE("MAX");
        HandleMax<T>(op_state, col_idx);
    } else if (String(function_name) == String("SUM")) {
        LOG_TRACE("SUM");
        HandleSum<T>(op_state, col_idx);
    }
}

template <typename T>
void PhysicalMergeAggregate::HandleMin(MergeAggregateOperatorState *op_state, SizeT col_idx) {
    MathOperation<T> minOperation = [](T a, T b) -> T { return (a < b) ? a : b; };
    UpdateData<T>(op_state, minOperation, col_idx);
}

template <typename T>
void PhysicalMergeAggregate::HandleMax(MergeAggregateOperatorState *op_state, SizeT col_idx) {
    MathOperation<T> maxOperation = [](T a, T b) -> T { return (a > b) ? a : b; };
    UpdateData<T>(op_state, maxOperation, col_idx);
}

template <typename T>
void PhysicalMergeAggregate::HandleCount(MergeAggregateOperatorState *op_state, SizeT col_idx) {
    MathOperation<T> countOperation = [](T a, T b) -> T { return a + b; };
    UpdateData<T>(op_state, countOperation, col_idx);
}

template <typename T>
void PhysicalMergeAggregate::HandleSum(MergeAggregateOperatorState *op_state, SizeT col_idx) {
    MathOperation<T> sumOperation = [](T a, T b) -> T { return a + b; };
    UpdateData<T>(op_state, sumOperation, col_idx);
}

template <typename T>
T PhysicalMergeAggregate::GetInputData(MergeAggregateOperatorState *op_state, SizeT block_index, SizeT col_idx, SizeT row_idx) {
    Value value = op_state->input_data_block_->GetValue(col_idx, row_idx);
    return value.GetValue<T>();
}

template <typename T>
T PhysicalMergeAggregate::GetOutputData(MergeAggregateOperatorState *op_state, SizeT block_index, SizeT col_idx, SizeT row_idx) {
    Value value = op_state->data_block_array_[block_index]->GetValue(col_idx, row_idx);
    return value.GetValue<T>();
}

template <typename T>
void PhysicalMergeAggregate::WriteValueAtPosition(MergeAggregateOperatorState *op_state, SizeT block_index, SizeT col_idx, SizeT row_idx, T value) {
    op_state->data_block_array_[block_index]->SetValue(col_idx, row_idx, CreateValue(value));
}

template <typename T>
void PhysicalMergeAggregate::UpdateData(MergeAggregateOperatorState *op_state, MathOperation<T> operation, SizeT col_idx) {
    T input = GetInputData<T>(op_state, 0, col_idx, 0);
    T output = GetOutputData<T>(op_state, 0, col_idx, 0);
    T new_value = operation(input, output);
    WriteValueAtPosition<T>(op_state, 0, col_idx, 0, new_value);
}

template <typename T>
T PhysicalMergeAggregate::AddData(T a, T b) {
    return a + b;
}

template <typename T>
T PhysicalMergeAggregate::MinValue(T a, T b) {
    return (a < b) ? a : b;
}

template <typename T>
T PhysicalMergeAggregate::MaxValue(T a, T b) {
    return (a > b) ? a : b;
}

} // namespace infinity
