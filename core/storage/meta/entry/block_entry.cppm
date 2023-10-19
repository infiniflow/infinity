//
// Created by jinhai on 23-6-30.
//

module;

import stl;
import default_values;
import base_entry;
import third_party;
import block_column_entry;

export module block_entry;

namespace infinity {

class BufferManager;
class Txn;
class SegmentEntry;
class DataBlock;

enum BlockEntryStatus : i8 {
    kBlockOpen,
    kBlockClosed,
    kBlockFlushing,
};

struct BlockVersion {
    BlockVersion(SizeT capacity) : created_(capacity), deleted_(capacity), txn_ptr_(capacity) {
        for (SizeT i = 0; i < capacity; ++i) {
            created_[i] = MAX_TXN_ID;
            deleted_[i] = MAX_TXN_ID;
            txn_ptr_[i] = (u64)(nullptr);
        }
    }
    Vector<au64> created_{};
    Vector<au64> deleted_{};
    Vector<aptr> txn_ptr_{};
};

export struct BlockEntry : public BaseEntry {
public:
    explicit BlockEntry(const SegmentEntry *segment_entry, i16 block_id, u64 column_count, SizeT start_row, BufferManager *buffer_mgr);

    RWMutex rw_locker_{};

    const SegmentEntry *segment_entry_{};

    SharedPtr<String> base_dir_{};

    i16 start_row_{0};

    i16 row_count_{};

    i16 row_capacity_{};

    i16 block_id_{};

    Atomic<BlockEntryStatus> status_{BlockEntryStatus::kBlockOpen};

    Vector<UniquePtr<BlockColumnEntry>> columns_;

    UniquePtr<BlockVersion> block_version_{};

    au64 min_row_ts_{MAX_TIMESTAMP};
    au64 max_row_ts_{MAX_TIMESTAMP};

    u64 start_ts_{0};
    u64 end_ts_{MAX_TIMESTAMP};

    ptr_t txn_ptr_{nullptr};

public:
    [[nodiscard]] inline SizeT AvailableCapacity() const { return row_capacity_ - row_count_; }

public:
    static i16 AppendData(BlockEntry *block_entry, Txn *txn_ptr, DataBlock *input_data_block, offset_t input_offset, i16 append_rows);

    static void CommitAppend(BlockEntry *block_entry, u64 txn_id_);

    static void CommitDelete(BlockEntry *block_entry, Txn *txn_ptr, u64 start_row_in_segment, u64 row_count);

    static bool PrepareFlush(BlockEntry *block_entry);

    static bool Flush(BlockEntry *block_entry);

    inline static BlockColumnEntry *GetColumnDataByID(BlockEntry *block_entry, u64 column_id) { return block_entry->columns_[column_id].get(); }

    static Json Serialize(const BlockEntry *segment_entry);

    static UniquePtr<BlockEntry> Deserialize(const Json &table_entry_json, SegmentEntry *table_entry, BufferManager *buffer_mgr);

    static bool IsFull(BlockEntry *block_entry);

private:
    static SharedPtr<String> DetermineFilename(const String &parent_dir, u64 seg_id);
};

} // namespace infinity
