//
// Created by JinHai on 2022/8/5.
//

#include "column_expression.h"

#include <utility>

namespace infinity {

ColumnExpression::ColumnExpression(LogicalType logical_type, std::string table_name,
                                   int64_t table_index, std::string column_name,
                                   int64_t column_index)
                                   : BaseExpression(ExpressionType::kColumn, {}),
                                   data_type_(logical_type),
                                   table_name_(std::move(table_name)),
                                   table_index_(table_index),
                                   column_name_(std::move(column_name)),
                                   column_index_(column_index)
{}

std::string
ColumnExpression::ToString() const {
    return column_name_;
}

}
