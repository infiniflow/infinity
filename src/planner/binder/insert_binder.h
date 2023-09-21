//
// Created by JinHai on 2022/8/12.
//

#include "planner/expression_binder.h"

namespace infinity {

class InsertBinder final : public ExpressionBinder {
public:
    explicit
    InsertBinder(QueryContext* query_context) : ExpressionBinder(query_context) {}

    // Bind expression entry
    SharedPtr<BaseExpression>
    BuildExpression(const ParsedExpr &expr,
                    BindContext* bind_context_ptr,
                    i64 depth,
                    bool root) final;

    SharedPtr<BaseExpression>
    BuildKnnExpr(const KnnExpr& expr,
                 BindContext* bind_context_ptr,
                 i64 depth,
                 bool root) override;

//    // Bind column reference expression also include correlated column reference.
//    SharedPtr<BaseExpression>
//    BuildColRefExpr(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr) override;
private:
};

}
