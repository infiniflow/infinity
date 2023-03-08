//
// Created by JinHai on 2022/9/14.
//

#include "join_binder.h"
#include "common/utility/infinity_assert.h"


namespace infinity {

SharedPtr<BaseExpression>
JoinBinder::BuildExpression(const ParsedExpr& expr,
                            BindContext* bind_context_ptr,
                            i64 depth,
                            bool root) {
    SharedPtr<BaseExpression> result;
    switch(expr.type_) {
        case ParsedExprType::kSubquery: {
            PlannerError("Subquery isn't allowed in JOIN condition.");
        }
        default: {
            result = ExpressionBinder::BuildExpression(expr, bind_context_ptr, depth, root);
        }
    }
    return result;
}

}
