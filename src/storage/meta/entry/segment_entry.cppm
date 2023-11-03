//
// Created by jinhai on 23-8-10.
//

module;

import stl;
import default_values;
import base_entry;
import block_entry;
import third_party;
import txn;
import buffer_manager;
import index_def;
import data_access_state;
import parser;
import index_entry;
import txn_store;

export module segment_entry;

namespace infinity {

// class BufferManager;
//
// class Txn;
//
class TableCollectionEntry;
//
// class IndexDef;
//
export class SegmentEntry;

export struct SegmentVersion {
public:
    explicit inline SegmentVersion(SizeT capacity) : created_(capacity), deleted_(capacity), txn_ptr_(capacity) {
        for (SizeT i = 0; i < capacity; ++i) {
            created_[i] = 0;
            deleted_[i] = 0;
            txn_ptr_[i] = (u64)(nullptr);
        }
    }
    Vector<au64> created_{};
    Vector<au64> deleted_{};
    Vector<aptr> txn_ptr_{};

public:
    static Json Serialize(SegmentVersion *segment_version);

    static UniquePtr<SegmentVersion> Deserialize(const Json &table_entry_json, SegmentEntry *segment_entry, BufferManager *buffer_mgr);
};

struct SegmentEntry : public BaseEntry {
public:
    explicit SegmentEntry(const TableCollectionEntry *table_entry);

    RWMutex rw_locker_{};

    const TableCollectionEntry *table_entry_{};

    SharedPtr<String> segment_dir_{};
    u32 segment_id_{};

    SizeT row_count_{};
    SizeT row_capacity_{};
    u64 column_count_{};

    TxnTimeStamp min_row_ts_{0}; // Indicate the commit_ts which create this SegmentEntry
    TxnTimeStamp max_row_ts_{0}; // Indicate the max commit_ts which create/update/delete data inside this SegmentEntry

    Vector<SharedPtr<BlockEntry>> block_entries_{};

    // Want key type to be const String or shared_ptr<const String>
    HashMap<String, SharedPtr<IndexEntry>> index_entry_map_{};

public:
    static SharedPtr<SegmentEntry> MakeNewSegmentEntry(const TableCollectionEntry *table_entry, u32 segment_id, BufferManager *buffer_mgr);

    static SharedPtr<SegmentEntry>
    MakeReplaySegmentEntry(const TableCollectionEntry *table_entry, u32 segment_id, SharedPtr<String> segment_dir, TxnTimeStamp commit_ts);

    static u64 AppendData(SegmentEntry *segment_entry, Txn *txn_ptr, AppendState *append_state_ptr, BufferManager *buffer_mgr);

    static void DeleteData(SegmentEntry *segment_entry, Txn *txn_ptr, const HashMap<u16, Vector<RowID>>& block_row_hashmap);

    static void CreateIndexScalar(SegmentEntry *segment_entry,
                                  Txn *txn_ptr,
                                  const IndexDef &index_def,
                                  u64 column_id,
                                  BufferManager *buffer_mgr,
                                  TxnTableStore *txn_store);

    static SharedPtr<IndexEntry> CreateIndexEmbedding(SegmentEntry *segment_entry,
                                                      const IndexDef &index_def,
                                                      u64 column_id,
                                                      int dimension,
                                                      TxnTimeStamp create_ts,
                                                      BufferManager *buffer_mgr,
                                                      TxnTableStore *txn_store);

    static void CommitAppend(SegmentEntry *segment_entry, Txn *txn_ptr, u16 block_id, u16 start_pos, u16 row_count);

    static void CommitCreateIndex(SegmentEntry *segment_entry, SharedPtr<IndexEntry> index_entry);

    static void CommitDelete(SegmentEntry *segment_entry, Txn *txn_ptr, const HashMap<u16, Vector<RowID>>& block_row_hashmap);

    static u16 GetMaxBlockID(const SegmentEntry *segment_entry);

    static BlockEntry *GetBlockEntryByID(const SegmentEntry *segment_entry, u16 block_id);

    static Json Serialize(SegmentEntry *segment_entry, TxnTimeStamp max_commit_ts, bool is_full_checkpoint);

    static SharedPtr<SegmentEntry> Deserialize(const Json &table_entry_json, TableCollectionEntry *table_entry, BufferManager *buffer_mgr);

    static int Room(SegmentEntry *segment_entry);

    void MergeFrom(infinity::BaseEntry &other) override;

private:
    static SharedPtr<String> DetermineSegFilename(const String &parent_dir, u32 seg_id);
};

} // namespace infinity
