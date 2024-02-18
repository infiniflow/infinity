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

namespace infinity {

class Txn;
struct TableIndexEntry;
struct FulltextIndexEntry;
struct TableEntry;
struct SegmentEntry;
class DataBlock;
class SegmentColumnIndexEntry;
class BGTaskProcessor;
class TxnManager;
enum class CompactSegmentsTaskType;

struct TxnSegmentIndexStore {
    HashMap<u32, SharedPtr<SegmentColumnIndexEntry>> index_entry_map_{};
};

export struct TxnIndexStore {
public:
    explicit TxnIndexStore(TableIndexEntry *table_index_entry);
    TxnIndexStore() = default;

    TableIndexEntry *const table_index_entry_{};

    //    Vector<ColumnIndexEntry*> column_index_entry_{};
    //
    FulltextIndexEntry *irs_index_entry_{};

    HashMap<u64, HashMap<u32, SharedPtr<SegmentColumnIndexEntry>>> index_entry_map_{}; // column_id -> segment_id -> segment_column_index_entry
};

export struct TxnCompactStore {
    Vector<Pair<SharedPtr<SegmentEntry>, Vector<SegmentEntry *>>> segment_data_;

    CompactSegmentsTaskType task_type_;
};

export class TxnTableStore {
public:
    explicit inline TxnTableStore(TableEntry *table_entry, Txn *txn) : table_entry_(table_entry), txn_(txn) {}

    Tuple<UniquePtr<String>, Status> Append(const SharedPtr<DataBlock> &input_block);

    Tuple<UniquePtr<String>, Status> Import(const SharedPtr<SegmentEntry> &segment);

    Tuple<UniquePtr<String>, Status>
    CreateIndexFile(TableIndexEntry *table_index_entry, u64 column_id, u32 segment_id, SharedPtr<SegmentColumnIndexEntry> index);

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
    Vector<SharedPtr<SegmentEntry>> uncommitted_segments_{};
    HashMap<String, TxnIndexStore> txn_indexes_store_{};
    UniquePtr<AppendState> append_state_{};
    DeleteState delete_state_{};
    TxnCompactStore compact_state_{};

    SizeT current_block_id_{0};

    TableEntry *table_entry_{};
    Txn *txn_{};
};

} // namespace infinity
