//
// Created by JinHai on 2022/8/5.
//

#pragma once

#include "base_expression.h"

namespace infinity {


class ColumnExpression : public BaseExpression {
public:
    ColumnExpression(LogicalType logical_type, std::string table_name, int64_t table_index,
                     std::string column_name, int64_t column_index, int64_t depth);

    LogicalType DataType() override { return data_type_; };
    std::string ToString() const override;
private:
    LogicalType data_type_;
    std::string table_name_;
    std::string column_name_;

    int64_t table_index_;
    int64_t column_index_;
    int64_t depth_;
};


}