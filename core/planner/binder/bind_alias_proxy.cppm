//
// Created by JinHai on 2022/9/24.
//

module;

import stl;
import base_expression;
import parser;
import bind_context;
import expression_binder;

export module bind_alias_proxy;

namespace infinity {

export class BindAliasProxy {
public:
    SharedPtr<BaseExpression>
    BindAlias(ExpressionBinder &expression_binder, const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root);

private:
    bool binding_alias_{false};
};

} // namespace infinity
