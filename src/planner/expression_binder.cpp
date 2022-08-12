//
// Created by JinHai on 2022/8/11.
//

#include "expression_binder.h"
#include "expression/value_expression.h"
#include "common/utility/asserter.h"

namespace infinity {

std::shared_ptr<BaseExpression>
ExpressionBinder::BuildExpression(const hsql::Expr &expr, const std::shared_ptr<BindContext> &bind_context_ptr) {
    std::string expr_name = expr.name ? std::string(expr.name) : std::string();

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
            logical_type = static_cast<int32_t>(expr.ival) == expr.ival ?
                           LogicalType(LogicalTypeId::kInteger): LogicalType(LogicalTypeId::kBigInt);
            return std::make_shared<ValueExpression>(logical_type, expr.ival);
        }
        case hsql::kExprLiteralString: {
            logical_type = LogicalType(LogicalTypeId::kVarchar);
            return std::make_shared<ValueExpression>(logical_type, expr_name);
        }
        case hsql::kExprLiteralNull: {
            logical_type = LogicalType(LogicalTypeId::kNull);
            return std::make_shared<ValueExpression>(logical_type);
        }
        case hsql::kExprColumnRef: {
            // Resolve column
            std::optional<std::string> table_name = nullptr;
            if(expr.table != nullptr) {
                table_name = std::optional<std::string>(std::string(expr.table));
            }
            ColumnIdentifier column_identifier(table_name, expr_name);
            std::shared_ptr<BaseExpression> column_expr = bind_context_ptr->ResolveColumnIdentifier(column_identifier);
            return column_expr;
        }
        default:
            ResponseError("Unsupported expr type");
    }

    return std::shared_ptr<BaseExpression>();
}

}
