//
// Created by jinhai on 23-8-10.
//

#pragma once

#include "base_entry.h"
#include "common/default_values.h"
#include "parser/statement/extra/create_index_info.h"
#include "parser/statement/extra/create_table_info.h"
#include "segment_column_entry.h"

#include "storage/index_def/index_def.h"
#include "storage/meta/entry/block_entry.h"
#include "storage/meta/entry/data_access_state.h"

namespace infinity {

class BufferManager;

class Txn;

class TableCollectionEntry;

enum DataSegmentStatus : i8 {
    kSegmentOpen,
    kSegmentClosed,
    kSegmentFlushing,
};

struct SegmentVersion {
public:
    explicit SegmentVersion(SizeT capacity) : created_(capacity), deleted_(capacity), txn_ptr_(capacity) {
        for (SizeT i = 0; i < capacity; ++i) {
            created_[i] = MAX_TXN_ID;
            deleted_[i] = MAX_TXN_ID;
            txn_ptr_[i] = (u64)(nullptr);
        }
    }
    Vector<au64> created_{};
    Vector<au64> deleted_{};
    Vector<aptr> txn_ptr_{};

public:
    static nlohmann::json Serialize(const SegmentVersion *segment_version);

    static UniquePtr<SegmentVersion> Deserialize(const nlohmann::json &table_entry_json, SegmentEntry *segment_entry, BufferManager *buffer_mgr);
};

struct SegmentEntry : public BaseEntry {
public:
    explicit SegmentEntry(const TableCollectionEntry *table_entry) : BaseEntry(EntryType::kSegment), table_entry_(table_entry) {}

    RWMutex rw_locker_{};

    const TableCollectionEntry *table_entry_{};

    SharedPtr<String> segment_dir_{};

    SizeT row_capacity_{};

    SizeT current_row_{};

    i32 segment_id_{};

    std::atomic<DataSegmentStatus> status_{DataSegmentStatus::kSegmentOpen};

    u64 column_count_{};
    //    Vector<SharedPtr<SegmentColumnEntry>> columns_;

    //    UniquePtr<SegmentVersion> segment_version_{};

    au64 min_row_ts_{};
    au64 max_row_ts_{}; // Indicate when the segment is removed.

    u64 start_ts_{0};
    u64 end_ts_{MAX_TIMESTAMP};

    Vector<UniquePtr<BlockEntry>> block_entries_{};

    SharedPtr<String> index_dir_{};

    HashMap<u64, SharedPtr<String>> index_name_map_{};

    // TODO shenyushi 3: add index here
public:
    [[nodiscard]] inline SizeT AvailableCapacity() const { return row_capacity_ - current_row_; }

public:
    static SharedPtr<SegmentEntry> MakeNewSegmentEntry(const TableCollectionEntry *table_entry,
                                                       u64 txn_id,
                                                       u64 segment_id,
                                                       BufferManager *buffer_mgr,
                                                       SizeT segment_row = DEFAULT_SEGMENT_ROW);

    static void AppendData(SegmentEntry *segment_entry, Txn *txn_ptr, AppendState *append_state_ptr, BufferManager *buffer_mgr);

    static void CreateIndexScalar(SegmentEntry *segment_entry, Txn *txn_ptr, const IndexDef &index_def, u64 column_id);

    static void CreateIndexEmbedding(SegmentEntry *segment_entry, Txn *txn_ptr, const IndexDef &index_def, u64 column_id, int dimension);

    static void CommitAppend(SegmentEntry *segment_entry, Txn *txn_ptr, i16 block_id, i16 start_pos, i16 row_count);

    static void CommitDelete(SegmentEntry *segment_entry, Txn *txn_ptr, u64 start_pos, u64 row_count);

    static bool PrepareFlush(SegmentEntry *segment_entry);

    static UniquePtr<String> Flush(SegmentEntry *segment_entry);

    //    inline static SegmentColumnEntry*
    //    GetColumnDataByID(SegmentEntry* segment_entry, u64 column_id) {
    //        return segment_entry->columns_[column_id].get();
    //    }

    static u64 GetBlockIDByRowID(SizeT row_id);

    static i16 GetMaxBlockID(const SegmentEntry *segment_entry);

    static BlockEntry *GetBlockEntryByID(const SegmentEntry *segment_entry, u64 block_id);

    static nlohmann::json Serialize(const SegmentEntry *segment_entry);

    static SharedPtr<SegmentEntry> Deserialize(const nlohmann::json &table_entry_json, TableCollectionEntry *table_entry, BufferManager *buffer_mgr);

private:
    static SharedPtr<String> DetermineSegFilename(const String &parent_dir, u64 seg_id);

    static SharedPtr<String> DetermineIndexFilename(const String &parent_dir, const String &index_name, u64 seg_id);
};

} // namespace infinity
