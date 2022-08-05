//
// Created by JinHai on 2022/8/5.
//

#include "cast_expression.h"

#include <sstream>

namespace infinity {

CastExpression::CastExpression(const std::shared_ptr<BaseExpression> &argument,
                               LogicalType data_type)
   : BaseExpression(ExpressionType::kCast, {argument}), data_type_(data_type) {}

std::string
CastExpression::ToString() const {
    std::stringstream ss;

    ss << "Cast(" << arguments_[0]->ToString() << " As " << data_type_.ToString();

    return ss.str();
}

LogicalType
CastExpression::DataType() {
    return data_type_;
}


}
