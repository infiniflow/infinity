//
// Created by JinHai on 2022/12/14.
//

#pragma once

#include "function_set.h"
#include "aggregate_function.h"

namespace infinity {

class AggregateFunctionSet: public FunctionSet {
public:
    explicit
    AggregateFunctionSet(String name): FunctionSet(std::move(name),FunctionType::kAggregate) {}

    void
    AddFunction(const AggregateFunction& func);

    AggregateFunction
    GetMostMatchFunction(const SharedPtr<BaseExpression>& input_argument);

private:
    static int64_t
    MatchFunctionCost(const AggregateFunction& func,  const SharedPtr<BaseExpression>& argument);

    Vector<AggregateFunction> functions_;
};

}
