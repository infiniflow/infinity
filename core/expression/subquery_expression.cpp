//
// Created by JinHai on 2022/9/19.
//

module;

#include <sstream>

import stl;
import bound_select_statement;
import expression_type;
import parser;

module subquery_expression;

namespace infinity {

SubqueryExpression::SubqueryExpression(SharedPtr<BoundSelectStatement> bound_select_statement, SubqueryType subquery_type)
    : BaseExpression(ExpressionType::kSubQuery, {}), bound_select_statement_ptr_(Move(bound_select_statement)), subquery_type_(subquery_type) {}

String SubqueryExpression::ToString() const {
    std::stringstream ss;
    ss << "Substring: " << std::endl;
    return ss.str();
}

DataType SubqueryExpression::Type() const {
    switch (subquery_type_) {
        case SubqueryType::kIn:
        case SubqueryType::kNotExists:
        case SubqueryType::kExists: {
            return DataType(LogicalType::kBoolean);
        }
        default: {
            return DataType(LogicalType::kBoolean);
        }
    }
}

} // namespace infinity
