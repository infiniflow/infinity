//
// Created by JinHai on 2022/9/14.
//

#pragma once

#include "common/utility/infinity_assert.h"
#include "aggregate_function.h"
#include "scalar_function.h"
#include "table_function.h"

#include "sql/Expr.h"

namespace infinity {


class FunctionSet {
public:
    static std::shared_ptr<FunctionSet> GetFunctionSet(const hsql::Expr &expr);
    static std::string
    ToString(const std::string& name, const std::vector<std::shared_ptr<BaseExpression>>& arguments);

    explicit FunctionSet(std::string name, FunctionType type): name_(std::move(name)), type_(type) {}

    [[nodiscard]] const std::string& name() const { return name_; }

    std::string name_;
    FunctionType type_;
};

class AggregateFunctionSet: public FunctionSet {
public:
    explicit AggregateFunctionSet(std::string name): FunctionSet(std::move(name), FunctionType::kAggregate) {}

    void AddFunction(const AggregateFunction& func);

    AggregateFunction GetMostMatchFunction(const std::vector<std::shared_ptr<BaseExpression>>& input_arguments);

private:
    std::vector<AggregateFunction> functions_;
};


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

class TableFunctionSet: public FunctionSet {
public:
    explicit TableFunctionSet(std::string name): FunctionSet(std::move(name), FunctionType::kTable) {}

    void AddFunction(const TableFunction& func);

private:
    std::vector<TableFunction> functions_;
};

}
