//
// Created by JinHai on 2022/9/14.
//

#pragma once

#include "base_expression.h"
#include "conjunction_expression.h"

#include <queue>
#include <functional>

namespace infinity {

enum class VisitControlType {
    kInvalid,
    kVisit,
    kNotVisit
};

// Transform expr_a AND expr_b AND expr_c into expressions array: [expr_a, expr_b, expr_c].
Vector<SharedPtr<BaseExpression>>
SplitExpressionByDelimiter(const SharedPtr<BaseExpression>& expression, ConjunctionType conjunction_type);

SharedPtr<BaseExpression>
ComposeExpressionWithDelimiter(const Vector<SharedPtr<BaseExpression>>& expressions,
                             ConjunctionType conjunction_type);


// Traverse the expression and it's child
void
VisitExpression(const SharedPtr<BaseExpression>& expression,
                const std::function<VisitControlType(SharedPtr<BaseExpression> &child)>& visitor);

void
VisitExpression(const SharedPtr<BaseExpression>& expression,
                const std::function<void(SharedPtr<BaseExpression> &child)>& visitor);

}
