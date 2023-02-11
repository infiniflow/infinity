//
// Created by JinHai on 2022/8/13.
//

#include "order_binder.h"
#include "function/function_set.h"
#include "parser/statement.h"
#include "parser/expression/parsed_raw_expression.h"
#include "expression/column_expression.h"

namespace infinity {

void
OrderBinder::PushExtraExprToSelectList(const hsql::Expr *expr, const SharedPtr<BindContext>& bind_context_ptr) {
    if(expr->type == hsql::kExprLiteralInt) {
        // Order by 1, means order by 1st select list item.
        return ;
    }

    String expr_name = Statement::ExprAsColumnName(expr);

    if(bind_context_ptr->select_alias2index_.contains(expr_name)) {
        return ;
    }

    if(bind_context_ptr->select_expr_name2index_.contains(expr_name)) {
        return ;
    }

    bind_context_ptr->select_expr_name2index_[expr_name] = bind_context_ptr->select_expression_.size();
    SharedPtr<ParsedRawExpression> raw_expr = MakeShared<ParsedRawExpression>(expr);
    bind_context_ptr->select_expression_.emplace_back(raw_expr);
}

SharedPtr<BaseExpression>
OrderBinder::BuildExpression(const hsql::Expr &expr,
                             const SharedPtr<BindContext>& bind_context_ptr,
                             i64 depth,
                             bool root) {
    i64 column_id = -1;

    // If the expr is from projection, then create a column reference from projection.
    if(expr.type == hsql::kExprLiteralInt) {
        column_id = expr.ival;
        if (column_id >= bind_context_ptr->project_exprs_.size()) {
            PlannerError("Order by are going to use nonexistent column from select list.")
        }
        -- column_id;
    } else {
        String expr_name = Statement::ExprAsColumnName(&expr);

        if(bind_context_ptr->select_alias2index_.contains(expr_name)) {
            column_id = bind_context_ptr->select_alias2index_[expr_name];
        }

        if(bind_context_ptr->select_expr_name2index_.contains(expr_name)) {
            column_id = bind_context_ptr->select_expr_name2index_[expr_name];
        }

        if(column_id == -1) {
            PlannerError(expr_name + " isn't found in project list.")
        }
    }



    const SharedPtr<BaseExpression>& project_expr = bind_context_ptr->project_exprs_[column_id];

    SharedPtr<ColumnExpression> result = ColumnExpression::Make(project_expr->Type(),
                                                                bind_context_ptr->project_table_name_,
                                                                bind_context_ptr->project_table_index_,
                                                                std::to_string(column_id),
                                                                column_id,
                                                                depth);
    result->source_position_ = SourcePosition(bind_context_ptr->binding_context_id_, ExprSourceType::kProjection);
    return result;
}

}
