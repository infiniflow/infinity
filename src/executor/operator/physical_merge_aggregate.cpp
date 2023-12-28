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

void PhysicalMergeAggregate::Init() {}

bool PhysicalMergeAggregate::Execute(QueryContext *query_context, OperatorState *operator_state) {
    LOG_TRACE("PhysicalMergeAggregate::Execute:: mark");
    auto merge_aggregate_op_state = static_cast<MergeAggregateOperatorState *>(operator_state);

    if (!merge_aggregate_op_state->input_complete_) {

        // auto result = SimpleMergeAggregateExecute(merge_aggregate_op_state->input_data_blocks_, merge_aggregate_op_state->data_block_array_);

        if (merge_aggregate_op_state->data_block_array_.size() == 0) {
            merge_aggregate_op_state->data_block_array_.emplace_back(Move(merge_aggregate_op_state->input_data_block_));
        } else {

            auto agg_op = dynamic_cast<PhysicalAggregate *>(this->left());

            for (SizeT col_idx = 0; auto expr : agg_op->aggregates_) {
                auto agg_expression = static_cast<AggregateExpression *>(expr.get());

                auto function_name = agg_expression->aggregate_function_.GetFuncName();

                auto function_return_type = agg_expression->aggregate_function_.return_type_;

                if (String(function_name) == String("COUNT")) {
                    LOG_TRACE("PhysicalAggregate::Execute:: COUNT");
                    UpdateBlockData(merge_aggregate_op_state, col_idx);
                } else if (String(function_name) == String("MIN")) {
                    LOG_TRACE("PhysicalAggregate::Execute:: MIN");
                    IntegerT input_int = GetDataFromValueAtInputBlockPosition<IntegerT>(merge_aggregate_op_state, 0, col_idx, 0);
                    IntegerT out_int = GetDataFromValueAtOutputBlockPosition<IntegerT>(merge_aggregate_op_state, 0, col_idx, 0);
                    IntegerT new_int = MinValue(input_int, out_int);
                    WriteIntegerAtPosition(merge_aggregate_op_state, 0, col_idx, 0, new_int);
                } else if (String(function_name) == String("MAX")) {
                    LOG_TRACE("PhysicalAggregate::Execute:: MAX");

                    IntegerT input_int = GetDataFromValueAtInputBlockPosition<IntegerT>(merge_aggregate_op_state, 0, col_idx, 0);
                    IntegerT out_int = GetDataFromValueAtOutputBlockPosition<IntegerT>(merge_aggregate_op_state, 0, col_idx, 0);
                    IntegerT new_int = MaxValue(input_int, out_int);
                    WriteIntegerAtPosition(merge_aggregate_op_state, 0, col_idx, 0, new_int);
                } else if (String(function_name) == String("SUM")) {
                    UpdateBlockData(merge_aggregate_op_state, col_idx);
                }

                ++col_idx;
            }
        }
        return false;
    } else {
        LOG_TRACE("PhysicalAggregate::Input is complete");
        for (auto &output_block : merge_aggregate_op_state->data_block_array_) {
            output_block->Finalize();
        }
        merge_aggregate_op_state->SetComplete();
        return true;
    }
}

void PhysicalMergeAggregate::UpdateBlockData(MergeAggregateOperatorState *merge_aggregate_op_state, SizeT col_idx) {
    Value input_value = merge_aggregate_op_state->input_data_block_->GetValue(col_idx, 0);
    DataType input_value_type = input_value.type();
    if (!input_value_type.IsNumeric()) {
        Error<NotImplementException>("input_value_type not Implement");
    }

    switch (input_value_type.type()) {
        case kBigInt: {
            auto new_big_int = AddData<BigIntT>(input_value, merge_aggregate_op_state);
            merge_aggregate_op_state->data_block_array_[0]->SetValue(col_idx, 0, Value::MakeBigInt(new_big_int));
            return;
        }
        case kInteger: {
            auto new_int = AddData<BigIntT>(input_value, merge_aggregate_op_state);
            merge_aggregate_op_state->data_block_array_[0]->SetValue(col_idx, 0, Value::MakeInt(new_int));
            return;
        }
        case kDouble: {
            auto new_double = AddData<DoubleT>(input_value, merge_aggregate_op_state);
            merge_aggregate_op_state->data_block_array_[0]->SetValue(col_idx, 0, Value::MakeDouble(new_double));
            return;
        }
        default:
            Error<NotImplementException>("input_value_type not Implement");
    }
}

template <typename T>
T PhysicalMergeAggregate::AddData(Value input_value, MergeAggregateOperatorState *merge_aggregate_op_state) {
    T input = input_value.GetValue<T>();
    Value out_value = merge_aggregate_op_state->data_block_array_[0]->GetValue(0, 0);
    T out_put = out_value.GetValue<T>();
    T new_input = input + out_put;
    return new_input;
}

} // namespace infinity
