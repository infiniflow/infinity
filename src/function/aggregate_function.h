//
// Created by JinHai on 2022/9/14.
//

#pragma once

#include "storage/data_type.h"
#include "expression/base_expression.h"
#include "function.h"
#include <vector>

namespace infinity {

class AggregateFunction: public Function {
public:
    explicit AggregateFunction(std::string name,
                               std::vector<LogicalType> argument_types,
                               LogicalType return_type);

    void CastArgumentTypes(std::vector<BaseExpression>& input_arguments);
    [[nodiscard]] const LogicalType& return_type() const { return return_type_; }

    [[nodiscard]] std::string
    ToString() const override;

private:
    std::vector<LogicalType> parameter_types_;
    LogicalType return_type_;

};
}

