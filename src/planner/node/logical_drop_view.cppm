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

export module infinity_core:logical_drop_view;

import :stl;
import :logical_node_type;
import :column_binding;
import :logical_node;
import data_type;
import internal_types;
import extra_ddl_info;

namespace infinity {

export class LogicalDropView final : public LogicalNode {
public:
    LogicalDropView(u64 node_id, SharedPtr<String> schema_name, SharedPtr<String> view_name, ConflictType conflict_type)
        : LogicalNode(node_id, LogicalNodeType::kDropView), schema_name_(std::move(schema_name)), view_name_(std::move(view_name)),
          conflict_type_(conflict_type) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalDropView"; }

    [[nodiscard]] SharedPtr<String> view_name() const { return view_name_; }

    [[nodiscard]] SharedPtr<String> schema_name() const { return schema_name_; }

    [[nodiscard]] inline ConflictType conflict_type() const { return conflict_type_; }

private:
    SharedPtr<String> schema_name_{};
    SharedPtr<String> view_name_{};
    ConflictType conflict_type_{ConflictType::kInvalid};
};

} // namespace infinity
