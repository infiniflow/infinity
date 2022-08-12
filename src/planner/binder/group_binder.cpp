//
// Created by JinHai on 2022/8/11.
//

#include "group_binder.h"

namespace infinity {

std::shared_ptr<BaseExpression>
GroupBinder::BuildExpression(const hsql::Expr &expr, const std::shared_ptr<BindContext> &bind_context_ptr) {
    std::shared_ptr<BaseExpression> result = ExpressionBinder::BuildExpression(expr, bind_context_ptr);
    return result;
}

}
