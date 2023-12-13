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

import parser;
import stl;

export module explain_ast;

namespace infinity {

export class ExplainAST {
public:
    static void Explain(const BaseStatement *statement, SharedPtr<Vector<SharedPtr<String>>> &stmt_string, i64 intent_size = 0);

    static void BuildCreate(const CreateStatement *statement, SharedPtr<Vector<SharedPtr<String>>> &stmt_string, i64 intent_size);

    static void BuildInsert(const InsertStatement *statement, SharedPtr<Vector<SharedPtr<String>>> &stmt_string, i64 intent_size);

    static void BuildCopy(const CopyStatement *statement, SharedPtr<Vector<SharedPtr<String>>> &stmt_string, i64 intent_size);

    static void BuildDrop(const DropStatement *statement, SharedPtr<Vector<SharedPtr<String>>> &stmt_string, i64 intent_size);

    static void BuildSelect(const SelectStatement *statement,
                            SharedPtr<Vector<SharedPtr<String>>> &stmt_string,
                            i64 intent_size,
                            SharedPtr<String> alias_ptr = nullptr);

    static void BuildBaseTableRef(const BaseTableReference *table_ref, SharedPtr<Vector<SharedPtr<String>>> &stmt_string, i64 intent_size);

    static void BuildShow(const ShowStatement *statement, SharedPtr<Vector<SharedPtr<String>>> &stmt_string, i64 intent_size);

    static void BuildFlush(const FlushStatement *flush_statement, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size);

    static void BuildOptimize(const OptimizeStatement *optimize_statement, SharedPtr<Vector<SharedPtr<String>>> &result, i64);
};

} // namespace infinity