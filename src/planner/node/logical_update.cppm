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
import parser;
import logical_node;
import logical_node_type;
import column_binding;
import catalog;
import base_expression;
export module logical_update;

namespace infinity {

export class LogicalUpdate final : public LogicalNode {

public:
    LogicalUpdate(u64 node_id, TableEntry *table_entry_ptr, const Vector<Pair<SizeT, SharedPtr<BaseExpression>>> &update_columns)
        : LogicalNode(node_id, LogicalNodeType::kUpdate), table_entry_ptr_(table_entry_ptr), update_columns_(update_columns) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalUpdate"; }

    TableEntry *table_entry_ptr_{};
    Vector<Pair<SizeT, SharedPtr<BaseExpression>>> update_columns_; // Column ID = Expression
};

} // namespace infinity
