//
// Created by JinHai on 2022/10/26.
//

#pragma once

#include <memory>
#include <vector>
#include "expression/base_expression.h"
#include "expression/aggregate_expression.h"
#include "expression/between_expression.h"
#include "expression/case_expression.h"
#include "expression/cast_expression.h"
#include "expression/column_expression.h"
#include "expression/conjunction_expression.h"
#include "expression/function_expression.h"
#include "expression/value_expression.h"
#include "storage/data_block.h"
#include "expression/reference_expression.h"

namespace infinity {

class ExpressionState {
public:
    // Static functions
    static SharedPtr<ExpressionState>
    CreateState(const SharedPtr<BaseExpression>& expression, SizeT block_count = 1);

    static SharedPtr<ExpressionState>
    CreateState(const SharedPtr<AggregateExpression>& agg_expr, SizeT block_count);

    static SharedPtr<ExpressionState>
    CreateState(const SharedPtr<BetweenExpression>& agg_expr, SizeT block_count);

    static SharedPtr<ExpressionState>
    CreateState(const SharedPtr<CaseExpression>& agg_expr, SizeT block_count);

    static SharedPtr<ExpressionState>
    CreateState(const SharedPtr<CastExpression>& agg_expr, SizeT block_count);

    static SharedPtr<ExpressionState>
    CreateState(const SharedPtr<ReferenceExpression>& agg_expr, SizeT block_count);

    static SharedPtr<ExpressionState>
    CreateState(const SharedPtr<ConjunctionExpression>& agg_expr, SizeT block_count);

    static SharedPtr<ExpressionState>
    CreateState(const SharedPtr<FunctionExpression>& agg_expr, SizeT block_count);

    static SharedPtr<ExpressionState>
    CreateState(const SharedPtr<ValueExpression>& agg_expr, SizeT block_count);
public:
    void
    AddChild(const SharedPtr<BaseExpression>& expression, SizeT block_count);

    Vector<SharedPtr<ExpressionState>>&
    Children() {
        return children_;
    }

    Vector<SharedPtr<ColumnVector>>&
    OutputColumnVectors() {
        return column_vectors_;
    }
private:
    Vector<SharedPtr<ExpressionState>> children_;
    String name_;
    Vector<SharedPtr<ColumnVector>> column_vectors_;
};

}

