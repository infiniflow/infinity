//
// Created by JinHai on 2022/10/26.
//

#pragma once

#include "storage/table.h"
#include "expression/base_expression.h"
#include "expression_state.h"

#include <unordered_map>

namespace infinity {

class ExpressionExecutor {
public:
    ExpressionExecutor() = default;

    void
    Init(const std::vector<SharedPtr<BaseExpression>>& expressions);

    // Evaluate all expressions
    void
    Execute(SharedPtr<Table>& input, SharedPtr<Table>& output);

    void
    Execute(const SharedPtr<DataBlock>& input_data_block, SharedPtr<DataBlock>& output_data_block);

    // Method to evaluate each expression recursively
    void
    Execute(SharedPtr<BaseExpression>& expr,
            SharedPtr<ExpressionState>& state,
            SharedPtr<ColumnVector>& output_column_vector,
            size_t count);

    void
    Execute(const SharedPtr<AggregateExpression>& expr,
            SharedPtr<ExpressionState>& state,
            SharedPtr<ColumnVector>& output_column_vector,
            size_t count);

    void
    Execute(const SharedPtr<CastExpression>& expr,
            SharedPtr<ExpressionState>& state,
            SharedPtr<ColumnVector>& output_column_vector,
            size_t count);

    void
    Execute(const SharedPtr<CaseExpression>& expr,
            SharedPtr<ExpressionState>& state,
            SharedPtr<ColumnVector>& output_column_vector,
            size_t count);

    void
    Execute(const SharedPtr<ConjunctionExpression>& expr,
            SharedPtr<ExpressionState>& state,
            SharedPtr<ColumnVector>& output_column_vector,
            size_t count);

    void
    Execute(const SharedPtr<ColumnExpression>& expr,
            SharedPtr<ExpressionState>& state,
            SharedPtr<ColumnVector>& output_column_vector,
            size_t count);

    void
    Execute(const SharedPtr<FunctionExpression>& expr,
            SharedPtr<ExpressionState>& state,
            SharedPtr<ColumnVector>& output_column_vector,
            size_t count);

    void
    Execute(const SharedPtr<BetweenExpression>& expr,
            SharedPtr<ExpressionState>& state,
            SharedPtr<ColumnVector>& output_column_vector,
            size_t count);

    void
    Execute(const SharedPtr<ValueExpression>& expr,
            SharedPtr<ExpressionState>& state,
            SharedPtr<ColumnVector>& output_column_vector,
            size_t count);

private:
    std::vector<SharedPtr<BaseExpression>> expressions;
    std::vector<SharedPtr<ExpressionState>> states;
    std::unordered_map<std::string, SharedPtr<Table>> table_map_;
    SharedPtr<DataBlock> input_data_{nullptr};
};

}
