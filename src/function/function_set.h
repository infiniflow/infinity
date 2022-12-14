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







}
