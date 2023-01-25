//
// Created by JinHai on 2022/10/26.
//

#pragma once

#include "storage/table.h"
#include "expression/base_expression.h"
#include "expression_state.h"
#include "common/column_vector/selection.h"

#include <unordered_map>

namespace infinity {

class ExpressionExecutor {
public:
    ExpressionExecutor() = default;

    void
    Init(const Vector<SharedPtr<BaseExpression>>& expressions);

    void
    Select(SharedPtr<Table>& input, SharedPtr<Table>& output);

    SizeT
    Select(const SharedPtr<DataBlock>& input_data_block, SharedPtr<DataBlock>& output_data_block);

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

    // Evaluate all expressions
    void
    Execute(const SharedPtr<Table>& input, SharedPtr<Table>& output);

    void
    Execute(const SharedPtr<DataBlock>& input_data_block, SharedPtr<DataBlock>& output_data_block);

    // Method to evaluate each expression recursively
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
    Vector<SharedPtr<BaseExpression>> expressions;
    Vector<SharedPtr<ExpressionState>> states;
    HashMap<String, SharedPtr<Table>> table_map_;
    SharedPtr<DataBlock> input_data_{nullptr};
};

}
