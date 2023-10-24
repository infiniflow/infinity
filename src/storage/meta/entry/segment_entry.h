//
// Created by jinhai on 23-8-10.
//

#pragma once

#include "base_entry.h"
#include "storage/meta/entry/block_entry.h"

namespace infinity {

class BufferManager;
class Txn;
class TableCollectionEntry;
class IndexDef;
class IndexEntry;
class AppendState;
class TxnTableStore;

struct SegmentVersion {
public:
    explicit SegmentVersion(SizeT capacity) : created_(capacity), deleted_(capacity), txn_ptr_(capacity) {
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
    static nlohmann::json Serialize(SegmentVersion *segment_version);

    static UniquePtr<SegmentVersion> Deserialize(const nlohmann::json &table_entry_json, SegmentEntry *segment_entry, BufferManager *buffer_mgr);
};

struct SegmentEntry : public BaseEntry {
public:
    explicit SegmentEntry(const TableCollectionEntry *table_entry) : BaseEntry(EntryType::kSegment), table_entry_(table_entry) {}

    RWMutex rw_locker_{};

    const TableCollectionEntry *table_entry_{};

    SharedPtr<String> segment_dir_{};
    i32 segment_id_{};

    SizeT row_count_{};
    SizeT row_capacity_{};
    u64 column_count_{};

    TxnTimeStamp min_row_ts_{0}; // Indicate the commit_ts which create this SegmentEntry
    TxnTimeStamp max_row_ts_{0}; // Indicate the max commit_ts which create/update/delete data inside this SegmentEntry

    Vector<SharedPtr<BlockEntry>> block_entries_{};

    // Want key type to be const String or shared_ptr<const String>
    HashMap<String, SharedPtr<IndexEntry>> index_entry_map_{};

public:
    static SharedPtr<SegmentEntry>
    MakeNewSegmentEntry(const TableCollectionEntry *table_entry, u64 segment_id, BufferManager *buffer_mgr, bool is_new = true);

    static int AppendData(SegmentEntry *segment_entry, Txn *txn_ptr, AppendState *append_state_ptr, BufferManager *buffer_mgr);

    static void DeleteData(SegmentEntry *segment_entry, Txn *txn_ptr, const Vector<RowID> &rows, BufferManager *buffer_mgr);

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

    static void CommitAppend(SegmentEntry *segment_entry, Txn *txn_ptr, i16 block_id, i16 start_pos, i16 row_count);

    static void CommitCreateIndex(SegmentEntry *segment_entry, SharedPtr<IndexEntry> index_entry);

    static void CommitDelete(SegmentEntry *segment_entry, Txn *txn_ptr, u64 start_pos, u64 row_count);

    static u64 GetBlockIDByRowID(SizeT row_id);

    static i16 GetMaxBlockID(const SegmentEntry *segment_entry);

    static BlockEntry *GetBlockEntryByID(const SegmentEntry *segment_entry, u64 block_id);

    static nlohmann::json Serialize(SegmentEntry *segment_entry, TxnTimeStamp max_commit_ts, bool is_full_checkpoint);

    static SharedPtr<SegmentEntry> Deserialize(const nlohmann::json &table_entry_json, TableCollectionEntry *table_entry, BufferManager *buffer_mgr);

    static int Room(SegmentEntry *segment_entry);

    void MergeFrom(infinity::BaseEntry &other) override;

private:
    static SharedPtr<String> DetermineSegFilename(const String &parent_dir, u64 seg_id);
};

} // namespace infinity
