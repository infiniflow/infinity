//
// Created by JinHai on 2022/9/24.
//

#pragma once

#include "planner/expression_binder.h"
#include "parser/parsed_expression.h"

namespace infinity {

class BindAliasProxy {
public:
    explicit
    BindAliasProxy(HashMap<String, SharedPtr<ParsedExpression>> alias2expr) : alias2expr_(std::move(alias2expr)) {}

    SharedPtr<BaseExpression>
    BindAlias(ExpressionBinder& expression_binder, const hsql::Expr& expr, const SharedPtr<BindContext>& bind_context_ptr);
private:
    bool binding_alias_{false};
    HashMap<String, SharedPtr<ParsedExpression>> alias2expr_;
};

}



