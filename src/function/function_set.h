//
// Created by JinHai on 2022/9/14.
//

#pragma once

#include "common/utility/infinity_assert.h"
#include "expression/base_expression.h"
#include "function.h"
#include "storage/meta/catalog.h"
#include "parser/parsed_expr/function_expr.h"

namespace infinity {

class FunctionSet {
public:
    static SharedPtr<FunctionSet>
    GetFunctionSet(NewCatalog* catalog, const FunctionExpr& expr);

    static String
    ToString(const String& name, const Vector<SharedPtr<BaseExpression>>& arguments);

public:
    explicit
    FunctionSet(String name, FunctionType type): name_(std::move(name)), type_(type) {}

    [[nodiscard]] const String&
    name() const {
        return name_;
    }

    String name_;
    FunctionType type_;
};

}
