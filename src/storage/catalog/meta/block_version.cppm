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

export module infinity_core:block_version;

import :stl;
import :local_file_handle;
import :status;

namespace infinity {

struct ColumnVector;

struct CreateField {
    //    CreateField(TxnTimeStamp create_ts, i32 row_count) : create_ts_(create_ts), row_count_(row_count) {}

    TxnTimeStamp create_ts_{};
    i64 row_count_{};

    bool operator==(const CreateField &rhs) const { return create_ts_ == rhs.create_ts_ && row_count_ == rhs.row_count_; }

    bool operator!=(const CreateField &rhs) const { return !(*this == rhs); }

    void SaveToFile(LocalFileHandle *file_handle) const;
    static CreateField LoadFromFile(LocalFileHandle *file_handle);
};

export struct BlockVersion {
    constexpr static std::string_view PATH = "version";

    static SharedPtr<String> FileName() { return MakeShared<String>(PATH); }

    explicit BlockVersion(SizeT capacity) : deleted_(capacity, 0) {}
    BlockVersion() = default;

    bool operator==(const BlockVersion &rhs) const;
    bool operator!=(const BlockVersion &rhs) const { return !(*this == rhs); };

    Pair<BlockOffset, i32> GetCommitRowCount(TxnTimeStamp commit_ts) const;
    i32 GetRowCount(TxnTimeStamp begin_ts) const;
    i64 GetRowCount() const;

    Tuple<i32, Status> GetRowCountForUpdate(TxnTimeStamp begin_ts) const;

    bool SaveToFile(TxnTimeStamp checkpoint_ts, LocalFileHandle &file_handler) const;

    void SpillToFile(LocalFileHandle *file_handle) const;
    static UniquePtr<BlockVersion> LoadFromFile(LocalFileHandle *file_handle);

    void GetCreateTS(SizeT offset, SizeT size, ColumnVector &res) const;

    void GetDeleteTS(SizeT offset, SizeT size, ColumnVector &res) const;

    void Append(TxnTimeStamp commit_ts, i32 row_count);

    void CommitAppend(TxnTimeStamp save_ts, TxnTimeStamp commit_ts);

    Status Delete(i32 offset, TxnTimeStamp commit_ts);

    void RollbackDelete(i32 offset);

    bool CheckDelete(i32 offset, TxnTimeStamp check_ts) const;

    Status Print(TxnTimeStamp commit_ts, i32 offset, bool ignore_invisible);

    TxnTimeStamp latest_change_ts() const {
        std::shared_lock<std::shared_mutex> lock(rw_mutex_);
        return latest_change_ts_;
    }

private:
    mutable std::shared_mutex rw_mutex_{};
    Vector<CreateField> created_{}; // second field width is same as timestamp, otherwise Valgrind will issue BlockVersion::SaveToFile has
                                    // risk to write uninitialized buffer. (ts, rows)
    Vector<TxnTimeStamp> deleted_{};

    TxnTimeStamp latest_change_ts_{}; // used by checkpoint to decide if the version file need to be flushed or not.
};

} // namespace infinity
