//
// Created by JinHai on 2022/9/24.
//

#pragma once

#include "planner/expression_binder.h"

namespace infinity {

class BindAliasProxy {
public:
    explicit
    BindAliasProxy(std::unordered_map<std::string, const hsql::Expr*> alias2expr) : alias2expr_(std::move(alias2expr)) {}

    std::shared_ptr<BaseExpression>
    BindAlias(ExpressionBinder& expression_binder, const hsql::Expr& expr, const std::shared_ptr<BindContext>& bind_context_ptr);
private:
    bool binding_alias_{false};
    std::unordered_map<std::string, const hsql::Expr*> alias2expr_;
};

}



