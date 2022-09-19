//
// Created by JinHai on 2022/8/4.
//

#include "in_expression.h"
#include "common/utility/infinity_assert.h"

#include <sstream>
#include <utility>

namespace infinity {


InExpression::InExpression(InType in_type,
                           std::shared_ptr<BaseExpression> left_operand,
                           const std::vector<std::shared_ptr<BaseExpression>>& value_list)
   : BaseExpression(ExpressionType::kArithmetic, value_list),
   left_operand_ptr_(std::move(left_operand)),
   in_type_(in_type) {}

std::string
InExpression::ToString() const {

    std::stringstream op;

    op << left_operand_ptr_->ToString();

    switch(in_type_) {
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

    for(auto& argument_ptr: arguments_) {
        op << argument_ptr->ToString() << ", ";
    }

    op << ")" << std::endl;

    return  op.str();
}

LogicalType
InExpression::DataType() {
    return LogicalType(LogicalTypeId::kBoolean);
}


}
