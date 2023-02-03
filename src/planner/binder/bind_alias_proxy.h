//
// Created by JinHai on 2022/9/24.
//

#pragma once

#include "planner/expression_binder.h"
#include "parser/parsed_expression.h"

namespace infinity {

class BindAliasProxy {
public:
    SharedPtr<BaseExpression>
    BindAlias(ExpressionBinder& expression_binder,
              const hsql::Expr& expr,
              const SharedPtr<BindContext>& bind_context_ptr,
              i64 depth,
              bool root);
private:
    bool binding_alias_{false};
};

}



