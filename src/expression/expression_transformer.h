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
std::vector<std::shared_ptr<BaseExpression>>
SplitExpressionByDelimiter(const std::shared_ptr<BaseExpression>& expression, ConjunctionType conjunction_type);

std::shared_ptr<BaseExpression>
ComposeExpressionWithDelimiter(const std::vector<std::shared_ptr<BaseExpression>>& expressions,
                             ConjunctionType conjunction_type);


// Traverse the expression and it's child
void
VisitExpression(const std::shared_ptr<BaseExpression>& expression,
                const std::function<VisitControlType(std::shared_ptr<BaseExpression> &child)>& visitor);

void
VisitExpression(const std::shared_ptr<BaseExpression>& expression,
                const std::function<void(std::shared_ptr<BaseExpression> &child)>& visitor);

}
