//
// Created by JinHai on 2022/8/5.
//

module;

#include <sstream>

import stl;
import expression_type;
import parser;

module correlated_column_expression;

namespace infinity {

CorrelatedColumnExpression::CorrelatedColumnExpression(DataType data_type, String column_name)
    : BaseExpression(ExpressionType::kCorrelatedColumn, {}), data_type_(Move(data_type)), column_name_(Move(column_name)) {}

String CorrelatedColumnExpression::ToString() const { return {}; }

} // namespace infinity
