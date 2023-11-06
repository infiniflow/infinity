//
// Created by JinHai on 2022/8/5.
//

module;

#include <cctype>

import stl;
import parser;
import expression_type;
import third_party;

module column_expression;

namespace infinity {

ColumnExpression::ColumnExpression(DataType data_type, String table_name, u64 table_index, String column_name, i64 column_index, i64 depth)
    : BaseExpression(ExpressionType::kColumn, {}), data_type_(Move(data_type)), table_name_(Move(table_name)), column_name_(Move(column_name)),
      binding_(table_index, column_index), depth_(depth) {}

String ColumnExpression::ToString() const {
    if (alias_.empty()) {
        bool all_digits{true};
        SizeT column_count = column_name_.size();
        for (SizeT idx = 0; idx < column_count; ++idx) {
            char a = column_name_[idx];
            if (!std::isdigit(a)) {
                all_digits = false;
                break;
            }
        }
        if (all_digits) {
            return Format("{}.{}", table_name_, column_name_);
        } else {
            return column_name_;
        }

    } else {
        return alias_;
    }
}

} // namespace infinity
