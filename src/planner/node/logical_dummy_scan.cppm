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

export module infinity_core:logical_dummy_scan;

import :stl;
import :logical_node_type;
import :column_binding;
import :logical_node;

import data_type;
import internal_types;

namespace infinity {

export class LogicalDummyScan : public LogicalNode {
public:
    explicit LogicalDummyScan(u64 node_id, String table_alias, u64 table_index)
        : LogicalNode(node_id, LogicalNodeType::kDummyScan), table_alias_(std::move(table_alias)), table_index_(table_index) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalDummyScan"; }

    String table_alias_;
    u64 table_index_;
};

} // namespace infinity
