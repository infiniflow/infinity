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

export module infinity_core:block_version;

import :local_file_handle;
import :status;
import :boost;

namespace infinity {

export struct ColumnVector;

struct CreateField {
    TxnTimeStamp create_ts_{};
    i64 row_count_{};

    bool operator==(const CreateField &rhs) const { return create_ts_ == rhs.create_ts_ && row_count_ == rhs.row_count_; }

    bool operator!=(const CreateField &rhs) const { return !(*this == rhs); }

    void SaveToFile(LocalFileHandle *file_handle) const;
    static CreateField LoadFromFile(LocalFileHandle *file_handle);
};

export struct BlockVersion {
    using segment_manager_t = boost::interprocess::managed_mapped_file::segment_manager;
    using void_allocator = boost::interprocess::allocator<void, segment_manager_t>;

    using TxnTimeStampAllocator = boost::interprocess::allocator<TxnTimeStamp, segment_manager_t>;
    using ShmemTxnTimeStampVector = boost::interprocess::vector<TxnTimeStamp, TxnTimeStampAllocator>;

    using CreateFieldAllocator = boost::interprocess::allocator<CreateField, segment_manager_t>;
    using ShmemCreateFieldVector = boost::interprocess::vector<CreateField, CreateFieldAllocator>;

    using StringAllocator = boost::interprocess::allocator<char, segment_manager_t>;
    using ShmemString = boost::container::basic_string<char, std::char_traits<char>, StringAllocator>;

    constexpr static std::string_view PATH = "version";

    static std::shared_ptr<std::string> FileName() { return std::make_shared<std::string>(PATH); }

    // explicit BlockVersion(size_t capacity) : deleted_(capacity) {}
    explicit BlockVersion(size_t capacity, const void_allocator &alloc_inst)
        : // path_(path, alloc_inst),
          deleted_(capacity, alloc_inst), created_(alloc_inst) {}
    BlockVersion() = default;

    // BlockVersion(const BlockVersion &other);
    // BlockVersion &operator=(const BlockVersion &other);
    // BlockVersion(BlockVersion &&other) noexcept;
    // BlockVersion &operator=(BlockVersion &&other) noexcept;

    bool operator==(const BlockVersion &rhs) const;
    bool operator!=(const BlockVersion &rhs) const { return !(*this == rhs); };

    std::pair<BlockOffset, i32> GetCommitRowCount(TxnTimeStamp commit_ts) const;
    i32 GetRowCount(TxnTimeStamp begin_ts) const;
    i64 GetRowCount() const;
    void SaveToFile(TxnTimeStamp checkpoint_ts, LocalFileHandle &file_handle) const;
    void FixByCheckpointTs(TxnTimeStamp checkpoint_ts);

    static void LoadFromFile(LocalFileHandle *file_handle, BlockVersion *&block_version);
    // static void LoadFromFile(std::shared_ptr<BlockVersion> &data, size_t &mmap_size, void *&mmap, LocalFileHandle *file_handle);

    void GetCreateTS(size_t offset, size_t size, ColumnVector &res) const;

    void GetDeleteTS(size_t offset, size_t size, ColumnVector &res) const;

    void Append(TxnTimeStamp commit_ts, i32 row_count);

    void CommitAppend(TxnTimeStamp save_ts, TxnTimeStamp commit_ts);

    Status Delete(i32 offset, TxnTimeStamp commit_ts);

    void RollbackDelete(i32 offset);

    void RestoreFromSnapshot(TxnTimeStamp commit_ts); // restore from snapshot, only used by block meta

    bool CheckDelete(i32 offset, TxnTimeStamp check_ts) const;

    Status Print(TxnTimeStamp commit_ts, i32 offset, bool ignore_invisible);

    ShmemTxnTimeStampVector DeletedVec();
    ShmemCreateFieldVector CreatedVec();

    void Import(ShmemTxnTimeStampVector &deleted, ShmemCreateFieldVector &created);

private:
    // mutable std::shared_mutex rw_mutex_;
    mutable boost::interprocess::interprocess_sharable_mutex rw_mutex_; // offset_ptr

    // ShmemString path_;

    ShmemTxnTimeStampVector deleted_;

    ShmemCreateFieldVector created_; // second field width is same as timestamp, otherwise Valgrind will issue BlockVersion::SaveToFile has
    // risk to write uninitialized buffer. (ts, rows)
};

} // namespace infinity
