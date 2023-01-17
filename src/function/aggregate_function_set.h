//
// Created by JinHai on 2022/12/14.
//

#include "function_set.h"

#pragma once

namespace infinity {

class AggregateFunctionSet: public FunctionSet {
public:
    explicit AggregateFunctionSet(std::string name): FunctionSet(std::move(name), FunctionType::kAggregate) {}

    void AddFunction(const AggregateFunction& func);

    AggregateFunction GetMostMatchFunction(const std::vector<SharedPtr<BaseExpression>>& input_arguments);

private:
    std::vector<AggregateFunction> functions_;
};

}
