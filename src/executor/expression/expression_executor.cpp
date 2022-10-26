//
// Created by JinHai on 2022/10/26.
//

#include "expression_executor.h"

namespace infinity {

void
ExpressionExecutor::Init(const std::vector<std::shared_ptr<BaseExpression>> &exprs) {
    for(auto& expr_ptr: exprs) {
        expressions.emplace_back(expr_ptr);
        states.emplace_back(ExpressionState::CreateState(expr_ptr));
    }
}

}
