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
import function;
import function_data;
import parser;
//import query_context;
import data_block;

export module table_function;

namespace infinity {

class QueryContext;
//class DataBlock;

export class TableFunctionData : public FunctionData {
public:
    ~TableFunctionData() override = default;
};

export using TableFunctionType = StdFunction<void(QueryContext *query_context, TableFunctionData *data, DataBlock &output)>;

export class TableFunction : public Function {
public:
    explicit TableFunction(String name, Vector<LogicalType> argument_types, TableFunctionType function);

    [[nodiscard]] String ToString() const override;

    TableFunctionType main_function_;

private:
    Vector<LogicalType> parameter_types_;
};
} // namespace infinity
