//
// Created by JinHai on 2022/9/14.
//

#pragma once

#include "common/utility/infinity_assert.h"
#include "aggregate_function.h"
#include "scalar_function.h"
#include "table_function.h"

namespace infinity {

template<typename Func>
class FunctionSet {
public:
    explicit FunctionSet(std::string name): name_(name) {}

    void AddFunction(Func function) {
        FunctionAssert(function.name == name_, "Mismatch the function name: " + function.name + " with: " + name_);
        functions_.template emplace_back(function);
    }

    std::string name_;

    std::vector<Func> functions_;
};

class ScalarFunctionSet: public FunctionSet<ScalarFunction> {
public:
    explicit ScalarFunctionSet(std::string name): FunctionSet(std::move(name)) {}
};

class AggregateFunctionSet: public FunctionSet<AggregateFunction> {
public:
    explicit AggregateFunctionSet(std::string name): FunctionSet(std::move(name)) {}
};

class TableFunctionSet: public FunctionSet<TableFunction> {
public:
    explicit TableFunctionSet(std::string name): FunctionSet(std::move(name)) {}
};

}
