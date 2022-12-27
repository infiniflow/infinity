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
    Init(const std::vector<std::shared_ptr<BaseExpression>>& expressions);

    // Evaluate all expressions
    void
    Execute(std::shared_ptr<Table>& input, std::shared_ptr<Table>& output);

    // Method to evaluate each expression recursively
    void
    Execute(std::shared_ptr<BaseExpression>& expr,
            std::shared_ptr<ExpressionState>& state,
            ColumnVector& output_column_vector,
            size_t count);

    void
    Execute(const std::shared_ptr<AggregateExpression>& expr,
            std::shared_ptr<ExpressionState>& state,
            ColumnVector& output_column_vector,
            size_t count);

    void
    Execute(const std::shared_ptr<CastExpression>& expr,
            std::shared_ptr<ExpressionState>& state,
            ColumnVector& output_column_vector,
            size_t count);

    void
    Execute(const std::shared_ptr<CaseExpression>& expr,
            std::shared_ptr<ExpressionState>& state,
            ColumnVector& output_column_vector,
            size_t count);

    void
    Execute(const std::shared_ptr<ConjunctionExpression>& expr,
            std::shared_ptr<ExpressionState>& state,
            ColumnVector& output_column_vector,
            size_t count);

    void
    Execute(const std::shared_ptr<ColumnExpression>& expr,
            std::shared_ptr<ExpressionState>& state,
            ColumnVector& output_column_vector,
            size_t count);

    void
    Execute(const std::shared_ptr<FunctionExpression>& expr,
            std::shared_ptr<ExpressionState>& state,
            ColumnVector& output_column_vector,
            size_t count);

    void
    Execute(const std::shared_ptr<BetweenExpression>& expr,
            std::shared_ptr<ExpressionState>& state,
            ColumnVector& output_column_vector,
            size_t count);

    void
    Execute(const std::shared_ptr<ValueExpression>& expr,
            std::shared_ptr<ExpressionState>& state,
            ColumnVector& output_column_vector,
            size_t count);

private:
    std::vector<std::shared_ptr<BaseExpression>> expressions;
    std::vector<std::shared_ptr<ExpressionState>> states;
    std::unordered_map<std::string, std::shared_ptr<Table>> table_map_;
};

}
