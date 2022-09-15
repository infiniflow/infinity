//
// Created by JinHai on 2022/9/14.
//

#pragma once

#include "common/utility/infinity_assert.h"
#include "aggregate_function.h"
#include "scalar_function.h"
#include "table_function.h"

namespace infinity {


class FunctionSet {
public:
    explicit FunctionSet(std::string name, FunctionType type): name_(std::move(name)), type_(type) {}

    [[nodiscard]] const std::string& name() const { return name_; }

    std::string name_;
    FunctionType type_;
};

class AggregateFunctionSet: public FunctionSet {
public:
    explicit AggregateFunctionSet(std::string name): FunctionSet(std::move(name), FunctionType::kAggregate) {}

    void AddFunction(const AggregateFunction& func);

    AggregateFunction GetMostMatchFunction(const std::vector<BaseExpression>& input_arguments);

private:
    std::vector<AggregateFunction> functions_;
};


class ScalarFunctionSet: public FunctionSet {
public:
    explicit ScalarFunctionSet(std::string name): FunctionSet(std::move(name), FunctionType::kScalar) {}

    void AddFunction(const ScalarFunction& func);

    ScalarFunction GetMostMatchFunction(const std::vector<BaseExpression>& input_arguments);

private:
    std::vector<ScalarFunction> functions_;
};

class TableFunctionSet: public FunctionSet {
public:
    explicit TableFunctionSet(std::string name): FunctionSet(std::move(name), FunctionType::kTable) {}

    void AddFunction(const TableFunction& func);

private:
    std::vector<TableFunction> functions_;
};

}
