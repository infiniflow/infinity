//
// Created by JinHai on 2022/8/11.
//

#include "expression/between_expression.h"
#include "expression/value_expression.h"
#include "expression/function_expression.h"
#include "expression/aggregate_expression.h"

#include "common/utility/infinity_assert.h"
#include "main/infinity.h"
#include "expression_binder.h"

namespace infinity {

std::shared_ptr<BaseExpression>
ExpressionBinder::BuildExpression(const hsql::Expr &expr, const std::shared_ptr<BindContext> &bind_context_ptr) {

    std::shared_ptr<BaseExpression> left = expr.expr ? BuildExpression(*expr.expr, bind_context_ptr) : nullptr;
    std::shared_ptr<BaseExpression> right = expr.expr2 ? BuildExpression(*expr.expr2, bind_context_ptr) : nullptr;

    LogicalType logical_type(LogicalTypeId::kNull);
    switch(expr.type) {
        case hsql::kExprLiteralFloat: {
            logical_type = LogicalType(LogicalTypeId::kDouble);
            return std::make_shared<ValueExpression>(logical_type, expr.fval);
        }
        case hsql::kExprLiteralInt: {
            // TODO: int16/int8 also can be found out.
//            logical_type = static_cast<int32_t>(expr.ival) == expr.ival ?
//                           LogicalType(LogicalTypeId::kInteger): LogicalType(LogicalTypeId::kBigInt);
            logical_type = LogicalType(LogicalTypeId::kBigInt);
            return std::make_shared<ValueExpression>(logical_type, expr.ival);
        }
        case hsql::kExprLiteralString: {
            logical_type = LogicalType(LogicalTypeId::kVarchar);
            PlannerAssert(expr.name != nullptr, "Literal string value is NULL.");
            return std::make_shared<ValueExpression>(logical_type, std::string(expr.name));
        }
        case hsql::kExprLiteralDate: {
            // TODO: transfer from date string (expr.name) to date int64
            int64_t date = 0;
            logical_type = LogicalType(LogicalTypeId::kDate);
            PlannerError("Date isn't supported during binding period.");
            return std::make_shared<ValueExpression>(logical_type, date);
        }
        case hsql::kExprLiteralInterval:
            // IntervalT should be a struct including the type of the value and an value of the interval
            // It will be bound into a ValueExpression here.
            PlannerError("Interval isn't supported during binding period.");
            break;
        case hsql::kExprLiteralNull: {
            logical_type = LogicalType(LogicalTypeId::kNull);
            return std::make_shared<ValueExpression>(logical_type);
        }
        case hsql::kExprColumnRef: {
            // TODO: Resolve column
            return BuildColExpr(expr, bind_context_ptr);
        }
        case hsql::kExprFunctionRef: {
            return BuildFuncExpr(expr, bind_context_ptr);
        }
        case hsql::kExprStar:
            PlannerError("Star expression shouldn't be bound here.");
            break;
        case hsql::kExprParameter:
            PlannerError("Used in prepare and execute? Not supported now.");
            break;
        case hsql::kExprOperator:
            // various expression
            break;
        case hsql::kExprSelect:
            // subquery expression
            break;
        case hsql::kExprHint:
            PlannerError("Hint isn't supported now.");
            break;
        case hsql::kExprArray:
            PlannerError("Array should be supported in IN expression.");
            break;
        case hsql::kExprArrayIndex:
            PlannerError("Array index isn't supported now.");
            break;
        case hsql::kExprExtract:
            // extract function expression
            break;
        case hsql::kExprCast:
            // cast function expression
            break;
        default:
            PlannerError("Unsupported expr type");
    }

    return std::shared_ptr<BaseExpression>();
}

std::shared_ptr<BaseExpression>
ExpressionBinder::BuildColExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr) {
    std::shared_ptr<std::string> column_name_ptr = std::make_shared<std::string>(expr.name);
    std::shared_ptr<std::string> table_name_ptr = nullptr;
    std::shared_ptr<std::string> alias_name_ptr = nullptr;
    if(expr.table != nullptr) {
        table_name_ptr = std::make_shared<std::string>(expr.table);
    }
    if(expr.alias != nullptr) {
        alias_name_ptr = std::make_shared<std::string>(expr.alias);
    }
    ColumnIdentifier column_identifier(table_name_ptr, column_name_ptr, alias_name_ptr);
    std::shared_ptr<BaseExpression> column_expr = bind_context_ptr->ResolveColumnIdentifier(column_identifier, 0);
    return column_expr;
}

