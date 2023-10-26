//
// Created by JinHai on 2022/8/12.
//

module;

import stl;
import base_expression;
import parser;
import function;
import bind_context;
import expression_binder;
import query_context;
import bind_alias_proxy;

export module where_binder;

namespace infinity {

export class WhereBinder final : public ExpressionBinder {
public:
    explicit WhereBinder(QueryContext *query_context, const SharedPtr<BindAliasProxy> &bind_alias_proxy)
        : ExpressionBinder(query_context), bind_alias_proxy_(bind_alias_proxy) {}

    // Bind expression entry
    SharedPtr<BaseExpression> BuildExpression(const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) final;

    SharedPtr<BaseExpression> BuildColExpr(const ColumnExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) final;

    void CheckFuncType(FunctionType func_type) const override;

private:
    const SharedPtr<BindAliasProxy> &bind_alias_proxy_;
};

} // namespace infinity
