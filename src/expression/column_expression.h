//
// Created by JinHai on 2022/8/5.
//

#pragma once

#include "base_expression.h"

namespace infinity {

struct ColumnBinding {
    ColumnBinding(std::shared_ptr<std::string> table_name_ptr,
                  std::shared_ptr<std::string> column_name_ptr,
                  uint64_t table_id,
                  uint64_t column_id)
        : table_name_ptr_(table_name_ptr), column_name_ptr_(column_name_ptr), table_id_(table_id), column_id_(column_id)
    {}

    std::shared_ptr<std::string> table_name_ptr_;
    std::shared_ptr<std::string> column_name_ptr_;
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