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

namespace infinity {

//    case ExpressionType::kAggregate:
//    break;
//    case ExpressionType::kCast:
//    break;
//    case ExpressionType::kCase:
//    break;
//    case ExpressionType::kConjunction:
//    break;
//    case ExpressionType::kColumn:
//    break;
//    case ExpressionType::kFunction:
//    break;
//    case ExpressionType::kBetween:
//    break;
//    case ExpressionType::kValue:
//    break;

class ExpressionState {
public:
    // Static functions
    static std::shared_ptr<ExpressionState>
    CreateState(const std::shared_ptr<BaseExpression>& expression);

    static std::shared_ptr<ExpressionState>
    CreateState(const std::shared_ptr<AggregateExpression>& agg_expr);

    static std::shared_ptr<ExpressionState>
    CreateState(const std::shared_ptr<BetweenExpression>& agg_expr);

    static std::shared_ptr<ExpressionState>
    CreateState(const std::shared_ptr<CaseExpression>& agg_expr);

    static std::shared_ptr<ExpressionState>
    CreateState(const std::shared_ptr<CastExpression>& agg_expr);

    static std::shared_ptr<ExpressionState>
    CreateState(const std::shared_ptr<ColumnExpression>& agg_expr);

    static std::shared_ptr<ExpressionState>
    CreateState(const std::shared_ptr<ConjunctionExpression>& agg_expr);

    static std::shared_ptr<ExpressionState>
    CreateState(const std::shared_ptr<FunctionExpression>& agg_expr);

    static std::shared_ptr<ExpressionState>
    CreateState(const std::shared_ptr<ValueExpression>& agg_expr);
public:
    void
    AddChild(const std::shared_ptr<BaseExpression>& expression);

    std::vector<std::shared_ptr<ExpressionState>>& Children() { return children_; }

    Chunk& OutputChunk() { return output_block_.chunks_[0]; }
private:
    std::vector<std::shared_ptr<ExpressionState>> children_;
    std::string name_;
    TransBlock output_block_;
};

}

