//
// Created by JinHai on 2022/8/11.
//

#pragma once

#include "expression/base_expression.h"
#include "bind_context.h"

#include <memory>

namespace infinity {

class ExpressionBinder {
public:
    virtual std::shared_ptr<BaseExpression>
    BuildExpression(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr);
private:
};

}
