//
// Created by jinhai on 23-2-7.
//

#pragma once

#include "expression/base_expression.h"
#include "expression/reference_expression.h"
#include "expression_state.h"

namespace infinity {

class ExpressionEvaluator {
public:
    void Init(const DataBlock *input_data_block);

    void Execute(const SharedPtr<BaseExpression> &expr, SharedPtr<ExpressionState> &state, SharedPtr<ColumnVector> &output_column_vector);

    void Execute(const SharedPtr<AggregateExpression> &expr, SharedPtr<ExpressionState> &state, SharedPtr<ColumnVector> &output_column_vector);

    void Execute(const SharedPtr<CastExpression> &expr, SharedPtr<ExpressionState> &state, SharedPtr<ColumnVector> &output_column_vector);

    void Execute(const SharedPtr<CaseExpression> &expr, SharedPtr<ExpressionState> &state, SharedPtr<ColumnVector> &output_column_vector);

    void Execute(const SharedPtr<ColumnExpression> &expr, SharedPtr<ExpressionState> &state, SharedPtr<ColumnVector> &output_column_vector);

    void Execute(const SharedPtr<FunctionExpression> &expr, SharedPtr<ExpressionState> &state, SharedPtr<ColumnVector> &output_column_vector);

    void Execute(const SharedPtr<ValueExpression> &expr, SharedPtr<ExpressionState> &state, SharedPtr<ColumnVector> &output_column_vector);

    void Execute(const SharedPtr<ReferenceExpression> &expr, SharedPtr<ExpressionState> &state, SharedPtr<ColumnVector> &output_column_vector);

    void Execute(const SharedPtr<InExpression> &expr, SharedPtr<ExpressionState> &state, SharedPtr<ColumnVector> &output_column_vector);

private:
    const DataBlock *input_data_block_{};
    bool in_aggregate_{false};
};

} // namespace infinity
