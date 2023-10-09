//
// Created by JinHai on 2022/10/26.
//

#pragma once

#include "expression/aggregate_expression.h"
#include "expression/base_expression.h"
#include "expression/between_expression.h"
#include "expression/case_expression.h"
#include "expression/cast_expression.h"
#include "expression/column_expression.h"
#include "expression/conjunction_expression.h"
#include "expression/function_expression.h"
#include "expression/in_expression.h"
#include "expression/reference_expression.h"
#include "expression/value_expression.h"
#include "storage/data_block.h"
#include <memory>
#include <vector>

namespace infinity {

class ExpressionState {
public:
    // Static functions
    static SharedPtr<ExpressionState> CreateState(const SharedPtr<BaseExpression> &expression);

    static SharedPtr<ExpressionState> CreateState(const SharedPtr<AggregateExpression> &agg_expr);

    static SharedPtr<ExpressionState> CreateState(const SharedPtr<CaseExpression> &agg_expr);

    static SharedPtr<ExpressionState> CreateState(const SharedPtr<CastExpression> &agg_expr);

    static SharedPtr<ExpressionState> CreateState(const SharedPtr<ReferenceExpression> &agg_expr);

    static SharedPtr<ExpressionState> CreateState(const SharedPtr<FunctionExpression> &agg_expr);

    static SharedPtr<ExpressionState> CreateState(const SharedPtr<ValueExpression> &agg_expr);

    static SharedPtr<ExpressionState> CreateState(const SharedPtr<InExpression> &in_expr);

public:
    void AddChild(const SharedPtr<BaseExpression> &expression);

    Vector<SharedPtr<ExpressionState>> &Children() { return children_; }

    SharedPtr<ColumnVector> &OutputColumnVector() { return column_vector_; }

private:
    Vector<SharedPtr<ExpressionState>> children_;
    String name_;

    // output blocks and each block have one output column;
    SharedPtr<ColumnVector> column_vector_;
};

} // namespace infinity
