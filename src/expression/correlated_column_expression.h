//
// Created by JinHai on 2022/8/5.
//

#pragma once

#include "base_expression.h"

namespace infinity {

class CorrelatedColumnExpression: public BaseExpression {
public:
    CorrelatedColumnExpression(DataType data_type, String column_name);

    DataType
    Type() const override {
        return data_type_;
    }

    String
    ToString() const override;

private:
    DataType data_type_;
    String column_name_;
};


}