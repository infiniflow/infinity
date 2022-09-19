//
// Created by JinHai on 2022/9/19.
//

#include "subquery_expression.h"
#include <sstream>

#include <utility>

namespace infinity {

SubqueryExpression::SubqueryExpression(std::shared_ptr<BoundSelectNode> select_node, SubqueryType subquery_type)
    : BaseExpression(ExpressionType::kSubQuery, {}),
    select_node_ptr_(std::move(select_node)),
    subquery_type_(subquery_type)
    {}

std::string
SubqueryExpression::ToString() const {
    std::stringstream ss;
    ss << "Substring: " << std::endl;
    return ss.str();
}

LogicalType
SubqueryExpression::DataType() {
    switch(subquery_type_) {
        case SubqueryType::kIn:
        case SubqueryType::kNotExists:
        case SubqueryType::kExists: {
            return LogicalType(LogicalTypeId::kBoolean);
        }
        default: {
            return LogicalType(LogicalTypeId::kBoolean);
        }
    }

}

}

