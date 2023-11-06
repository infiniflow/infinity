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

import std;
import stl;
import function;
import parser;
import query_context;

module table_function;
namespace infinity {

TableFunction::TableFunction(String name, Vector<LogicalType> argument_types, TableFunctionType function)
    : Function(Move(name), FunctionType::kTable), parameter_types_(Move(argument_types)), main_function_(Move(function)) {}

String TableFunction::ToString() const {
    std::stringstream ss;
    ss << "Table Function: " << name_;
    return ss.str();
}

} // namespace infinity
