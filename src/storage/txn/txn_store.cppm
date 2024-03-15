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
struct BlockEntry;
class DataBlock;
class SegmentIndexEntry;
class BGTaskProcessor;
class TxnManager;
enum class CompactSegmentsTaskType;

export struct TxnSegmentStore {
public:
    static TxnSegmentStore AddSegmentStore(SegmentEntry *segment_entry);

    explicit TxnSegmentStore(SegmentEntry *segment_entry);

    TxnSegmentStore() = default;

public:
    SegmentEntry *const segment_entry_ = nullptr;
    Vector<BlockEntry *> block_entries_;
};

export struct TxnIndexStore {
public:
    explicit TxnIndexStore(TableIndexEntry *table_index_entry);
    TxnIndexStore() = default;

public:
    TableIndexEntry *const table_index_entry_{};

    FulltextIndexEntry *fulltext_index_entry_{};

    HashMap<SegmentID, SegmentIndexEntry *> index_entry_map_{};
};

export struct TxnCompactStore {
    Vector<Pair<TxnSegmentStore, Vector<SegmentEntry *>>> compact_data_;

    CompactSegmentsTaskType task_type_;

    TxnCompactStore();
    TxnCompactStore(CompactSegmentsTaskType type);
};

export class TxnTableStore {
public:
    explicit inline TxnTableStore(TableEntry *table_entry, Txn *txn) : table_entry_(table_entry), txn_(txn) {}

    Tuple<UniquePtr<String>, Status> Import(SharedPtr<SegmentEntry> segment_entry);

    Tuple<UniquePtr<String>, Status> Append(const SharedPtr<DataBlock> &input_block);

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

    void AddSegmentStore(SegmentEntry *segment_entry);

    void AddBlockStore(SegmentEntry *segment_entry, BlockEntry *block_entry);

    void AddSealedSegment(SegmentEntry *segment_entry);

public: // Getter
    const HashMap<String, UniquePtr<TxnIndexStore>> &txn_indexes_store() const { return txn_indexes_store_; }

    const HashMap<SegmentID, TxnSegmentStore> &txn_segments() const { return txn_segments_; }

private:
    HashMap<SegmentID, TxnSegmentStore> txn_segments_{};
    Vector<SegmentEntry *> set_sealed_segments_{};

    Set<TableIndexEntry *> txn_indexes_{};
    HashMap<String, UniquePtr<TxnIndexStore>> txn_indexes_store_{};

    TxnCompactStore compact_state_;

public:
    Vector<SharedPtr<DataBlock>> blocks_{};

    UniquePtr<AppendState> append_state_{};
    DeleteState delete_state_{};

    SizeT current_block_id_{0};

    TableEntry *table_entry_{};
    Txn *txn_{};
};

} // namespace infinity
