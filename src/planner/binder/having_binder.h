//
// Created by JinHai on 2022/8/11.
//

#pragma once

#include "planner/expression_binder.h"

namespace infinity {

class HavingBinder : public ExpressionBinder {
public:
    std::shared_ptr<BaseExpression>
    BuildExpression(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr) override;
private:
};

}
