//
// Created by JinHai on 2022/9/14.
//

module;

import stl;
import base_expression;
import conjunction_expression;

export module expression_transformer;

namespace infinity {

enum class VisitControlType { kInvalid, kVisit, kNotVisit };

// Transform expr_a AND expr_b AND expr_c into expressions array: [expr_a, expr_b, expr_c].
Vector<SharedPtr<BaseExpression>> SplitExpressionByDelimiter(const SharedPtr<BaseExpression> &expression, ConjunctionType conjunction_type);

export SharedPtr<BaseExpression> ComposeExpressionWithDelimiter(const Vector<SharedPtr<BaseExpression>> &expressions, ConjunctionType conjunction_type);

// Traverse the expression and it's child
void VisitExpression(const SharedPtr<BaseExpression> &expression, const StdFunction<VisitControlType(SharedPtr<BaseExpression> &child)> &visitor);

export void VisitExpression(const SharedPtr<BaseExpression> &expression, const StdFunction<void(SharedPtr<BaseExpression> &child)> &visitor);

} // namespace infinity
