//
// Created by jinhai on 23-2-7.
//

#pragma once

#include "expression/base_expression.h"
#include "expression_state.h"

namespace infinity {

class ExpressionEvaluator {
public:
    void
    Execute(const SharedPtr<BaseExpression>& expression,
            SharedPtr<ExpressionState>& state,
            SharedPtr<DataBlock> input_data_block,
            SharedPtr<ColumnVector>& output_column_vector);

    void
    Execute(const SharedPtr<BaseExpression>& expr,
            SharedPtr<ExpressionState>& state,
            SharedPtr<ColumnVector>& output_column_vector,
            SizeT count);

    void
    Execute(const SharedPtr<AggregateExpression>& expr,
            SharedPtr<ExpressionState>& state,
            SharedPtr<ColumnVector>& output_column_vector,
            SizeT count);

    void
    Execute(const SharedPtr<CastExpression>& expr,
            SharedPtr<ExpressionState>& state,
            SharedPtr<ColumnVector>& output_column_vector,
            SizeT count);

    void
    Execute(const SharedPtr<CaseExpression>& expr,
            SharedPtr<ExpressionState>& state,
            SharedPtr<ColumnVector>& output_column_vector,
            SizeT count);

    void
    Execute(const SharedPtr<ConjunctionExpression>& expr,
            SharedPtr<ExpressionState>& state,
            SharedPtr<ColumnVector>& output_column_vector,
            SizeT count);

    void
    Execute(const SharedPtr<ColumnExpression>& expr,
            SharedPtr<ExpressionState>& state,
            SharedPtr<ColumnVector>& output_column_vector,
            SizeT count);

    void
    Execute(const SharedPtr<FunctionExpression>& expr,
            SharedPtr<ExpressionState>& state,
            SharedPtr<ColumnVector>& output_column_vector,
            SizeT count);

    void
    Execute(const SharedPtr<BetweenExpression>& expr,
            SharedPtr<ExpressionState>& state,
            SharedPtr<ColumnVector>& output_column_vector,
            SizeT count);

    void
    Execute(const SharedPtr<ValueExpression>& expr,
            SharedPtr<ExpressionState>& state,
            SharedPtr<ColumnVector>& output_column_vector,
            SizeT count);
private:
    SharedPtr<DataBlock> input_data_block_{nullptr};
    bool in_aggregate_{false};
};

}
