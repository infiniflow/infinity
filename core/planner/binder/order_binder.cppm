//
// Created by JinHai on 2022/8/13.
//

module;

import stl;
import parser;
import base_expression;
import bind_context;
import expression_binder;
import query_context;

export module order_binder;


namespace infinity {

class OrderBinder : public ExpressionBinder {
public:
    static void PushExtraExprToSelectList(ParsedExpr *expr, const SharedPtr<BindContext> &bind_context_ptr);

public:
    explicit OrderBinder(QueryContext *query_context) : ExpressionBinder(query_context) {}

    // Bind expression entry
    SharedPtr<BaseExpression> BuildExpression(const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) override;

private:
};

} // namespace infinity
