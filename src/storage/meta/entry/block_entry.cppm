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

export module block_entry;

import stl;
import default_values;
import third_party;
import data_type;
import local_file_system;
import column_vector;
import bitmask;
import internal_types;
import base_entry;
import block_column_entry;

namespace infinity {

class BufferManager;
class Txn;
struct SegmentEntry;
struct TableEntry;
class DataBlock;

#pragma pack(4)
struct CreateField {
    //    CreateField(TxnTimeStamp create_ts, i32 row_count) : create_ts_(create_ts), row_count_(row_count) {}

    TxnTimeStamp create_ts_{};
    i32 row_count_{};

    bool operator==(const CreateField &rhs) const { return create_ts_ == rhs.create_ts_ && row_count_ == rhs.row_count_; }

    bool operator!=(const CreateField &rhs) const { return !(*this == rhs); }
};
#pragma pack()

export struct BlockVersion {
    constexpr static String PATH = "version";

    explicit BlockVersion(SizeT capacity) : deleted_(capacity, 0) {}
    bool operator==(const BlockVersion &rhs) const;
    bool operator!=(const BlockVersion &rhs) const { return !(*this == rhs); };
    i32 GetRowCount(TxnTimeStamp begin_ts);
    void LoadFromFile(const String &version_path);
    void SaveToFile(const String &version_path);

    Vector<CreateField> created_{}; // second field width is same as timestamp, otherwise Valgrind will issue BlockVersion::SaveToFile has
                                    // risk to write uninitialized buffer. (ts, rows)
    Vector<TxnTimeStamp> deleted_{};
};

/// class BlockEntry
export struct BlockEntry : public BaseEntry {
    friend struct TableEntry;
    friend struct SegmentEntry;

public:
    // for iterator unit test
    explicit BlockEntry() : BaseEntry(EntryType::kBlock){};

    // Normal Constructor
    explicit BlockEntry(const SegmentEntry *segment_entry, BlockID block_id, TxnTimeStamp checkpoint_ts);

    static UniquePtr<BlockEntry>
    NewBlockEntry(const SegmentEntry *segment_entry, BlockID block_id, TxnTimeStamp checkpoint_ts, u64 column_count, Txn *txn);

    static UniquePtr<BlockEntry> NewReplayBlockEntry(const SegmentEntry *segment_entry,
                                                     BlockID block_id,
                                                     TxnTimeStamp checkpoint_ts,
                                                     u64 column_count,
                                                     BufferManager *buffer_mgr,
                                                     u16 row_count,
                                                     TxnTimeStamp min_row_ts,
                                                     TxnTimeStamp max_row_ts);

    static UniquePtr<BlockEntry> NewReplayCatalogBlockEntry(const SegmentEntry *segment_entry,
                                                            BlockID block_id,
                                                            u16 row_count,
                                                            u16 row_capacity,
                                                            TxnTimeStamp min_row_ts,
                                                            TxnTimeStamp max_row_ts,
                                                            TxnTimeStamp check_point_ts,
                                                            u16 checkpoint_row_count,
                                                            BufferManager *buffer_mgr);

public:
    // Used in physical import
    void FlushData(i64 checkpoint_row_count);

    nlohmann::json Serialize(TxnTimeStamp max_commit_ts);

    static UniquePtr<BlockEntry> Deserialize(const nlohmann::json &table_entry_json, SegmentEntry *table_entry, BufferManager *buffer_mgr);

    void MergeFrom(BaseEntry &other) override;

    void AppendBlock(const Vector<ColumnVector> &column_vectors, SizeT row_begin, SizeT read_size, BufferManager *buffer_mgr);

protected:
    u16 AppendData(TransactionID txn_id, DataBlock *input_data_block, BlockOffset, u16 append_rows, BufferManager *buffer_mgr);

    void DeleteData(TransactionID txn_id, TxnTimeStamp commit_ts, const Vector<BlockOffset> &rows);

    void CommitAppend(TransactionID txn_id, TxnTimeStamp commit_ts);

    void CommitDelete(TransactionID txn_id, TxnTimeStamp commit_ts);

    void Flush(TxnTimeStamp checkpoint_ts);

    void FlushVersion(BlockVersion &checkpoint_version);

    static SharedPtr<String> DetermineDir(const String &parent_dir, BlockID block_id);

public:
    // Getter
    inline const SegmentEntry *GetSegmentEntry() const { return segment_entry_; }

    inline SizeT row_count() const { return row_count_; }

    inline SizeT row_capacity() const { return row_capacity_; }

    inline TxnTimeStamp min_row_ts() const { return min_row_ts_; }

    inline TxnTimeStamp max_row_ts() const { return max_row_ts_; }

    inline TxnTimeStamp checkpoint_ts() const { return checkpoint_ts_; }

    inline TransactionID using_txn_id() const { return using_txn_id_; }

    inline u16 checkpoint_row_count() const { return checkpoint_row_count_; }

    inline u16 block_id() const { return block_id_; }

    u32 segment_id() const;

    const SharedPtr<String> &base_dir() const { return block_dir_; }

    BlockColumnEntry *GetColumnBlockEntry(SizeT column_id) const { return columns_[column_id].get(); }

    // Get visible range of the BlockEntry since the given row number for a txn
    Pair<u16, u16> GetVisibleRange(TxnTimeStamp begin_ts, BlockOffset block_offset_begin = 0) const;

    bool CheckVisible(BlockOffset block_offset, TxnTimeStamp check_ts) const;

    void SetDeleteBitmask(TxnTimeStamp query_ts, Bitmask &bitmask) const;

    i32 GetAvailableCapacity();

    const String &DirPath() { return *block_dir_; }

    String VersionFilePath() { return LocalFileSystem::ConcatenateFilePath(*block_dir_, BlockVersion::PATH); }

    const SharedPtr<DataType> GetColumnType(u64 column_id) const;

    Vector<UniquePtr<BlockColumnEntry>> &columns() { return columns_; }

public:
    // Setter
    inline void IncreaseRowCount(SizeT increased_row_count) { row_count_ += increased_row_count; }

protected:
    std::shared_mutex rw_locker_{};
    const SegmentEntry *segment_entry_{};

    BlockID block_id_{};
    SharedPtr<String> block_dir_{};

    u16 row_count_{};
    u16 row_capacity_{};

    UniquePtr<BlockVersion> block_version_{};

    TxnTimeStamp min_row_ts_{0};    // Indicate the commit_ts which create this BlockEntry
    TxnTimeStamp max_row_ts_{0};    // Indicate the max commit_ts which create/update/delete data inside this BlockEntry
    TxnTimeStamp checkpoint_ts_{0}; // replay not set

    TransactionID using_txn_id_{0}; // Temporarily used to lock the modification to block entry.

    // checkpoint state
    u16 checkpoint_row_count_{0};

    // Column data
    Vector<UniquePtr<BlockColumnEntry>> columns_{};
};
} // namespace infinity
