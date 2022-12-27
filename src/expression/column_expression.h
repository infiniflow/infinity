//
// Created by JinHai on 2022/8/5.
//

#pragma once

#include "base_expression.h"

namespace infinity {


class ColumnExpression : public BaseExpression {
public:
    ColumnExpression(DataType data_type, String table_name,
                     String column_name, i64 column_index, i64 depth);

    DataType Type() const override {
        return data_type_;
    };

    String
    ToString() const override;

    const String&
    table_name() const {
        return table_name_;
    }

    const String&
    column_name() const {
        return column_name_;
    }

    i64
    column_index() const {
        return column_index_;
    }

private:

    DataType data_type_;
    String table_name_;
    String column_name_;

    i64 column_index_;
    i64 depth_;
};


}