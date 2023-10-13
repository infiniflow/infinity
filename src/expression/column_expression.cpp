//
// Created by JinHai on 2022/8/5.
//

#include "column_expression.h"

namespace infinity {

ColumnExpression::ColumnExpression(DataType data_type, String table_name, u64 table_index, String column_name, i64 column_index, i64 depth)
    : BaseExpression(ExpressionType::kColumn, {}), data_type_(std::move(data_type)), table_name_(std::move(table_name)),
      column_name_(std::move(column_name)), binding_(table_index, column_index), depth_(depth) {}

String ColumnExpression::ToString() const {
    if (alias_.empty()) {
        bool all_digits{true};
        for (char a : column_name_) {
            if (!std::isdigit(a)) {
                all_digits = false;
                break;
            }
        }
        if (all_digits) {
            return table_name_ + "." + column_name_;
        } else {
            return column_name_;
        }

    } else {
        return alias_;
    }
}

} // namespace infinity
