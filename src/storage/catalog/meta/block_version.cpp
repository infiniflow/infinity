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

#include <fstream>

module block_version;

import stl;
import infinity_exception;
import logger;
import third_party;
import default_values;
import column_vector;
import serialize;
import local_file_handle;
import status;

namespace infinity {

void CreateField::SaveToFile(LocalFileHandle *file_handle) const {
    Status status = file_handle->Append((char *)(&create_ts_), sizeof(create_ts_));
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    status = file_handle->Append((char *)(&row_count_), sizeof(row_count_));
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
}

CreateField CreateField::LoadFromFile(LocalFileHandle *file_handle) {
    CreateField create_field;
    auto [size1, status1] = file_handle->Read(&create_field.create_ts_, sizeof(create_field.create_ts_));
    if (!status1.ok()) {
        UnrecoverableError(status1.message());
    }
    auto [size2, status2] = file_handle->Read(&create_field.row_count_, sizeof(create_field.row_count_));
    if (!status2.ok()) {
        UnrecoverableError(status2.message());
    }
    return create_field;
}

bool BlockVersion::operator==(const BlockVersion &rhs) const {
    std::shared_lock<std::shared_mutex> lock_created(rw_mutex_);
    if (this->created_.size() != rhs.created_.size() || this->deleted_.size() != rhs.deleted_.size())
        return false;
    for (SizeT i = 0; i < this->created_.size(); i++) {
        if (this->created_[i] != rhs.created_[i])
            return false;
    }
    for (SizeT i = 0; i < this->deleted_.size(); i++) {
        if (this->deleted_[i] != rhs.deleted_[i])
            return false;
    }
    return true;
}

Pair<BlockOffset, i32> BlockVersion::GetCommitRowCount(TxnTimeStamp commit_ts) const {
    if (commit_ts == MAX_TIMESTAMP) {
        return {};
    }
    std::shared_lock<std::shared_mutex> lock(rw_mutex_);
    auto iter = std::lower_bound(created_.begin(), created_.end(), commit_ts, [](const CreateField &field, TxnTimeStamp ts) { return field.create_ts_ < ts; });
    if (iter == created_.end() || iter->create_ts_ != commit_ts) {
        return {};
    }
    i64 total_cnt = iter->row_count_;
    if (iter == created_.begin()) {
        return {0, total_cnt};
    }
    --iter;
    return {iter->row_count_, total_cnt - iter->row_count_};
}

i32 BlockVersion::GetRowCount(TxnTimeStamp begin_ts) const {
    // use binary search find the last create_field that has create_ts_ <= check_ts
    std::shared_lock<std::shared_mutex> lock(rw_mutex_);
    auto iter =
        std::upper_bound(created_.begin(), created_.end(), begin_ts, [](TxnTimeStamp ts, const CreateField &field) { return ts < field.create_ts_; });
    if (iter == created_.begin()) {
        return 0;
    }
    --iter;
    return iter->row_count_;
}

i64 BlockVersion::GetRowCount() const {
    std::shared_lock<std::shared_mutex> lock(rw_mutex_);
    i64 row_count = created_.empty() ? 0 : created_.back().row_count_;
    return row_count;
}

Tuple<i32, Status> BlockVersion::GetRowCountForUpdate(TxnTimeStamp begin_ts) const {
    // check read-write conflict
    std::shared_lock<std::shared_mutex> lock(rw_mutex_);
    if (!created_.empty() && created_.back().create_ts_ >= begin_ts) {
        return {0, Status::TxnConflict(0, fmt::format("Append conflict, begin_ts: {}, last_create_ts: {}", begin_ts, created_.back().create_ts_))};
    }
    i32 row_count = created_.empty() ? 0 : created_.back().row_count_;
    return {row_count, Status::OK()};
}

bool BlockVersion::SaveToFile(TxnTimeStamp checkpoint_ts, LocalFileHandle &file_handle) const {
    std::unique_lock<std::shared_mutex> lock(rw_mutex_);
    BlockOffset create_size = created_.size();
    while (create_size > 0 && created_[create_size - 1].create_ts_ > checkpoint_ts) {
        --create_size;
    }

    file_handle.Append(&create_size, sizeof(create_size));
    for (SizeT j = 0; j < create_size; ++j) {
        created_[j].SaveToFile(&file_handle);
    }

    BlockOffset capacity = deleted_.size();
    file_handle.Append(&capacity, sizeof(capacity));
    TxnTimeStamp dump_ts = 0;
    u32 deleted_row_count = 0;
    for (const auto &ts : deleted_) {
        if (ts <= checkpoint_ts) {
            ++deleted_row_count;
            file_handle.Append(&ts, sizeof(ts));
        } else {
            file_handle.Append(&dump_ts, sizeof(dump_ts));
        }
    }
    
    // Check if the version file is full by calculating total row count from created data
    i64 total_row_count = 0;
    for (SizeT j = 0; j < create_size; ++j) {
        total_row_count += created_[j].row_count_;
    }

    bool is_full = total_row_count >= DEFAULT_BLOCK_CAPACITY;
    
    LOG_TRACE(fmt::format("Flush block version, ckp ts: {}, write create: {}, delete {}, total_rows: {}, is_full: {}", 
                          checkpoint_ts, create_size, deleted_row_count, total_row_count, is_full));
    
    return is_full;
}

void BlockVersion::SpillToFile(LocalFileHandle *file_handle) const {
    std::unique_lock<std::shared_mutex> lock(rw_mutex_);
    BlockOffset create_size = created_.size();
    Status status = file_handle->Append(&create_size, sizeof(create_size));
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    for (const auto &create : created_) {
        create.SaveToFile(file_handle);
    }

    BlockOffset capacity = deleted_.size();
    status = file_handle->Append(&capacity, sizeof(capacity));
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    status = file_handle->Append(deleted_.data(), capacity * sizeof(TxnTimeStamp));
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
}

UniquePtr<BlockVersion> BlockVersion::LoadFromFile(LocalFileHandle *file_handle) {
    auto block_version = MakeUnique<BlockVersion>();

    BlockOffset create_size;
    file_handle->Read(&create_size, sizeof(create_size));
    block_version->created_.reserve(create_size);
    for (BlockOffset i = 0; i < create_size; i++) {
        block_version->created_.push_back(CreateField::LoadFromFile(file_handle));
    }
    LOG_TRACE(fmt::format("BlockVersion::LoadFromFile version, created: {}", create_size));
    BlockOffset capacity;
    file_handle->Read(&capacity, sizeof(capacity));
    block_version->deleted_.resize(capacity);
    for (BlockOffset i = 0; i < capacity; i++) {
        file_handle->Read(&block_version->deleted_[i], sizeof(TxnTimeStamp));
    }
    return block_version;
}

void BlockVersion::GetCreateTS(SizeT offset, SizeT size, ColumnVector &res) const {
    // find the first create_field that has row_count_ >= offset
    std::shared_lock<std::shared_mutex> lock(rw_mutex_);
    auto iter = std::lower_bound(created_.begin(), created_.end(), static_cast<i64>(offset), [](const CreateField &field, const i64 offset_cp) {
        return field.row_count_ < offset_cp;
    });
    SizeT i = 0;
    for (; i < size; ++i) {
        if (iter == created_.end()) {
            break;
        }
        res.AppendByPtr(reinterpret_cast<const char *>(&iter->create_ts_));
        if (iter->row_count_ == (i64)(i + offset)) {
            ++iter;
        }
    }
    TxnTimeStamp empty_ts = MAX_TIMESTAMP;
    for (; i < size; ++i) {
        res.AppendByPtr(reinterpret_cast<const char *>(&empty_ts));
    }
}

void BlockVersion::GetDeleteTS(SizeT offset, SizeT size, ColumnVector &res) const {
    std::shared_lock<std::shared_mutex> lock(rw_mutex_);
    for (SizeT i = offset; i < offset + size; ++i) {
        res.AppendByPtr(reinterpret_cast<const char *>(&deleted_[i]));
    }
}

void BlockVersion::Append(TxnTimeStamp commit_ts, i32 row_count) {
    std::unique_lock<std::shared_mutex> lock(rw_mutex_);
    created_.emplace_back(commit_ts, row_count);
    latest_change_ts_ = commit_ts;
}

void BlockVersion::CommitAppend(TxnTimeStamp save_ts, TxnTimeStamp commit_ts) {
    std::unique_lock<std::shared_mutex> lock(rw_mutex_);
    for (auto &[create_ts, row_count] : created_) {
        if (create_ts == save_ts) {
            create_ts = commit_ts;
            break;
        }
    }
}

Status BlockVersion::Delete(i32 offset, TxnTimeStamp commit_ts) {
    std::unique_lock<std::shared_mutex> lock(rw_mutex_);
    if (deleted_[offset] != 0) {
        return Status::TxnWWConflict(fmt::format("Delete twice at offset: {}, commit_ts: {}, old_ts: {}", offset, commit_ts, deleted_[offset]));
    }
    deleted_[offset] = commit_ts;
    latest_change_ts_ = commit_ts;
    return Status::OK();
}

void BlockVersion::RollbackDelete(i32 offset) {
    std::unique_lock<std::shared_mutex> lock(rw_mutex_);
    deleted_[offset] = 0;
} // FIXME latest_change_ts_ ?

bool BlockVersion::CheckDelete(i32 offset, TxnTimeStamp check_ts) const {
    std::shared_lock<std::shared_mutex> lock(rw_mutex_);
    if (SizeT(offset) >= deleted_.size()) {
        return false;
    }
    return deleted_[offset] != 0 && deleted_[offset] <= check_ts;
}

Status BlockVersion::Print(TxnTimeStamp begin_ts, i32 offset, bool ignore_invisible) {
    i32 row_count = 0;
    std::shared_lock<std::shared_mutex> lock_created(rw_mutex_);
    for (const auto &created_range : created_) {
        if (offset < row_count + created_range.row_count_) {
            if (ignore_invisible) {
                if (begin_ts < created_range.create_ts_) {
                    // Can't see the record
                    ;
                } else if (begin_ts >= created_range.create_ts_ && (begin_ts < deleted_[offset] or deleted_[offset] == 0)) {
                    LOG_INFO(fmt::format("Row {} created: {}", offset, created_range.create_ts_));
                } else if (begin_ts >= deleted_[offset]) {
                    // Can't see the record
                    ;
                } else {
                    return Status::UnexpectedError(
                        fmt::format("BeginTS is invalid: {}, create_ts: {}, delete_ts: {}", begin_ts, created_range.create_ts_, deleted_[offset]));
                }
            } else {
                LOG_INFO(fmt::format("Row {}, created: {}, deleted: {}", offset, created_range.create_ts_, deleted_[offset]));
            }
            return Status::OK();
        }
        row_count += created_range.row_count_;
    }
    return Status::UnexpectedError(fmt::format("Offset {} out of range", offset));
}

} // namespace infinity