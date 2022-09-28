//
// Created by JinHai on 2022/8/12.
//

#include "planner/expression_binder.h"

namespace infinity {

class InsertBinder : public ExpressionBinder {
public:
    explicit InsertBinder(std::shared_ptr<QueryContext>& query_context) : ExpressionBinder(query_context) {}

    // Bind expression entry
    std::shared_ptr<BaseExpression>
    BuildExpression(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr) override;

//    // Bind column reference expression also include correlated column reference.
//    std::shared_ptr<BaseExpression>
//    BuildColRefExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr) override;
private:
};

}
