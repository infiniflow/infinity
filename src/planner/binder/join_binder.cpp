//
// Created by JinHai on 2022/9/14.
//

#include "join_binder.h"
#include "common/utility/infinity_assert.h"


namespace infinity {

std::shared_ptr<BaseExpression>
JoinBinder::BuildExpression(const hsql::Expr &expr, const std::shared_ptr<BindContext> &bind_context_ptr) {
    std::shared_ptr<BaseExpression> result;
    switch(expr.type) {
        case hsql::ExprType::kExprSelect: {
            PlannerError("Subquery isn't allowed in JOIN condition.");
        }
        default: {
            result = ExpressionBinder::BuildExpression(expr, bind_context_ptr);
        }
    }
    return result;
}

}
