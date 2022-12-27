//
// Created by JinHai on 2022/8/5.
//

#include "conjunction_expression.h"

namespace infinity {

ConjunctionExpression::ConjunctionExpression(ConjunctionType type,
                                             const SharedPtr<BaseExpression> &left_operand,
                                             const SharedPtr<BaseExpression> &right_operand)
     : BaseExpression(ExpressionType::kConjunction, {left_operand, right_operand}), conjunction_type_(type) {}

String
ConjunctionExpression::ToString() const {
    return {};
}

}
