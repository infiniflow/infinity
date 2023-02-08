//
// Created by JinHai on 2022/12/14.
//

#pragma once

#include "function_set.h"

namespace infinity {

class ScalarFunctionSet: public FunctionSet {
public:
    explicit
    ScalarFunctionSet(String name): FunctionSet(std::move(name), FunctionType::kScalar) {}

    void
    AddFunction(const ScalarFunction& func);

    ScalarFunction
    GetMostMatchFunction(const Vector<SharedPtr<BaseExpression>>& input_arguments);

private:
    static int64_t
    MatchFunctionCost(const ScalarFunction& func,  const Vector<SharedPtr<BaseExpression>>& arguments);

    Vector<ScalarFunction> functions_;
};

}
