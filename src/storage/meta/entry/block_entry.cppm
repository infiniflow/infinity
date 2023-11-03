//
// Created by jinhai on 23-6-30.
//

module;

import stl;
import default_values;
import base_entry;
import third_party;
import block_column_entry;
import parser;

export module block_entry;

namespace infinity {

class BufferManager;
class Txn;
class SegmentEntry;
class DataBlock;

export struct BlockVersion {
    BlockVersion(SizeT capacity) : deleted_(capacity, 0) {}
    bool operator==(const BlockVersion &rhs) const;
    bool operator!=(const BlockVersion &rhs) const { return !(*this == rhs); };
    i32 GetRowCount(TxnTimeStamp begin_ts);
    void LoadFromFile(const String &version_path);
    void SaveToFile(const String &version_path);

    Vector<Pair<TxnTimeStamp, i32>> created_{};
    Vector<TxnTimeStamp> deleted_{};
};

export struct BlockEntry : public BaseEntry {
public:
    /// Normal Constructor
    explicit BlockEntry(const SegmentEntry *segment_entry, u16 block_id, TxnTimeStamp checkpoint_ts, u64 column_count, BufferManager *buffer_mgr);
    /// Construct a new block entry For Replay
    explicit BlockEntry(const SegmentEntry *segment_entry,
                        u16 block_id,
                        TxnTimeStamp checkpoint_ts,
                        u64 column_count,
                        BufferManager *buffer_mgr,
                        u16 row_count_,
                        i16 min_row_ts_,
                        i16 max_row_ts_);

    RWMutex rw_locker_{};

    const SegmentEntry *segment_entry_{};

    SharedPtr<String> base_dir_{};

    u16 block_id_{};
    u16 row_count_{};
    u16 row_capacity_{};

    Vector<UniquePtr<BlockColumnEntry>> columns_;

    UniquePtr<BlockVersion> block_version_{};

    TxnTimeStamp min_row_ts_{0};    // Indicate the commit_ts which create this BlockEntry
    TxnTimeStamp max_row_ts_{0};    // Indicate the max commit_ts which create/update/delete data inside this BlockEntry
    TxnTimeStamp checkpoint_ts_{0}; // replay not set

    Txn *txn_ptr_{nullptr};
    BufferManager *buffer_{nullptr};

    // checkpoint state
    u16 checkpoint_row_count_{0};

public:
    // Get visible range of the BlockEntry since the given row number for a txn
    static Pair<u16, u16> VisibleRange(BlockEntry *block_entry, TxnTimeStamp begin_ts, u16 block_offset_begin = 0);

    static u16
    AppendData(BlockEntry *block_entry, Txn *txn_ptr, DataBlock *input_data_block, u16 input_block_offset, u16 append_rows, BufferManager *buffer_mgr);

    static void DeleteData(BlockEntry *block_entry, Txn *txn_ptr, const Vector<RowID>& rows);

    static void CommitAppend(BlockEntry *block_entry, Txn *txn_ptr);

    static void CommitDelete(BlockEntry *block_entry, Txn *txn_ptr);

    static void Flush(BlockEntry *block_entry, TxnTimeStamp checkpoint_ts);

    static void FlushData(BlockEntry *block_entry, i64 checkpoint_row_count);

    static void FlushVersion(BlockEntry *block_entry, BlockVersion &checkpoint_version);

    inline static BlockColumnEntry *GetColumnDataByID(BlockEntry *block_entry, u64 column_id) { return block_entry->columns_[column_id].get(); }

    static Json Serialize(BlockEntry *segment_entry, TxnTimeStamp max_commit_ts);

    static UniquePtr<BlockEntry> Deserialize(const Json &table_entry_json, SegmentEntry *table_entry, BufferManager *buffer_mgr);

    static i32 Room(BlockEntry *block_entry);

    void MergeFrom(BaseEntry &other) override;

private:
    static SharedPtr<String> DetermineDir(const String &parent_dir, u64 block_id);
};
} // namespace infinity
