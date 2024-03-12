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

export module txn_store;

import stl;

import data_access_state;
import status;
import internal_types;
import index_base;
import extra_ddl_info;

namespace infinity {

class Txn;
struct TableIndexEntry;
struct FulltextIndexEntry;
struct TableEntry;
struct SegmentEntry;
class DataBlock;
class SegmentIndexEntry;
class BGTaskProcessor;
class TxnManager;
enum class CompactSegmentsTaskType;

export struct TxnIndexStore {
public:
    explicit TxnIndexStore(TableIndexEntry *table_index_entry);
    TxnIndexStore() = default;

    TableIndexEntry *const table_index_entry_{};

    FulltextIndexEntry *fulltext_index_entry_{};

    HashMap<SegmentID, SegmentIndexEntry *> index_entry_map_{};
};

export struct TxnCompactStore {
    Vector<Pair<SharedPtr<SegmentEntry>, Vector<SegmentEntry *>>> segment_data_;

    CompactSegmentsTaskType task_type_;

    TxnCompactStore();
    TxnCompactStore(Vector<Pair<SharedPtr<SegmentEntry>, Vector<SegmentEntry *>>> &&, CompactSegmentsTaskType);
};

export class TxnTableStore {
public:
    explicit inline TxnTableStore(TableEntry *table_entry, Txn *txn) : table_entry_(table_entry), txn_(txn) {}

    Tuple<UniquePtr<String>, Status> Append(const SharedPtr<DataBlock> &input_block);

    Tuple<UniquePtr<String>, Status> Import(const SharedPtr<SegmentEntry> &segment);

    void AddIndexStore(TableIndexEntry *table_index_entry);

    void AddSegmentIndexesStore(TableIndexEntry *table_index_entry, const Vector<SegmentIndexEntry *> &segment_index_entries);

    void AddFulltextIndexStore(TableIndexEntry *table_index_entry, FulltextIndexEntry *fulltext_index_entry);

    TxnIndexStore *GetIndexStore(TableIndexEntry *table_index_entry);

    void DropIndexStore(TableIndexEntry *table_index_entry);

    Tuple<UniquePtr<String>, Status> Delete(const Vector<RowID> &row_ids);

    Tuple<UniquePtr<String>, Status> Compact(Vector<Pair<SharedPtr<SegmentEntry>, Vector<SegmentEntry *>>> &&segment_data,
                                             CompactSegmentsTaskType type);

    void Scan(SharedPtr<DataBlock> &output_block);

    void Rollback();

    void PrepareCommit();

    void Commit() const;

    void TryTriggerCompaction(BGTaskProcessor *bg_task_processor, TxnManager *txn_mgr);

public:
    Vector<SharedPtr<DataBlock>> blocks_{};
    Vector<SharedPtr<SegmentEntry>> uncommitted_segments_{}; // Only used for IMPORT and COMPACT

    Set<TableIndexEntry *> txn_indexes_{};
    HashMap<String, UniquePtr<TxnIndexStore>> txn_indexes_store_{};

    UniquePtr<AppendState> append_state_{};
    DeleteState delete_state_{};

    SizeT current_block_id_{0};

    TableEntry *table_entry_{};
    Txn *txn_{};

private:
    TxnCompactStore compact_state_;
};

} // namespace infinity
