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

export module catalog:block_entry;

import :block_column_entry;
import :base_entry;

import stl;
import default_values;
import third_party;
import parser;
import local_file_system;

namespace infinity {



class BufferManager;
class Txn;
struct SegmentEntry;
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

struct BlockEntry : public BaseEntry {
    friend struct TableEntry;
    friend struct SegmentEntry;

public:
    // for iterator unit test
    explicit BlockEntry() : BaseEntry(EntryType::kBlock){};

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

public:
    // Used in physical import
    void FlushData(i64 checkpoint_row_count);

    // Used in block iterator
    inline BlockColumnEntry *GetColumnDataByID(u64 column_id) const { return this->columns_[column_id].get(); }

    static Json Serialize(BlockEntry *segment_entry, TxnTimeStamp max_commit_ts);

    static UniquePtr<BlockEntry> Deserialize(const Json &table_entry_json, SegmentEntry *table_entry, BufferManager *buffer_mgr);

    void MergeFrom(BaseEntry &other) override;

protected:
    u16 AppendData(u64 txn_id, DataBlock *input_data_block, u16 input_block_offset, u16 append_rows, BufferManager *buffer_mgr);

    void DeleteData(u64 txn_id, TxnTimeStamp commit_ts, const Vector<RowID> &rows);

    void CommitAppend(u64 txn_id, TxnTimeStamp commit_ts);

    void CommitDelete(u64 txn_id, TxnTimeStamp commit_ts);

    void Flush(TxnTimeStamp checkpoint_ts);

    void FlushVersion(BlockVersion &checkpoint_version);

    static SharedPtr<String> DetermineDir(const String &parent_dir, u64 block_id);

public:
    // Getter
    inline SizeT row_count() const { return row_count_; }

    inline SizeT row_capacity() const { return row_capacity_; }

    inline TxnTimeStamp min_row_ts() const { return min_row_ts_; }

    inline TxnTimeStamp max_row_ts() const { return max_row_ts_; }

    inline TxnTimeStamp checkpoint_ts() const { return checkpoint_ts_; }

    inline u16 block_id() const { return block_id_; }

    u32 segment_id() const;

    const SharedPtr<String> &base_dir() const { return base_dir_; }

    BlockColumnEntry *GetColumnBlockEntry(SizeT column_id) const { return columns_[column_id].get(); }

    // Get visible range of the BlockEntry since the given row number for a txn
    Pair<u16, u16> GetVisibleRange(TxnTimeStamp begin_ts, u16 block_offset_begin = 0) const;

    i32 GetAvailableCapacity();

    const String &DirPath() { return *base_dir_; }

    String VersionFilePath() { return LocalFileSystem::ConcatenateFilePath(*base_dir_, BlockVersion::PATH); }

    const SharedPtr<DataType> GetColumnType(u64 column_id) const;

public:
    // Setter
    inline void IncreaseRowCount(SizeT increased_row_count) { row_count_ += increased_row_count; }

protected:
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

    u64 using_txn_id_{0}; // Temporarily used to lock the modification to block entry.
    BufferManager *buffer_{nullptr};

    // checkpoint state
    u16 checkpoint_row_count_{0};
};
} // namespace infinity
