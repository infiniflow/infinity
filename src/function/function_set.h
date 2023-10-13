//
// Created by JinHai on 2022/9/14.
//

#pragma once

//#include "common/utility/infinity_assert.h"
//#include "expression/base_expression.h"
#include "function.h"
//#include "parser/parsed_expr/function_expr.h"
//#include "storage/meta/catalog.h"

#include "common/types/alias/smart_ptr.h"
#include "common/types/alias/strings.h"
#include "common/types/alias/containers.h"

namespace infinity {

class NewCatalog;
class FunctionExpr;
class BaseExpression;

class FunctionSet {
public:
    static SharedPtr<FunctionSet> GetFunctionSet(NewCatalog *catalog, const FunctionExpr &expr);

    static String ToString(const String &name, const Vector<SharedPtr<BaseExpression>> &arguments);

public:
    explicit FunctionSet(String name, FunctionType type) : name_(std::move(name)), type_(type) {}

    virtual ~FunctionSet() = default;

    [[nodiscard]] const String &name() const { return name_; }

    String name_;
    FunctionType type_;
};

} // namespace infinity
