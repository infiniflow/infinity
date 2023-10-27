//
// Created by JinHai on 2022/9/15.
//

module;

#include <sstream>

import stl;
import new_catalog;
import parser;
import base_expression;

module function_set;

namespace infinity {

SharedPtr<FunctionSet> FunctionSet::GetFunctionSet(NewCatalog *catalog, const FunctionExpr &expr) {
    String function_name = expr.func_name_;

    // SharedPtr<NewCatalog>& catalog
    SharedPtr<FunctionSet> function_set_ptr = NewCatalog::GetFunctionSetByName(catalog, function_name);

    return function_set_ptr;
}

String FunctionSet::ToString(const String &name, const Vector<SharedPtr<BaseExpression>> &arguments) {
    std::stringstream ss;

    ss << name;
    auto argument_count = arguments.size();
    if (argument_count == 0) {
        ss << "()";

    } else {
        ss << "(";
        for (auto i = 0; i < argument_count - 1; ++i) {
            ss << arguments[i]->Type().ToString() << ", ";
        }
        ss << arguments.back()->Type().ToString();
        ss << ")";
    }
    return ss.str();
}

} // namespace infinity
