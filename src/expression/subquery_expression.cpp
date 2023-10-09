//
// Created by JinHai on 2022/9/19.
//

#include "subquery_expression.h"
#include <sstream>

#include <utility>

namespace infinity {

SubqueryExpression::SubqueryExpression(SharedPtr<BoundSelectStatement> bound_select_statement, SubqueryType subquery_type)
    : BaseExpression(ExpressionType::kSubQuery, {}), bound_select_statement_ptr_(std::move(bound_select_statement)), subquery_type_(subquery_type) {}

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
