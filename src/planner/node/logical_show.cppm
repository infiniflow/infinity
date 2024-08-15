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
    kShowIndexChunk,
    kShowDatabases,
    kShowTables,
    kShowViews,
    kShowColumn,
    kShowConfigs,
    kShowProfiles,
    kShowQueries,
    kShowQuery,
    kShowTransactions,
    kShowTransaction,
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
    kShowBuffer,
    kShowMemIndex,
    kShowLogs,
    kShowDeltaLogs,
    kShowCatalogs,
    kShowPersistenceFiles,
    kShowPersistenceObjects,
    kShowPersistenceObject,
};

export String ToString(ShowType type);

export class LogicalShow : public LogicalNode {
public:
    explicit LogicalShow(u64 node_id,
                         ShowType type,
                         String schema_name,
                         String object_name,
                         u64 table_index,
                         Optional<SegmentID> segment_id = None,
                         Optional<BlockID> block_id = None,
                         Optional<ChunkID> chunk_id = None,
                         Optional<ColumnID> column_id = None,
                         Optional<String> index_name = None,
                         Optional<TransactionID> session_id = None,
                         Optional<u64> txn_id = None)
        : LogicalNode(node_id, LogicalNodeType::kShow), show_type_(type), schema_name_(std::move(schema_name)), object_name_(std::move(object_name)),
          table_index_(table_index), segment_id_(segment_id), block_id_(block_id), chunk_id_(chunk_id), column_id_(column_id), index_name_(index_name), session_id_(session_id), txn_id_(txn_id) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalShow"; }

    [[nodiscard]] ShowType show_type() const { return show_type_; }

    [[nodiscard]] inline u64 table_index() const { return table_index_; }

    [[nodiscard]] inline const String &schema_name() const { return schema_name_; }

    [[nodiscard]] inline const String &object_name() const { return object_name_; }

    [[nodiscard]] inline const Optional<u64> session_id() const { return session_id_; }

    [[nodiscard]] inline const Optional<SegmentID> segment_id() const { return segment_id_; }

    [[nodiscard]] inline const Optional<TransactionID> transaction_id() const { return txn_id_; }

    [[nodiscard]] inline const Optional<BlockID> block_id() const { return block_id_; }

    [[nodiscard]] inline const Optional<ChunkID> chunk_id() const { return chunk_id_; }

    [[nodiscard]] inline const Optional<ColumnID> column_id() const { return column_id_; }

    [[nodiscard]] inline const Optional<String> index_name() const { return index_name_; }

private:
    ShowType show_type_{ShowType::kInvalid};
    String schema_name_;
    String object_name_; // It could be table/collection/view name
    u64 table_index_{};

    Optional<SegmentID> segment_id_{};
    Optional<BlockID> block_id_{};
    Optional<ChunkID> chunk_id_{};
    Optional<ColumnID> column_id_{};
    Optional<String> index_name_{};
    Optional<u64> session_id_{};
    Optional<TransactionID> txn_id_{};
};

} // namespace infinity
