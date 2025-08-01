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

module infinity_core:table_function.impl;

import :table_function;

import :stl;
import :function;
import :query_context;
import logical_type;

namespace infinity {

TableFunction::TableFunction(String name, Vector<LogicalType> argument_types, TableFunctionType function)
    : Function(std::move(name), FunctionType::kTable), main_function_(std::move(function)), parameter_types_(std::move(argument_types)) {}

String TableFunction::ToString() const {
    std::stringstream ss;
    ss << "Table Function: " << name_;
    return ss.str();
}

} // namespace infinity
