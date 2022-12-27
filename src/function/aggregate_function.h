//
// Created by JinHai on 2022/9/14.
//

#pragma once

#include "common/types/data_type.h"
#include "expression/base_expression.h"
#include "function.h"
#include <vector>

namespace infinity {

class AggregateFunction: public Function {
public:
    explicit AggregateFunction(String name,
                               Vector<DataType> argument_types,
                               DataType return_type);

    void
    CastArgumentTypes(std::vector<BaseExpression>& input_arguments);

    [[nodiscard]] const DataType&
    return_type() const {
        return return_type_;
    }

    [[nodiscard]] std::string
    ToString() const override;

private:
    Vector<DataType> parameter_types_;
    DataType return_type_;

};
}

