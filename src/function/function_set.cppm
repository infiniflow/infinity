//
// Created by JinHai on 2022/9/14.
//

module;

import stl;

import parser;
import function;
import base_expression;
//import new_catalog;

export module function_set;

namespace infinity {

class NewCatalog;
//class FunctionExpr;
//class BaseExpression;

export class FunctionSet {
public:
    static SharedPtr<FunctionSet> GetFunctionSet(NewCatalog *catalog, const FunctionExpr &expr);

    static String ToString(const String &name, const Vector<SharedPtr<BaseExpression>> &arguments);

public:
    explicit FunctionSet(String name, FunctionType type) : name_(Move(name)), type_(type) {}

    virtual ~FunctionSet() = default;

    [[nodiscard]] const String &name() const { return name_; }

    String name_;
    FunctionType type_;
};

} // namespace infinity
