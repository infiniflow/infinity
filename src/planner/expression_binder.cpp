//
// Created by JinHai on 2022/8/11.
//

#include "expression_binder.h"
#include "expression/value_expression.h"
#include "common/utility/infinity_assert.h"

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
            // Check the function alias

            // Check if the function is aggregate function

            // Normal function
            PlannerError("Function reference");
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
    std::string expr_name = std::string(expr.getName());
    std::optional<std::string> table_name = nullptr;
    if(expr.table != nullptr) {
        table_name = std::optional<std::string>(expr_name);
    }
    ColumnIdentifier column_identifier(table_name, expr_name);
    std::shared_ptr<BaseExpression> column_expr = bind_context_ptr->ResolveColumnIdentifier(column_identifier);
    return column_expr;
}

//// Bind aggregate function.
//std::shared_ptr<BaseExpression>
//ExpressionBinder::BuildAggFunc(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr) {
//    PlannerError("ExpressionBinder::BuildAggFunc");
//    return std::shared_ptr<BaseExpression>();
//}
//
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
