//
// Created by JinHai on 2022/9/15.
//

#include "function_set.h"
#include "main/infinity.h"

namespace infinity {

std::shared_ptr<FunctionSet>
FunctionSet::GetFunctionSet(const hsql::Expr &expr) {
    std::string function_name = expr.name;

    // Transfer the function to upper case.
    std::transform(function_name.begin(), function_name.end(), function_name.begin(), [](const auto c) {
        return std::toupper(c);
    });

    // std::unique_ptr<Catalog>& catalog
    auto& catalog = Infinity::instance().catalog();
    std::shared_ptr<FunctionSet> function_set_ptr = catalog->GetFunctionSetByName(function_name);

    return function_set_ptr;
}

void
AggregateFunctionSet::AddFunction(const infinity::AggregateFunction& func) {
    functions_.emplace_back(func);
}

AggregateFunction
AggregateFunctionSet::GetMostMatchFunction(const std::vector<std::shared_ptr<BaseExpression>> &input_arguments) {
    PlannerError("not implemented.");
}

void
ScalarFunctionSet::AddFunction(const infinity::ScalarFunction& func) {
    functions_.emplace_back(func);
}

ScalarFunction
ScalarFunctionSet::GetMostMatchFunction(const std::vector<std::shared_ptr<BaseExpression>> &input_arguments) {
    PlannerError("not implemented.");
}

void
TableFunctionSet::AddFunction(const infinity::TableFunction& func) {
    functions_.emplace_back(func);
}

}