std::shared_ptr<BaseExpression>
ExpressionBinder::BuildFuncExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr) {

    std::string function_name = expr.name;

    // Transfer the function to upper case.
    std::transform(function_name.begin(), function_name.end(), function_name.begin(), [](const auto c) {
        return std::toupper(c);
    });

    // std::unique_ptr<Catalog>& catalog
    auto& catalog = Infinity::instance().catalog();
    std::shared_ptr<FunctionSet> function_set_ptr = catalog->GetFunctionSetByName(function_name);

    std::vector<std::shared_ptr<BaseExpression>> arguments;
    std::vector<LogicalType> arg_types;
    arguments.reserve(expr.exprList->size());
    for(const auto* arg_expr : *expr.exprList) {
        // std::shared_ptr<BaseExpression> expr_ptr
        auto expr_ptr = BuildExpression(*arg_expr, bind_context_ptr);
        arguments.emplace_back(expr_ptr);
    }

    switch(function_set_ptr->type_) {
        case FunctionType::kScalar:{
            // std::shared_ptr<ScalarFunctionSet> scalar_function_set_ptr
            auto scalar_function_set_ptr = std::static_pointer_cast<ScalarFunctionSet>(function_set_ptr);
            ScalarFunction scalar_function = scalar_function_set_ptr->GetMostMatchFunction(arguments);
            std::shared_ptr<FunctionExpression> function_expr_ptr
                = std::make_shared<FunctionExpression>(scalar_function, arguments);
            return function_expr_ptr;
        }
        case FunctionType::kAggregate: {
            // std::shared_ptr<AggregateFunctionSet> aggregate_function_set_ptr
            auto aggregate_function_set_ptr = std::static_pointer_cast<AggregateFunctionSet>(function_set_ptr);
            AggregateFunction aggregate_function = aggregate_function_set_ptr->GetMostMatchFunction(arguments);
            auto aggregate_function_ptr = std::make_shared<AggregateExpression>(aggregate_function, arguments);
            return aggregate_function_ptr;
        }
        case FunctionType::kTable:
            PlannerError("Table function shouldn't be bound here.");
        default: {
            PlannerError("Unknown function type: " + function_set_ptr->name());
        }
    }
}

std::shared_ptr<BaseExpression>
ExpressionBinder::BuildOperatorExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr) {

    switch (expr.opType) {
            case hsql::kOpBetween: {
                std::shared_ptr<BaseExpression> value = BuildExpression(*expr.expr, bind_context_ptr);
                std::shared_ptr<BaseExpression> left_bound = BuildExpression(*(*expr.exprList)[0], bind_context_ptr);
                std::shared_ptr<BaseExpression> right_bound = BuildExpression(*(*expr.exprList)[1], bind_context_ptr);
                return std::make_shared<BetweenExpression>(value, left_bound, right_bound, true, true);
            }
            case hsql::kOpCase: {
                break;
            }
            case hsql::kOpCaseListElement:
                PlannerError("Unexpected expression type");
            case hsql::kOpPlus: // +
                break;
            case hsql::kOpMinus: // -
                break;
            case hsql::kOpAsterisk: // *
                break;
            case hsql::kOpSlash: // /
                break;
            case hsql::kOpPercentage: // %
                break;
            case hsql::kOpCaret: // ^
                break;
            case hsql::kOpEquals: // =
                break;
            case hsql::kOpNotEquals: // <>
                break;
            case hsql::kOpLess: // <
                break;
            case hsql::kOpLessEq: // <=
                break;
            case hsql::kOpGreater: // >
                break;
            case hsql::kOpGreaterEq: // >=
                break;
            case hsql::kOpLike: // like
                break;
            case hsql::kOpNotLike: // not like
                break;
            case hsql::kOpILike: // ilike
                break;
            case hsql::kOpAnd: // AND
                break;
            case hsql::kOpOr: // OR
                break;
            case hsql::kOpIn: // IN
                break;
            case hsql::kOpConcat: // Concat
                break;
            case hsql::kOpNot: // Not
                break;
            case hsql::kOpUnaryMinus: // -
                break;
            case hsql::kOpIsNull: // IsNull
                break;
            case hsql::kOpExists: // Exists
                break;
            default: {

            }
    }

    PlannerError("ExpressionBinder::Build Operator");
}

//// Bind subquery expression.
//std::shared_ptr<BaseExpression>
//ExpressionBinder::BuildSubquery(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr) {
//    PlannerError("ExpressionBinder::BuildSubquery");
//}
//
//// Bind window function.
//std::shared_ptr<BaseExpression>
//ExpressionBinder::BuildWindow(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr) {
//    PlannerError("ExpressionBinder::BuildWindow");
//}

}
