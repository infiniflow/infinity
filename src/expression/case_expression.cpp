//
// Created by JinHai on 2022/8/5.
//

#include "case_expression.h"

#include <sstream>

namespace infinity {

CaseExpression::CaseExpression(const std::shared_ptr<BaseExpression> &when,
                               const std::shared_ptr<BaseExpression> &then,
                               const std::shared_ptr<BaseExpression> &otherwise)
   : BaseExpression(ExpressionType::kCase, {when, then, otherwise}) {}

std::string
CaseExpression::ToString() const {
    std::stringstream ss;

    ss << "Case When " << arguments_[0]->ToString() <<
       " Then " << arguments_[1]->ToString() << " Else " << arguments_[2]->ToString() << " End";

    return ss.str();
}

LogicalType
CaseExpression::DataType() {
    // expression_common_type(then()->data_type(), otherwise()->data_type());
    return LogicalType(LogicalTypeId::kBigInt);
}

}
