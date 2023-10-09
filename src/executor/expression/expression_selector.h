//
// Created by jinhai on 23-3-12.
//

#pragma once

#include "executor/expression/expression_state.h"
#include "expression/base_expression.h"
#include "storage/data_block.h"

namespace infinity {

class ExpressionSelector {
public:
    SizeT Select(const SharedPtr<BaseExpression> &expr,
                 SharedPtr<ExpressionState> &state,
                 const DataBlock *input_data_block,
                 DataBlock *output_data_block,
                 SizeT count);

    void Select(const SharedPtr<BaseExpression> &expr,
                SharedPtr<ExpressionState> &state,
                SizeT count,
                const SharedPtr<Selection> &input_select,
                SharedPtr<Selection> &output_true_select,
                SharedPtr<Selection> &output_false_select);

    void Select(const SharedPtr<BaseExpression> &expr, SharedPtr<ExpressionState> &state, SizeT count, SharedPtr<Selection> &output_true_select);

    static void Select(const u8 *__restrict bool_column,
                       const SharedPtr<Bitmask> &null_mask,
                       SizeT count,
                       SharedPtr<Selection> &output_true_select,
                       bool nullable);

private:
    const DataBlock *input_data_{nullptr};
};

} // namespace infinity
