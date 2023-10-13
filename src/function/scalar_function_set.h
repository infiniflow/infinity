//
// Created by JinHai on 2022/12/14.
//

#pragma once

#include "function/function_set.h"
#include "function/scalar_function.h"

namespace infinity {

class ScalarFunctionSet final : public FunctionSet {
public:
    explicit ScalarFunctionSet(String name) : FunctionSet(std::move(name), FunctionType::kScalar) {}

    ~ScalarFunctionSet() final;

    void AddFunction(const ScalarFunction &func);

    ScalarFunction GetMostMatchFunction(const Vector<SharedPtr<BaseExpression>> &input_arguments);

private:
    static i64 MatchFunctionCost(const ScalarFunction &func, const Vector<SharedPtr<BaseExpression>> &arguments);

    Vector<ScalarFunction> functions_;
};

} // namespace infinity
