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
import catalog;
import table_function;
import query_context;
import data_block;

export module table_scan;

namespace infinity {

export void RegisterTableScanFunction(const UniquePtr<NewCatalog> &catalog_ptr);

export class TableScanFunction : public TableFunction {
public:
    static SharedPtr<TableScanFunction> Make(NewCatalog *catalog, const String &func_name);

public:
    explicit TableScanFunction(String name, TableFunctionType function) : TableFunction(Move(name), {}, Move(function)) {}

private:
};

static void TableScanFunc(QueryContext *query_context, TableFunctionData *table_function_data_ptr, DataBlock &output);

} // namespace infinity
