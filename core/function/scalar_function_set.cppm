//
// Created by JinHai on 2022/12/14.
//

module;

import stl;
import base_expression;
import scalar_function;
import function;
import function_set;

export module scalar_function_set;

namespace infinity {

export class ScalarFunctionSet final : public FunctionSet {
public:
    explicit ScalarFunctionSet(String name) : FunctionSet(Move(name), FunctionType::kScalar) {}

    ~ScalarFunctionSet() final;

    void AddFunction(const ScalarFunction &func);

    ScalarFunction GetMostMatchFunction(const Vector<SharedPtr<BaseExpression>> &input_arguments);

private:
    static i64 MatchFunctionCost(const ScalarFunction &func, const Vector<SharedPtr<BaseExpression>> &arguments);

    Vector<ScalarFunction> functions_;
};

} // namespace infinity
