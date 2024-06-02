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

export module logical_show;

import stl;
import logical_node_type;
import column_binding;
import logical_node;
import data_type;
import internal_types;

namespace infinity {

export enum class ShowType {
    kInvalid,
    kShowDatabase,
    kShowTable,
    kShowIndex,
    kShowIndexSegment,
    kShowDatabases,
    kShowTables,
    kShowViews,
    kShowColumn,
    kShowConfigs,
    kShowProfiles,
    kShowIndexes,
    kShowSegments,
    kShowSegment,
    kShowBlocks,
    kShowBlock,
    kShowBlockColumn,
    kShowSessionVariable,
    kShowSessionVariables,
    kShowGlobalVariable,
    kShowGlobalVariables,
    kShowConfig,
};

export String ToString(ShowType type);

export class LogicalShow : public LogicalNode {
public:
    explicit LogicalShow(u64 node_id,
                         ShowType type,
                         String schema_name,
                         String object_name,
                         u64 table_index,
                         Optional<u32> segment_id = None,
                         Optional<u16> block_id = None,
                         Optional<u32> column_id = None,
                         Optional<String> index_name = None)
        : LogicalNode(node_id, LogicalNodeType::kShow), scan_type_(type), schema_name_(std::move(schema_name)), object_name_(std::move(object_name)),
          table_index_(table_index), segment_id_(segment_id), block_id_(block_id), column_id_(column_id), index_name_(index_name) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalShow"; }

    [[nodiscard]] ShowType scan_type() const { return scan_type_; }

    [[nodiscard]] inline u64 table_index() const { return table_index_; }

    [[nodiscard]] inline const String &schema_name() const { return schema_name_; }

    [[nodiscard]] inline const String &object_name() const { return object_name_; }

    [[nodiscard]] inline const Optional<u32> segment_id() const { return segment_id_; }

    [[nodiscard]] inline const Optional<u16> block_id() const { return block_id_; }

    [[nodiscard]] inline const Optional<u32> column_id() const { return column_id_; }

    [[nodiscard]] inline const Optional<String> index_name() const { return index_name_; }

private:
    ShowType scan_type_{ShowType::kInvalid};
    String schema_name_;
    String object_name_; // It could be table/collection/view name
    u64 table_index_{};
    Optional<u32> segment_id_{};
    Optional<u16> block_id_{};
    Optional<u64> column_id_{};
    Optional<String> index_name_{};
};

} // namespace infinity
