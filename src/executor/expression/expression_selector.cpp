//
// Created by jinhai on 23-3-12.
//

#include "expression/base_expression.h"
#include "executor/expression/expression_selector.h"
#include "executor/expression/expression_evaluator.h"
#include "common/column_vector/selection.h"
#include "common/column_vector/bitmask.h"
#include "common/column_vector/column_vector.h"
#include "storage/data_block.h"

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
    if (count == 0)
        return; // All data are false;
    ExecutorAssert(output_true_select != nullptr || output_false_select != nullptr, "No output select column vector is given")
        ExecutorAssert(expr->Type().type() == LogicalType::kBoolean, "Attempting to select non-boolean expression")
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

    const auto *bool_column_ptr = (const u8 *)(bool_column->data_ptr_);
    SharedPtr<Bitmask> &null_mask = bool_column->nulls_ptr_;

    Select(bool_column_ptr, null_mask, count, output_true_select, true);
}

void ExpressionSelector::Select(const u8 *__restrict bool_column,
                                const SharedPtr<Bitmask> &null_mask,
                                SizeT count,
                                SharedPtr<Selection> &output_true_select,
                                bool nullable) {
    if (nullable) {
        if (null_mask->IsAllTrue()) {
            for (SizeT idx = 0; idx < count; ++idx) {
                if (bool_column[idx] > 0) {
                    output_true_select->Append(idx);
                }
            }
        } else {
            const u64 *result_null_data = null_mask->GetData();
            SizeT unit_count = BitmaskBuffer::UnitCount(count);
            for (SizeT i = 0, start_index = 0, end_index = BitmaskBuffer::UNIT_BITS; i < unit_count; ++i, end_index += BitmaskBuffer::UNIT_BITS) {
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
                    ;
                } else {
                    SizeT original_start = start_index;
                    while (start_index < end_index) {
                        if (null_mask->IsTrue(start_index - original_start)) {
                            if (bool_column[start_index] > 0) {
                                output_true_select->Append(start_index);
                            }
                        }
                        ++start_index;
                    }
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