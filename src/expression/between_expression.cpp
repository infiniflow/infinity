//
// Created by JinHai on 2022/8/5.
//

module;

import std;
import stl;
import expression_type;
import parser;

module between_expression;

namespace infinity {

BetweenExpression::BetweenExpression(const SharedPtr<BaseExpression> &value,
                                     const SharedPtr<BaseExpression> &upper_bound,
                                     const SharedPtr<BaseExpression> &lower_bound,
                                     bool upper_inclusive,
                                     bool lower_inclusive)
    : BaseExpression(ExpressionType::kBetween, {lower_bound, value, upper_bound}), upper_inclusive_(upper_inclusive),
      lower_inclusive_(lower_inclusive) {}

String BetweenExpression::ToString() const {
    std::stringstream ss;

    ss << arguments_[1]->Name() << " Between " << arguments_[0]->Name() << " And " << arguments_[2]->Name();

    return ss.str();
}

DataType BetweenExpression::Type() const { return DataType(LogicalType::kBoolean); }

} // namespace infinity
