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

export module table_index_entry;

import stl;

import txn_store;
import segment_index_entry;
import fulltext_index_entry;
import segment_index_entry;
import base_entry;
import index_base;
import block_index;
import third_party;
import status;
import random;

import cleanup_scanner;
import meta_entry_interface;
import index_file_worker;
import column_def;
import memory_pool;

namespace infinity {

class Txn;
class TxnManager;
class TableIndexMeta;
class BufferManager;
struct TableEntry;
class BaseTableRef;
class AddTableIndexEntryOp;

export struct TableIndexEntry : public BaseEntry, public EntryInterface {
    friend struct TableEntry;

public:
    using EntryOp = AddTableIndexEntryOp;

public:
    TableIndexEntry(const SharedPtr<IndexBase> &index_base,
                    TableIndexMeta *table_index_meta,
                    SharedPtr<String> index_dir,
                    TransactionID txn_id,
                    TxnTimeStamp begin_ts,
                    bool replay = false);

    TableIndexEntry(TableIndexMeta *table_index_meta, TransactionID txn_id, TxnTimeStamp begin_ts);

    static SharedPtr<TableIndexEntry> NewTableIndexEntry(const SharedPtr<IndexBase> &index_base,
                                                         TableIndexMeta *table_index_meta,
                                                         Txn *txn,
                                                         TransactionID txn_id,
                                                         TxnTimeStamp begin_ts,
                                                         bool is_replay = false,
                                                         String replay_table_index_dir = "");

    static SharedPtr<TableIndexEntry>
    NewDropTableIndexEntry(TableIndexMeta *table_index_meta, TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr);

    static SharedPtr<TableIndexEntry> NewReplayTableIndexEntry(TableIndexMeta *table_index_meta,
                                                               const SharedPtr<IndexBase> &index_base,
                                                               const SharedPtr<String> &index_dir,
                                                               TransactionID txn_id,
                                                               TxnTimeStamp begin_ts,
                                                               TxnTimeStamp commit_ts,
                                                               bool is_delete) noexcept;

    static SharedPtr<TableIndexEntry> NewDropReplayTableIndexEntry(TableIndexMeta *table_index_meta, TransactionID txn_id, TxnTimeStamp begin_ts);

    nlohmann::json Serialize(TxnTimeStamp max_commit_ts);

    static SharedPtr<TableIndexEntry>
    Deserialize(const nlohmann::json &index_def_entry_json, TableIndexMeta *table_index_meta, BufferManager *buffer_mgr, TableEntry *table_entry);

public:
    // Getter
    inline const TableIndexMeta *table_index_meta() const { return table_index_meta_; }
    inline const IndexBase *index_base() const { return index_base_.get(); }
    const SharedPtr<IndexBase> &table_index_def() { return index_base_; }

    HashMap<SegmentID, SharedPtr<SegmentIndexEntry>> &segment_index_map() { return segment_index_map_; }
    SharedPtr<FulltextIndexEntry> &fulltext_index_entry() { return fulltext_index_entry_; }
    HashMap<SegmentID, SharedPtr<SegmentIndexEntry>> &index_by_segment() { return index_by_segment_; }
    SharedPtr<String> index_dir() { return index_dir_; }
    bool IsFulltextIndexHomebrewed() const;

    Status CreateIndexPrepare(TableEntry *table_entry, BlockIndex *block_index, Txn *txn, bool prepare, bool is_replay, bool check_ts = true);

    Status CreateIndexDo(const TableEntry *table_entry, HashMap<SegmentID, atomic_u64> &create_index_idxes);

    Vector<UniquePtr<IndexFileWorker>> CreateFileWorker(CreateIndexParam *param, u32 segment_id);

    static String IndexFileName(u32 segment_id) { return fmt::format("seg{}.idx", segment_id); }

    static UniquePtr<CreateIndexParam> GetCreateIndexParam(const IndexBase *index_base, SizeT seg_row_count, const ColumnDef *column_def);

    MemoryPool &GetFulltextByteSlicePool() { return byte_slice_pool_; }
    RecyclePool &GetFulltextBufferPool() { return buffer_pool_; }
    ThreadPool &GetFulltextThreadPool() { return thread_pool_; }
    u64 GetFulltexSegmentUpdateTs() { return segment_update_ts_; }

    void UpdateFulltextSegmentTs(u64 ts) { segment_update_ts_ = ts; }

private:
    static SharedPtr<String> DetermineIndexDir(const String &parent_dir, const String &index_name) {
        return DetermineRandomString(parent_dir, fmt::format("index_{}", index_name));
    }

    // For SegmentIndexEntry
    void CommitCreateIndex(u32 segment_id, SharedPtr<SegmentIndexEntry> index_entry, bool is_replay = false);
    // For FulltextIndexEntry
    void CommitCreateIndex(const SharedPtr<FulltextIndexEntry> &fulltext_index_entry);

private:
    std::shared_mutex rw_locker_{};
    TableIndexMeta *table_index_meta_{};
    const SharedPtr<IndexBase> index_base_{};
    SharedPtr<String> index_dir_{};

    HashMap<SegmentID, SharedPtr<SegmentIndexEntry>> index_by_segment_{};
    HashMap<SegmentID, SharedPtr<SegmentIndexEntry>> segment_index_map_{};
    SharedPtr<FulltextIndexEntry> fulltext_index_entry_{};

    // For fulltext index
    MemoryPool byte_slice_pool_{};
    RecyclePool buffer_pool_{};
    ThreadPool thread_pool_{};
    u64 segment_update_ts_{0};

public:
    void Cleanup() override;

    void PickCleanup(CleanupScanner *scanner) override;

    void PickCleanupBySegments(const Vector<SegmentID> &sorted_segment_ids, CleanupScanner *scanner);
};

} // namespace infinity
