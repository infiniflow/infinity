//
// Created by JinHai on 2022/8/4.
//

module;

#include <sstream>


import infinity_assert;
import infinity_exception;
import stl;
import expression_type;

module in_expression;

namespace infinity {

InExpression::InExpression(InType in_type, SharedPtr<BaseExpression> left_operand, const Vector<SharedPtr<BaseExpression>> &value_list)
    : BaseExpression(ExpressionType::kIn, value_list), left_operand_ptr_(Move(left_operand)), in_type_(in_type) {}

String InExpression::ToString() const {

    std::stringstream op;

    op << left_operand_ptr_->Name();

    switch (in_type_) {
        case InType::kIn: {
            op << " IN ";
            break;
        }
        case InType::kNotIn: {
            op << " NOT IN ";
            break;
        }
        default: {
            Error<PlannerException>("Unknown In operator type.", __FILE_NAME__, __LINE__);
        }
    }

    op << "(";

    for (auto &argument_ptr : arguments_) {
        op << argument_ptr->Name() << ", ";
    }

    op << ")" << std::endl;

    return op.str();
}

} // namespace infinity
