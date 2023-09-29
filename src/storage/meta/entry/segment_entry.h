//
// Created by jinhai on 23-8-10.
//


#pragma once

#include <mutex>
#include <utility>

#include "base_entry.h"
#include "column_data_entry.h"
#include "common/default_values.h"
#include "common/types/internal_types.h"
#include "common/utility/infinity_assert.h"
#include "common/utility/random.h"
#include "data_access_state.h"

namespace infinity {

class BufferManager;

class Txn;

class TableCollectionEntry;

enum DataSegmentStatus : i8 {
    kOpen,
    kClosed,
    kFlushing,
};

struct SegmentVersion {
    explicit
    SegmentVersion(SizeT capacity) : created_(capacity), deleted_(capacity), txn_ptr_(capacity) {
        for(SizeT i = 0; i < capacity; ++i) {
            created_[i] = MAX_TXN_ID;
            deleted_[i] = MAX_TXN_ID;
            txn_ptr_[i] = (u64)(nullptr);
        }
    }
    Vector<au64> created_{};
    Vector<au64> deleted_{};
    Vector<aptr> txn_ptr_{};
};

struct SegmentEntry : public BaseEntry {
public:
    explicit
    SegmentEntry(const TableCollectionEntry* table_entry)
            : BaseEntry(EntryType::kSegment), table_entry_(table_entry) {}

    RWMutex rw_locker_{};

    const TableCollectionEntry* table_entry_{};

    SharedPtr<String> base_dir_{};

    SizeT row_capacity_{};

    SizeT current_row_{};

    u64 segment_id_{};

    std::atomic<DataSegmentStatus> status_{DataSegmentStatus::kOpen};

    Vector<SharedPtr<ColumnDataEntry>> columns_;

    UniquePtr<SegmentVersion> segment_version_{};

    au64 start_txn_id_{};
    au64 end_txn_id_{};   // Indicate when the segment is removed.

public:
    [[nodiscard]] inline SizeT
    AvailableCapacity() const {
        return row_capacity_ - current_row_;
    }

public:
    static SharedPtr<SegmentEntry>
    MakeNewSegmentEntry(const TableCollectionEntry* table_entry,
                        u64 txn_id,
                        u64 segment_id,
                        BufferManager* buffer_mgr,
                        SizeT segment_row = DEFAULT_SEGMENT_ROW);

    static void
    AppendData(SegmentEntry* segment_entry, Txn* txn_ptr, AppendState* append_state_ptr, BufferManager* buffer_mgr);

    static void
    CommitAppend(SegmentEntry* segment_entry, Txn* txn_ptr, u64 start_pos, u64 row_count);

    static void
    CommitDelete(SegmentEntry* segment_entry, Txn* txn_ptr, u64 start_pos, u64 row_count);

    static bool
    PrepareFlush(SegmentEntry* segment_entry);

    static UniquePtr<String>
    Flush(SegmentEntry* segment_entry);

    inline static ColumnDataEntry*
    GetColumnDataByID(SegmentEntry* segment_entry, u64 column_id) {
        return segment_entry->columns_[column_id].get();
    }

    static nlohmann::json
    Serialize(const SegmentEntry* segment_entry);

    static SharedPtr<SegmentEntry>
    Deserialize(const nlohmann::json& table_entry_json, TableCollectionEntry* table_entry, BufferManager* buffer_mgr);

private:
    static SharedPtr<String>
    DetermineFilename(const String& parent_dir, u64 seg_id);
};

}
