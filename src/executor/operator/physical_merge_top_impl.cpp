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

module infinity_core:physical_merge_top.impl;

import :physical_merge_top;
import :query_context;
import :operator_state;
import :base_expression;
import :value_expression;
import :expression_state;
import :infinity_exception;
import :expression_evaluator;
import :data_block;
import :column_vector;
import :default_values;
import :physical_top;
import :logger;

import std;

namespace infinity {

void PhysicalMergeTop::Init(QueryContext *query_context) {
    left()->Init(query_context);
    // Initialize sort parameters
    sort_expr_count_ = order_by_types_.size();
    if (sort_expr_count_ != sort_expressions_.size()) {
        UnrecoverableError("order_by_types_.size() != sort_expressions_.size()");
    }
    // copy compare function from PhysicalTop
    prefer_left_function_ = (reinterpret_cast<PhysicalTop *>(left()))->GetInnerCompareFunction();
}

bool PhysicalMergeTop::Execute(QueryContext *, OperatorState *operator_state) {
    auto &output_data_block_array = operator_state->data_block_array_;
    if (!output_data_block_array.empty()) {
        UnrecoverableError("output data_block_array_ is not empty");
    }
    auto merge_top_op_state = static_cast<MergeTopOperatorState *>(operator_state);
    auto &middle_data_block_array = merge_top_op_state->middle_sorted_data_blocks_;
    auto &input_data_block_array = merge_top_op_state->input_data_blocks_;
    auto &execution_count = merge_top_op_state->execution_count_;
    if (!input_data_block_array.empty()) {
        middle_data_block_array.insert(middle_data_block_array.end(),
                                       std::make_move_iterator(input_data_block_array.begin()),
                                       std::make_move_iterator(input_data_block_array.end()));
        input_data_block_array.clear();
        execution_count++;
    }
    if (merge_top_op_state->input_complete_) {
        // PhysicalPlanner::BuildTop() initialized `limit_` as `offset + limit`
        TopSolver top_solver(limit_, prefer_left_function_, sort_expressions_, merge_top_op_state->expr_states_);
        top_solver.Finalize(middle_data_block_array, execution_count, offset_);
        output_data_block_array = std::move(middle_data_block_array);
        operator_state->SetComplete();
        return true;
    }
    return false;
}

} // namespace infinity
