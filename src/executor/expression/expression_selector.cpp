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

module expression_selector;

import stl;
import data_block;
import base_expression;
import expression_state;
import selection;
import roaring_bitmap;
import logical_type;
import column_vector;
import expression_evaluator;
import internal_types;
import third_party;
import data_type;
import logger;

import infinity_exception;

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

    // Shrink the input data block into output data block
    // this Init function will throw if output_data_block is already initialized before
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
    if (output_true_select.get() == nullptr && output_false_select.get() == nullptr) {
        String error_message = "No output select column vector is given";
        UnrecoverableError(error_message);
    }
    if (expr->Type().type() != LogicalType::kBoolean) {
        String error_message = "Attempting to select non-boolean expression";
        UnrecoverableError(error_message);
    }
    Select(expr, state, count, output_true_select);
}

void ExpressionSelector::Select(const SharedPtr<BaseExpression> &expr,
                                SharedPtr<ExpressionState> &state,
                                SizeT count,
                                SharedPtr<Selection> &output_true_select) {
    SharedPtr<ColumnVector> bool_column = MakeShared<ColumnVector>(MakeShared<DataType>(LogicalType::kBoolean));
    bool_column->Initialize(ColumnVectorType::kCompactBit);

    ExpressionEvaluator expr_evaluator;
    expr_evaluator.Init(input_data_);
    expr_evaluator.Execute(expr, state, bool_column);

    Select(bool_column, count, output_true_select, true);
}

void ExpressionSelector::Select(const SharedPtr<ColumnVector> &bool_column, SizeT count, SharedPtr<Selection> &output_true_select, bool nullable) {
    if (bool_column->vector_type() != ColumnVectorType::kCompactBit || bool_column->data_type()->type() != LogicalType::kBoolean) {
        UnrecoverableError("Attempting to select non-boolean expression");
    }
    const auto &boolean_buffer = *(bool_column->buffer_);
    const auto &null_mask = bool_column->nulls_ptr_;
    if (nullable) {
        null_mask->RoaringBitmapApplyFunc([&](const u32 idx) -> bool {
            if (idx >= count) [[unlikely]] {
                return false;
            }
            if (boolean_buffer.GetCompactBit(idx)) {
                output_true_select->Append(idx);
            }
            return idx + 1 < count;
        });
    } else {
        for (SizeT idx = 0; idx < count; ++idx) {
            if (boolean_buffer.GetCompactBit(idx)) {
                output_true_select->Append(idx);
            }
        }
    }
}

} // namespace infinity