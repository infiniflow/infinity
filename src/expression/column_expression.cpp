//
// Created by JinHai on 2022/8/5.
//

#include "column_expression.h"

#include <utility>
#include <sstream>

namespace infinity {

ColumnExpression::ColumnExpression(DataType data_type,
                                   String table_name,
                                   u64 table_index,
                                   String column_name,
                                   i64 column_index,
                                   i64 depth)
                                   : BaseExpression(ExpressionType::kColumn, {}),
                                   data_type_(std::move(data_type)),
                                   table_name_(std::move(table_name)),
                                   column_name_(std::move(column_name)),
                                   binding_(table_index, column_index),
                                   depth_(depth)
{}

String
ColumnExpression::ToString() const {
    std::stringstream ss;
    if(!table_name_.empty()) {
        ss << table_name_ << '.';
    }
    ss << column_name_;
    return ss.str();
}

}
