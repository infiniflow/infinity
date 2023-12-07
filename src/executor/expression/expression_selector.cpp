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
import data_block;
import base_expression;
import expression_state;
import selection;
import bitmask;

import bitmask_buffer;
import column_vector;
import expression_evaluator;

import third_party;

import infinity_exception;
import parser;

module expression_selector;

namespace infinity {

SizeT ExpressionSelector::Select(const SharedPtr<BaseExpression> &expr,
                                 SharedPtr<ExpressionState> &state,
                                 const DataBlock *input_data_block,
                                 DataBlock *output_data_block,
                                 SizeT count) {
    this->input_data_ = input_data_block;
    SharedPtr<Selection> input_select = nullptr;
    SharedPtr<Selection> output_true_select = MakeShared<Selection>();
    output_true_select->Initialize(count);
    SharedPtr<Selection> output_false_select = nullptr;

    Select(expr, state, count, input_select, output_true_select, output_false_select);

    output_data_block->UnInit();
    // Shrink the input data block into output data block
    output_data_block->Init(input_data_block, output_true_select);
    return output_true_select->Size();
}

void ExpressionSelector::Select(const SharedPtr<BaseExpression> &expr,
                                SharedPtr<ExpressionState> &state,
                                SizeT count,
                                const SharedPtr<Selection> &input_select,
                                SharedPtr<Selection> &output_true_select,
                                SharedPtr<Selection> &output_false_select) {
    if (count == 0) {
        return; // All data are false;
    }
    Assert<ExecutorException>(output_true_select.get() != nullptr || output_false_select.get() != nullptr, "No output select column vector is given");
    Assert<ExecutorException>(expr->Type().type() == LogicalType::kBoolean, "Attempting to select non-boolean expression");
    Select(expr, state, count, output_true_select);
}

void ExpressionSelector::Select(const SharedPtr<BaseExpression> &expr,
                                SharedPtr<ExpressionState> &state,
                                SizeT count,
                                SharedPtr<Selection> &output_true_select) {
    SharedPtr<ColumnVector> bool_column = MakeShared<ColumnVector>(MakeShared<DataType>(LogicalType::kBoolean));
    bool_column->Initialize();

    ExpressionEvaluator expr_evaluator;
    expr_evaluator.Init(input_data_);
    expr_evaluator.Execute(expr, state, bool_column);

    const auto *bool_column_ptr = (const u8 *)(bool_column->data());
    SharedPtr<Bitmask> &null_mask = bool_column->nulls_ptr_;

    Select(bool_column_ptr, null_mask, count, output_true_select, true);
}

void ExpressionSelector::Select(const u8 *__restrict bool_column,
                                const SharedPtr<Bitmask> &null_mask,
                                SizeT count,
                                SharedPtr<Selection> &output_true_select,
                                bool nullable) {
    if (nullable && !(null_mask->IsAllTrue())) {
            const u64 *result_null_data = null_mask->GetData();
            SizeT unit_count = BitmaskBuffer::UnitCount(count);
            for (SizeT i = 0, start_index = 0, end_index = BitmaskBuffer::UNIT_BITS; i < unit_count; ++i, end_index += BitmaskBuffer::UNIT_BITS) {
                end_index = Min(end_index, count);
                if (result_null_data[i] == BitmaskBuffer::UNIT_MAX) {
                    // all data of 64 rows are not null
                    while (start_index < end_index) {
                        if (bool_column[start_index] > 0) {
                            output_true_select->Append(start_index);
                        }
                        ++start_index;
                    }
                } else if (result_null_data[i] == BitmaskBuffer::UNIT_MIN) {
                    // all data of 64 rows are null
                    start_index = end_index;
                } else {
                    while (start_index < end_index) {
                        if ((null_mask->IsTrue(start_index)) && (bool_column[start_index] > 0)) {
                            output_true_select->Append(start_index);
                        }
                        ++start_index;
                    }
                }
            }
    } else {
        for (SizeT idx = 0; idx < count; ++idx) {
            if (bool_column[idx] > 0) {
                output_true_select->Append(idx);
            }
        }
    }
}

} // namespace infinity