// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

#include <sstream>
import stl;
import catalog;
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
    SizeT argument_count = arguments.size();
    if (argument_count == 0) {
        ss << "()";

    } else {
        ss << "(";
        for (SizeT i = 0; i < argument_count - 1; ++i) {
            ss << arguments[i]->Type().ToString() << ", ";
        }
        ss << arguments.back()->Type().ToString();
        ss << ")";
    }
    return ss.str();
}

} // namespace infinity
