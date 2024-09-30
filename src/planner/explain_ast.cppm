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
import base_statement;
import create_statement;
import insert_statement;
import copy_statement;
import drop_statement;
import select_statement;
import show_statement;
import flush_statement;
import optimize_statement;
import base_table_reference;
import status;

export module explain_ast;

namespace infinity {

export class ExplainAST {
public:
    static Status Explain(const BaseStatement *statement, SharedPtr<Vector<SharedPtr<String>>> &stmt_string, i64 intent_size = 0);

    static Status BuildCreate(const CreateStatement *statement, SharedPtr<Vector<SharedPtr<String>>> &stmt_string, i64 intent_size);

    static Status BuildInsert(const InsertStatement *statement, SharedPtr<Vector<SharedPtr<String>>> &stmt_string, i64 intent_size);

    static Status BuildCopy(const CopyStatement *statement, SharedPtr<Vector<SharedPtr<String>>> &stmt_string, i64 intent_size);

    static Status BuildDrop(const DropStatement *statement, SharedPtr<Vector<SharedPtr<String>>> &stmt_string, i64 intent_size);

    static Status BuildSelect(const SelectStatement *statement,
                              SharedPtr<Vector<SharedPtr<String>>> &stmt_string,
                              i64 intent_size,
                              SharedPtr<String> alias_ptr = nullptr);

    static Status BuildBaseTableRef(const BaseTableReference *table_ref, SharedPtr<Vector<SharedPtr<String>>> &stmt_string, i64 intent_size);

    static Status BuildShow(const ShowStatement *statement, SharedPtr<Vector<SharedPtr<String>>> &stmt_string, i64 intent_size);

    static Status BuildFlush(const FlushStatement *flush_statement, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size);

    static Status BuildOptimize(const OptimizeStatement *optimize_statement, SharedPtr<Vector<SharedPtr<String>>> &result, i64);
};

} // namespace infinity