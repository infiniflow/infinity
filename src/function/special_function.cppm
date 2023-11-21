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

import stl;
import new_catalog;
import special_function_set;
import parser;

export module special_function;

namespace infinity {

export void RegisterSpecialFunction(const UniquePtr<NewCatalog> &catalog_ptr) {
    {
        String func_name = "ROW_ID";
        SharedPtr<SpecialFunctionSet> function_set_ptr = MakeShared<SpecialFunctionSet>(func_name, DataType(LogicalType::kBigInt), 1);
        NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
    }

    {
        String func_name = "CREATE_TS";
        SharedPtr<SpecialFunctionSet> function_set_ptr = MakeShared<SpecialFunctionSet>(func_name, DataType(LogicalType::kBigInt), 2);
        NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
    }

    {
        String func_name = "DELETE_TS";
        SharedPtr<SpecialFunctionSet> function_set_ptr = MakeShared<SpecialFunctionSet>(func_name, DataType(LogicalType::kBigInt), 3);
        NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
    }
}

} // namespace infinity