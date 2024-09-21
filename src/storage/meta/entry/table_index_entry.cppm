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
import chunk_index_entry;
import base_entry;
import index_base;
import block_index;
import third_party;
import status;
import random;
import statement_common;
import cleanup_scanner;
import meta_entry_interface;
import index_file_worker;
import column_def;

import block_entry;

namespace infinity {

class Txn;
class TxnManager;
class TableIndexMeta;
class BufferManager;
struct TableEntry;
struct SegmentEntry;
class BaseTableRef;
class AddTableIndexEntryOp;
class BaseMemIndex;

export struct SegmentIndexesGuard {
    const Map<SegmentID, SharedPtr<SegmentIndexEntry>> &index_by_segment_;
    std::shared_lock<std::shared_mutex> lock_;
};

export struct TableIndexEntry : public BaseEntry, public EntryInterface {
    friend struct TableEntry;

public:
    static Vector<std::string_view> DecodeIndex(std::string_view encode);

    static String EncodeIndex(const String &index_name, TableIndexMeta *index_meta);

public:
    using EntryOp = AddTableIndexEntryOp;

public:
    TableIndexEntry(const SharedPtr<IndexBase> &index_base,
                    bool is_delete,
                    TableIndexMeta *table_index_meta,
                    const SharedPtr<String> &index_entry_dir,
                    TransactionID txn_id,
                    TxnTimeStamp begin_ts);

private:
    TableIndexEntry(const TableIndexEntry &other);

public:
    UniquePtr<TableIndexEntry> Clone(TableIndexMeta *table_index_meta) const;

    static SharedPtr<TableIndexEntry> NewTableIndexEntry(const SharedPtr<IndexBase> &index_base,
                                                         bool is_delete,
                                                         const SharedPtr<String> &table_entry_dir,
                                                         TableIndexMeta *table_index_meta,
                                                         TransactionID txn_id,
                                                         TxnTimeStamp begin_ts);

    static SharedPtr<TableIndexEntry> ReplayTableIndexEntry(TableIndexMeta *table_index_meta,
                                                            bool is_delete,
                                                            const SharedPtr<IndexBase> &index_base,
                                                            const SharedPtr<String> &index_entry_dir,
                                                            TransactionID txn_id,
                                                            TxnTimeStamp begin_ts,
                                                            TxnTimeStamp commit_ts) noexcept;

    nlohmann::json Serialize(TxnTimeStamp max_commit_ts);

    static SharedPtr<TableIndexEntry>
    Deserialize(const nlohmann::json &index_def_entry_json, TableIndexMeta *table_index_meta, BufferManager *buffer_mgr, TableEntry *table_entry);

public:
    // Getter
    const SharedPtr<String> &GetIndexName() const { return index_base_->index_name_; }

    BaseMemIndex *GetMemIndex() const;

    inline TableIndexMeta *table_index_meta() { return table_index_meta_; }
    inline const IndexBase *index_base() const { return index_base_.get(); }
    const SharedPtr<IndexBase> &table_index_def() const { return index_base_; }
    inline const SharedPtr<ColumnDef> &column_def() const { return column_def_; }

    Map<SegmentID, SharedPtr<SegmentIndexEntry>> &index_by_segment() { return index_by_segment_; }

    SegmentIndexesGuard GetSegmentIndexesGuard() const { return {index_by_segment_, std::shared_lock(rw_locker_)}; }

    Map<SegmentID, SharedPtr<SegmentIndexEntry>> GetIndexBySegmentSnapshot(const TableEntry *table_entry, Txn *txn);
    const SharedPtr<String> &index_dir() const { return index_dir_; }
    String GetPathNameTail() const;
    bool GetOrCreateSegment(SegmentID segment_id, Txn *txn, SharedPtr<SegmentIndexEntry> &segment_index_entry);

    // MemIndexCommit is non-blocking.
    // User shall invoke this reguarly to populate recently inserted rows into the fulltext index. Noop for other types of index.
    void MemIndexCommit();

    // MemIndexCommit is blocking.
    // Dump or spill the memory indexer
    SharedPtr<ChunkIndexEntry> MemIndexDump(Txn *txn, TxnTableStore *txn_table_store, bool spill = false, SizeT *dump_size = nullptr);

    // PopulateEntirely is blocking.
    // Populate index entirely for the segment
    SharedPtr<SegmentIndexEntry> PopulateEntirely(SegmentEntry *segment_entry, Txn *txn, const PopulateEntireConfig &config);

    Tuple<Vector<SegmentIndexEntry *>, Status>
    CreateIndexPrepare(BaseTableRef *table_ref, Txn *txn, bool prepare, bool is_replay, bool check_ts = true);

    Status CreateIndexDo(BaseTableRef *table_ref, HashMap<SegmentID, atomic_u64> &create_index_idxes, Txn *txn);

    TxnTimeStamp GetFulltexSegmentUpdateTs() {
        std::shared_lock lock(segment_update_ts_mutex_);
        return segment_update_ts_;
    }

    void UpdateFulltextSegmentTs(TxnTimeStamp ts);

    void CommitCreateIndex(TxnIndexStore *txn_index_store, TxnTimeStamp commit_ts, bool is_replay = false);

    // void RollbackPopulateIndex(TxnIndexStore *txn_index_store, Txn *txn);

    // replay
    void UpdateEntryReplay(TransactionID txn_id, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts);

    void OptIndex(TxnTableStore *txn_table_store, const Vector<UniquePtr<InitParameter>> &opt_params, bool replay);

    bool CheckIfIndexColumn(ColumnID column_id) const;

private:
    static SharedPtr<String> DetermineIndexDir(const String &parent_dir, const String &index_name);

private:
    // For fulltext index
    std::shared_mutex segment_update_ts_mutex_{};
    TxnTimeStamp segment_update_ts_{0};

    mutable std::shared_mutex rw_locker_{};
    TableIndexMeta *table_index_meta_{};
    const SharedPtr<IndexBase> index_base_{};
    const SharedPtr<String> index_dir_{};
    SharedPtr<ColumnDef> column_def_{};

    Map<SegmentID, SharedPtr<SegmentIndexEntry>> index_by_segment_{};
    SharedPtr<SegmentIndexEntry> last_segment_{};

public:
    void Cleanup(CleanupInfoTracer *info_tracer = nullptr, bool dropped = true) override;

    void PickCleanup(CleanupScanner *scanner) override;

    void PickCleanupBySegments(const Vector<SegmentID> &sorted_segment_ids, CleanupScanner *scanner);
};

} // namespace infinity
