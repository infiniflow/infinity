//
// Created by jinhai on 23-3-12.
//

#pragma once

#include "expression/base_expression.h"
#include "storage/data_block.h"
#include "executor/expression/expression_state.h"

namespace infinity {

class ExpressionSelector {
public:
    SizeT
    Select(const SharedPtr<BaseExpression>& expr,
           SharedPtr<ExpressionState>& state,
           const SharedPtr<DataBlock>& input_data_block,
           SharedPtr<DataBlock>& output_data_block,
           SizeT count);

    void
    Select(const SharedPtr<BaseExpression>& expr,
           SharedPtr<ExpressionState>& state,
           SizeT count,
           const SharedPtr<Selection>& input_select,
           SharedPtr<Selection>& output_true_select,
           SharedPtr<Selection>& output_false_select);

    void
    Select(const SharedPtr<BaseExpression>& expr,
           SharedPtr<ExpressionState>& state,
           SizeT count,
           SharedPtr<Selection>& output_true_select);

    static void
    Select(const u8 *__restrict bool_column,
           const SharedPtr<Bitmask>& null_mask,
           SizeT count,
           SharedPtr<Selection>& output_true_select,
           bool nullable);

private:
    SharedPtr<DataBlock> input_data_{nullptr};
};

}


