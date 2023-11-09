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
import logical_node_type;
import column_binding;
import logical_node;
import parser;

export module logical_show;

namespace infinity {

export enum class ShowType {
    kInvalid,
    kShowDatabases,
    kShowTables,
    kShowViews,
    kShowColumn,
    kShowConfigs,
    kShowProfiles,
    kShowIndexes,
    kIntermediate,
};

export String ToString(ShowType type);

export class LogicalShow : public LogicalNode {
public:
    explicit LogicalShow(u64 node_id, ShowType type, String schema_name, String object_name, u64 table_index)
        : LogicalNode(node_id, LogicalNodeType::kShow), scan_type_(type), schema_name_(Move(schema_name)), object_name_(Move(object_name)),
          table_index_(table_index) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalShow"; }

    [[nodiscard]] ShowType scan_type() const { return scan_type_; }

    [[nodiscard]] inline u64 table_index() const { return table_index_; }

    [[nodiscard]] inline const String &schema_name() const { return schema_name_; }

    [[nodiscard]] inline const String &object_name() const { return object_name_; }

private:
    ShowType scan_type_{ShowType::kInvalid};
    String schema_name_;
    String object_name_; // It could be table/collection/view name
    u64 table_index_{};
};

} // namespace infinity
