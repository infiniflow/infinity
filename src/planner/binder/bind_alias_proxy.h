//
// Created by JinHai on 2022/9/24.
//

#pragma once

#include "planner/expression_binder.h"

namespace infinity {

class BindAliasProxy {
public:
    SharedPtr<BaseExpression>
    BindAlias(ExpressionBinder &expression_binder, const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root);

private:
    bool binding_alias_{false};
};

} // namespace infinity
