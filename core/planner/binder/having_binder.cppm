//
// Created by JinHai on 2022/8/11.
//

module;

import stl;
import parser;
import base_expression;
import bind_context;
import expression_binder;
import query_context;
import bind_alias_proxy;

export module having_binder;

namespace infinity {

export class HavingBinder final : public ExpressionBinder {
public:
    explicit HavingBinder(QueryContext *query_context, const SharedPtr<BindAliasProxy> &bind_alias_proxy)
        : ExpressionBinder(query_context), bind_alias_proxy_(bind_alias_proxy) {}

    // Bind expression entry
    SharedPtr<BaseExpression> BuildExpression(const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) final;

    SharedPtr<BaseExpression> BuildColExpr(const ColumnExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) final;

    SharedPtr<BaseExpression> BuildFuncExpr(const FunctionExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) final;

    SharedPtr<BaseExpression> BuildKnnExpr(const KnnExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) override;
private:
    const SharedPtr<BindAliasProxy> &bind_alias_proxy_;
    bool binding_agg_func_ = false;
};

} // namespace infinity
