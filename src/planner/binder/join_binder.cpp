//
// Created by JinHai on 2022/9/14.
//

#include "join_binder.h"
#include "common/utility/infinity_assert.h"


namespace infinity {

SharedPtr<BaseExpression>
JoinBinder::BuildExpression(const hsql::Expr &expr, const SharedPtr<BindContext> &bind_context_ptr) {
    SharedPtr<BaseExpression> result;
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
