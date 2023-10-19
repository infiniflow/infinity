//
// Created by JinHai on 2022/8/12.
//

module;

import stl;
import parser;
import base_expression;
import bind_context;
import expression_binder;
import query_context;

export module project_binder;

namespace infinity {

class ProjectBinder final : public ExpressionBinder {
public:
    explicit ProjectBinder(QueryContext *query_context) : ExpressionBinder(query_context) {}

    // Bind expression entry
    SharedPtr<BaseExpression> BuildExpression(const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) final;

    SharedPtr<BaseExpression> BuildFuncExpr(const FunctionExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) final;

    SharedPtr<BaseExpression> BuildColExpr(const ColumnExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) final;

    [[nodiscard]] const String &BoundColumn() const { return bound_column_name_; }

private:
    bool binding_agg_func_ = false;
    String bound_column_name_;
};

} // namespace infinity
