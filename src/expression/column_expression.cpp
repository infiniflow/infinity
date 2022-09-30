//
// Created by JinHai on 2022/8/5.
//

#include "column_expression.h"

#include <utility>
#include <sstream>

namespace infinity {

ColumnExpression::ColumnExpression(LogicalType logical_type, std::string table_name,
                                   std::string column_name, int64_t column_index, int64_t depth)
                                   : BaseExpression(ExpressionType::kColumn, {}),
                                   data_type_(logical_type),
                                   table_name_(std::move(table_name)),
                                   column_name_(std::move(column_name)),
                                   column_index_(column_index),
                                   depth_(depth)
{}

std::string
ColumnExpression::ToString() const {
    std::stringstream ss;
    if(!table_name_.empty()) {
        ss << table_name_ << '.';
    }
    ss << column_name_;
    return ss.str();
}

}
