//
// Created by JinHai on 2022/12/14.
//

#include "aggregate_function_set.h"

namespace infinity {

void
AggregateFunctionSet::AddFunction(const infinity::AggregateFunction& func) {
    functions_.emplace_back(func);
}

AggregateFunction
AggregateFunctionSet::GetMostMatchFunction(const std::vector<SharedPtr<BaseExpression>> &input_arguments) {
    PlannerError("not implemented.");
}

}

