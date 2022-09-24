//
// Created by JinHai on 2022/8/12.
//

#include "where_binder.h"

namespace infinity {

std::shared_ptr<BaseExpression>
WhereBinder::BuildExpression(const hsql::Expr &expr, const std::shared_ptr<BindContext> &bind_context_ptr) {
    std::shared_ptr<BaseExpression> result = ExpressionBinder::BuildExpression(expr, bind_context_ptr);
    return result;
}

std::shared_ptr<BaseExpression>
WhereBinder::BuildColExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr) {
    // Check if the column is using an alias from select list.
    auto result = bind_alias_proxy_->BindAlias(*this, expr, bind_context_ptr);

    if(result == nullptr) {
        result = ExpressionBinder::BuildColExpr(expr, bind_context_ptr);
    }

    return result;
}

}