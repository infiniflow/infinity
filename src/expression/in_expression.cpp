//
// Created by JinHai on 2022/8/4.
//

#include "in_expression.h"
#include "common/utility/infinity_assert.h"

#include <sstream>
#include <utility>

namespace infinity {

InExpression::InExpression(InType in_type, SharedPtr<BaseExpression> left_operand, const Vector<SharedPtr<BaseExpression>> &value_list)
    : BaseExpression(ExpressionType::kIn, value_list), left_operand_ptr_(std::move(left_operand)), in_type_(in_type) {}

std::string InExpression::ToString() const {

    std::stringstream op;

    op << left_operand_ptr_->Name();

    switch (in_type_) {
        case InType::kIn:
            op << " IN ";
            break;
        case InType::kNotIn:
            op << " NOT IN ";
            break;
        default:
            PlannerError("Unknown In operator type.")
    }

    op << "(";

    for (auto &argument_ptr : arguments_) {
        op << argument_ptr->Name() << ", ";
    }

    op << ")" << std::endl;

    return op.str();
}

} // namespace infinity
