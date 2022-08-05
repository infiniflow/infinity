//
// Created by JinHai on 2022/8/5.
//

#pragma once

#include "base_expression.h"

namespace infinity {

struct ColumnBinding {
    std::string table_name_;
    std::string column_name_;
    uint64_t table_id_;
    uint64_t column_id_;
};

class BoundColumnExpression : public BaseExpression {
public:
    BoundColumnExpression(LogicalType logical_type, ColumnBinding column_binding);

    LogicalType DataType() override { return data_type_; };
    std::string ToString() const override;
private:
    ColumnBinding column_binding_;
    LogicalType data_type_;
};


}