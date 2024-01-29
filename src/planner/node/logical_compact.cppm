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
import column_binding;
import logical_node;
import catalog;
import parser;
import logical_node_type;

export module logical_compact;

namespace infinity {

export class LogicalCompact : public LogicalNode {
public:
    explicit LogicalCompact(u64 node_id, TableEntry *table_entry) : LogicalNode(node_id, LogicalNodeType::kCompact), table_entry_(table_entry) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalCompact"; }

    inline TableEntry *table_entry() { return table_entry_; }

private:
    TableEntry *const table_entry_;
};
} // namespace infinity