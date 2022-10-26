//
// Created by JinHai on 2022/10/26.
//

#pragma once

#include "expression/base_expression.h"
#include "expression_state.h"

namespace infinity {

class ExpressionExecutor {
public:
    ExpressionExecutor() = default;

    void
    Init(const std::vector<std::shared_ptr<BaseExpression>>& expressions);

    std::vector<std::shared_ptr<BaseExpression>> expressions;
    std::vector<std::shared_ptr<ExpressionState>> states;
};

}
