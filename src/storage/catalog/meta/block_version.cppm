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

std::atomic_int cnt{};

export struct BlockVersion {
    using segment_manager = boost::interprocess::managed_mapped_file::segment_manager;
    using TxnTimeStamp_allocator = boost::interprocess::allocator<TxnTimeStamp, segment_manager>;
    using CreateField_allocator = boost::interprocess::allocator<CreateField, segment_manager>;
    constexpr static std::string_view PATH = "version";

    static std::shared_ptr<std::string> FileName() { return std::make_shared<std::string>(PATH); }

    // explicit BlockVersion(size_t capacity) : deleted_(capacity) {}
    explicit BlockVersion(size_t capacity, segment_manager *sm)
        : deleted_(capacity, TxnTimeStamp_allocator(sm)), created_(CreateField_allocator(sm)) {}
    BlockVersion() = default;

    BlockVersion(const BlockVersion &other) : rw_mutex_(), deleted_(other.deleted_), created_(other.created_) {}

    BlockVersion(BlockVersion &&other) noexcept : rw_mutex_(), deleted_(std::move(other.deleted_)), created_(std::move(other.created_)) {}

    // BlockVersion &operator=(const BlockVersion &other) {
    //     // if (this != &other) {
    //     deleted_ = other.deleted_;
    //     created_ = other.created_;
    //     // }
    //     return *this;
    // }
    //
    // BlockVersion &operator=(BlockVersion &&other) noexcept {
    //     // if (this != &other) {
    //     deleted_ = std::move(other.deleted_);
    //     // }
    //     return *this;
    // }

    bool operator==(const BlockVersion &rhs) const;
    bool operator!=(const BlockVersion &rhs) const { return !(*this == rhs); };

    std::pair<BlockOffset, i32> GetCommitRowCount(TxnTimeStamp commit_ts) const;
    i32 GetRowCount(TxnTimeStamp begin_ts) const;
    i64 GetRowCount() const;
    bool SaveToFile(void *&mmap, size_t &mmap_size, const std::string &rel_path, TxnTimeStamp checkpoint_ts, LocalFileHandle &file_handler);
    bool SaveToFile(TxnTimeStamp checkpoint_ts, LocalFileHandle &file_handle) const;

    static void LoadFromFile(LocalFileHandle *file_handle, BlockVersion *&block_version);
    static void LoadFromFile(std::shared_ptr<BlockVersion> &data, size_t &mmap_size, void *&mmap, LocalFileHandle *file_handle);

    void GetCreateTS(size_t offset, size_t size, ColumnVector &res) const;

    void GetDeleteTS(size_t offset, size_t size, ColumnVector &res) const;

    void Append(TxnTimeStamp commit_ts, i32 row_count);

    void CommitAppend(TxnTimeStamp save_ts, TxnTimeStamp commit_ts);

    Status Delete(i32 offset, TxnTimeStamp commit_ts);

    void RollbackDelete(i32 offset);

    void RestoreFromSnapshot(TxnTimeStamp commit_ts); // restore from snapshot, only used by block meta

    bool CheckDelete(i32 offset, TxnTimeStamp check_ts) const;

    Status Print(TxnTimeStamp commit_ts, i32 offset, bool ignore_invisible);

private:
    // mutable std::shared_mutex rw_mutex_;
    mutable boost::interprocess::interprocess_sharable_mutex rw_mutex_;

    boost::interprocess::vector<TxnTimeStamp, boost::interprocess::allocator<TxnTimeStamp, segment_manager>> deleted_;

    boost::interprocess::vector<CreateField, boost::interprocess::allocator<CreateField, segment_manager>>
        created_; // second field width is same as timestamp, otherwise Valgrind will issue BlockVersion::SaveToFile has
    // risk to write uninitialized buffer. (ts, rows)
};

} // namespace infinity
