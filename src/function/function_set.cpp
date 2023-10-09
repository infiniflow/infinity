//
// Created by JinHai on 2022/9/15.
//

#include "function_set.h"
#include <iostream>

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
