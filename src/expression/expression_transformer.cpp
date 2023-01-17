//
// Created by JinHai on 2022/9/14.
//

#include "expression_transformer.h"

namespace infinity {

Vector<SharedPtr<BaseExpression>>
SplitExpressionByDelimiter(const SharedPtr<BaseExpression> &expression, ConjunctionType delimiter) {
    Vector<SharedPtr<BaseExpression>> result;

    std::function<VisitControlType(SharedPtr<BaseExpression> &child)> func
        = [&](const SharedPtr<BaseExpression> &expr_ptr) -> VisitControlType {
        if(expr_ptr->type() == ExpressionType::kConjunction) {
            const auto conjunction_expr_ptr = std::static_pointer_cast<ConjunctionExpression>(expr_ptr);
            if(conjunction_expr_ptr->conjunction_type() == delimiter) return VisitControlType::kVisit;
        }
        result.emplace_back(expr_ptr);
        return VisitControlType::kNotVisit;
    };

    VisitExpression(expression, func);

    return result;
}

SharedPtr<BaseExpression>
ComposeExpressionWithDelimiter(const Vector<SharedPtr<BaseExpression>>& expressions,
                             ConjunctionType conjunction_type) {
    auto expr_count = expressions.size();
    if(expr_count == 0) {
        return nullptr;
    }
    SharedPtr<BaseExpression> result = expressions[0];
    for(auto i = 1; i < expr_count; ++ i) {
        result = MakeShared<ConjunctionExpression>(conjunction_type, result, expressions[i]);
    }
    return result;
}

void
VisitExpression(const SharedPtr<BaseExpression>& expression,
                const std::function<VisitControlType(SharedPtr<BaseExpression> &child)>& visitor) {
    std::queue<SharedPtr<BaseExpression>> queue;
    queue.push(expression);

    while (!queue.empty()) {
        auto expr = queue.front();
        queue.pop();

        if(visitor(expr) == VisitControlType::kVisit) {
            for(auto& argument: expr->arguments()) {
                queue.push(argument);
            }
        }
    }
}

void
VisitExpression(const SharedPtr<BaseExpression>& expression,
                const std::function<void(SharedPtr<BaseExpression> &child)>& visitor) {
    std::queue<SharedPtr<BaseExpression>> queue;
    queue.push(expression);

    while (!queue.empty()) {
        auto expr = queue.front();
        queue.pop();
        visitor(expr);
        // TODO: Different expression will have different arguments, such as CaseExpression. following part need to be
        // refactored.
        for(auto& argument: expr->arguments()) {
            queue.push(argument);
        }
    }
}

}