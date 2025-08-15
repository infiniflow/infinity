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

module infinity_core:function_set.impl;

import :function_set;
import :stl;
import :new_catalog;
import :base_expression;

import std;

import function_expr;

namespace infinity {

std::shared_ptr<FunctionSet> FunctionSet::GetFunctionSet(NewCatalog *catalog, const FunctionExpr &expr) {
    std::string function_name = expr.func_name_;

    // std::shared_ptr<Catalog>& catalog
    std::shared_ptr<FunctionSet> function_set_ptr = NewCatalog::GetFunctionSetByName(catalog, function_name);

    return function_set_ptr;
}

std::string FunctionSet::ToString(const std::string &name, const std::vector<std::shared_ptr<BaseExpression>> &arguments) {
    std::stringstream ss;

    ss << name;
    size_t argument_count = arguments.size();
    if (argument_count == 0) {
        ss << "()";

    } else {
        ss << "(";
        for (size_t i = 0; i < argument_count - 1; ++i) {
            ss << arguments[i]->Type().ToString() << ", ";
        }
        ss << arguments.back()->Type().ToString();
        ss << ")";
    }
    return ss.str();
}

} // namespace infinity
