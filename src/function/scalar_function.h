//
// Created by JinHai on 2022/9/14.
//

#pragma once

#include "function.h"
#include "storage/data_type.h"
#include <vector>
#include "expression/base_expression.h"

namespace infinity {

class ScalarFunction : public Function {
public:
    explicit ScalarFunction(std::string name,
                            std::vector<LogicalType> argument_types,
                            LogicalType return_type);

    void CastArgumentTypes(std::vector<BaseExpression>& input_arguments);

private:
    std::vector<LogicalType> argument_types_;
    LogicalType return_type_;

};
}

