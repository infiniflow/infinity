//
// Created by JinHai on 2022/12/14.
//

#pragma once

#include "function_set.h"

namespace infinity {

class ScalarFunctionSet: public FunctionSet {
public:
    explicit ScalarFunctionSet(std::string name): FunctionSet(std::move(name), FunctionType::kScalar) {}

    void AddFunction(const ScalarFunction& func);

    ScalarFunction GetMostMatchFunction(const std::vector<std::shared_ptr<BaseExpression>>& input_arguments);

private:
    int64_t
    MatchFunctionCost(const ScalarFunction& func,  const std::vector<std::shared_ptr<BaseExpression>>& arguments);

    std::vector<ScalarFunction> functions_;
};

}
