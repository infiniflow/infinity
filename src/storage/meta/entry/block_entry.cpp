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

#include <filesystem>
#include <memory>

module catalog;

import stl;
import default_values;
import logger;
import third_party;
import defer_op;
import local_file_system;
import serialize;

import infinity_exception;
import parser;


namespace infinity {

bool BlockVersion::operator==(const BlockVersion &rhs) const {
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

i32 BlockVersion::GetRowCount(TxnTimeStamp begin_ts) {
    if (created_.empty())
        return 0;
    i64 idx = created_.size() - 1;
    for (; idx >= 0; idx--) {
        if (created_[idx].create_ts_ <= begin_ts)
            break;
    }
    if (idx < 0)
        return 0;
    return created_[idx].row_count_;
}

void BlockVersion::LoadFromFile(const String &version_path) {
    std::ifstream ifs(version_path);
    if (!ifs.is_open()) {
        LOG_WARN(Format("Failed to open block_version file: {}", version_path));
        // load the block_version file not exist return and create version
        return;
    }
    int buf_len = std::filesystem::file_size(version_path);
    Vector<char> buf(buf_len);
    ifs.read(buf.data(), buf_len);
    ifs.close();
    char *ptr = buf.data();
    int32_t created_size = ReadBufAdv<int32_t>(ptr);
    int32_t deleted_size = ReadBufAdv<int32_t>(ptr);
    created_.resize(created_size);
    deleted_.resize(deleted_size);
    Memcpy(created_.data(), ptr, created_size * sizeof(CreateField));
    ptr += created_size * sizeof(CreateField);
    Memcpy(deleted_.data(), ptr, deleted_size * sizeof(TxnTimeStamp));
    ptr += deleted_.size() * sizeof(TxnTimeStamp);
    if (ptr - buf.data() != buf_len) {
        Error<StorageException>("Failed to load block_version file: " + version_path);
    }
}

void BlockVersion::SaveToFile(const String &version_path) {
    int32_t exp_size = sizeof(int32_t) + created_.size() * sizeof(CreateField);
    exp_size += sizeof(int32_t) + deleted_.size() * sizeof(TxnTimeStamp);
    Vector<char> buf(exp_size, 0);
    char *ptr = buf.data();
    WriteBufAdv<int32_t>(ptr, int32_t(created_.size()));
    WriteBufAdv<int32_t>(ptr, int32_t(deleted_.size()));
    Memcpy(ptr, created_.data(), created_.size() * sizeof(CreateField));
    ptr += created_.size() * sizeof(CreateField);
    Memcpy(ptr, deleted_.data(), deleted_.size() * sizeof(TxnTimeStamp));
    ptr += deleted_.size() * sizeof(TxnTimeStamp);
    if (ptr - buf.data() != exp_size) {
        Error<StorageException>("Failed to save block_version file: " + version_path);
    }
    std::ofstream ofs = std::ofstream(version_path, std::ios::trunc | std::ios::binary);
    if (!ofs.is_open()) {
        Error<StorageException>("Failed to open block_version file: " + version_path);
    }
    ofs.write(buf.data(), ptr - buf.data());
    ofs.flush();
    ofs.close();
}

BlockEntry::BlockEntry(const SegmentEntry *segment_entry, u16 block_id, TxnTimeStamp checkpoint_ts, u64 column_count, BufferManager *buffer_mgr)
    : BaseEntry(EntryType::kBlock), segment_entry_(segment_entry), block_id_(block_id), row_count_(0), row_capacity_(DEFAULT_VECTOR_SIZE),
      checkpoint_ts_(checkpoint_ts) {
    base_dir_ = BlockEntry::DetermineDir(*segment_entry->segment_dir(), block_id);
    columns_.reserve(column_count);
    for (SizeT column_id = 0; column_id < column_count; ++column_id) {
        columns_.emplace_back(BlockColumnEntry::MakeNewBlockColumnEntry(this, column_id, buffer_mgr));
    }

    block_version_ = MakeUnique<BlockVersion>(row_capacity_);
}

BlockEntry::BlockEntry(const SegmentEntry *segment_entry,
                       u16 block_id,
                       TxnTimeStamp checkpoint_ts,
                       u64 column_count,
                       BufferManager *buffer_mgr,
                       u16 row_count_,
                       i16 min_row_ts_,
                       i16 max_row_ts_)
    : BaseEntry(EntryType::kBlock), segment_entry_(segment_entry), block_id_(block_id), row_count_(row_count_), row_capacity_(DEFAULT_VECTOR_SIZE),
      min_row_ts_(min_row_ts_), max_row_ts_(max_row_ts_), checkpoint_ts_(checkpoint_ts) {

    base_dir_ = BlockEntry::DetermineDir(*segment_entry->segment_dir(), block_id);

    columns_.reserve(column_count);
    for (SizeT column_id = 0; column_id < column_count; ++column_id) {
        // For replay purposes not create a new column buffer handler
        columns_.emplace_back(BlockColumnEntry::MakeNewBlockColumnEntry(this, column_id, buffer_mgr, true));
    }

    block_version_ = MakeUnique<BlockVersion>(row_capacity_);
    block_version_->created_.emplace_back((TxnTimeStamp)min_row_ts_, (i32)row_count_);
}

Pair<u16, u16> BlockEntry::GetVisibleRange(TxnTimeStamp begin_ts, u16 block_offset_begin) const {
    auto &block_version = this->block_version_;
    auto &deleted = block_version->deleted_;
    u16 block_offset_end = block_version->GetRowCount(begin_ts);
    while (block_offset_begin < block_offset_end && deleted[block_offset_begin] != 0 && deleted[block_offset_begin] <= begin_ts) {
        block_offset_begin++;
    }
    u16 row_idx;
    for (row_idx = block_offset_begin; row_idx < block_offset_end; ++row_idx) {
        if (deleted[row_idx] != 0 && deleted[row_idx] <= begin_ts) {
            break;
        }
    }
    return {block_offset_begin, row_idx};
}

u16 BlockEntry::AppendData(u64 txn_id,
                           DataBlock *input_data_block,
                           u16 input_block_offset,
                           u16 append_rows,
                           BufferManager *) {
    UniqueLock<RWMutex> lck(this->rw_locker_);
    if (this->using_txn_id_ != 0 && this->using_txn_id_ != txn_id) {
        Error<StorageException>(Format("Multiple transactions are changing data of Segment: {}, Block: {}",
                                       this->segment_entry_->segment_id(),
                                       this->block_id_));
    }

    this->using_txn_id_ = txn_id;
    u16 actual_copied = append_rows;
    if (this->row_count_ + append_rows > this->row_capacity_) {
        actual_copied = this->row_capacity_ - this->row_count_;
    }

    SizeT column_count = this->columns_.size();
    for (SizeT column_id = 0; column_id < column_count; ++column_id) {
        BlockColumnEntry::Append(this->columns_[column_id].get(),
                                 this->row_count_,
                                 input_data_block->column_vectors[column_id].get(),
                                 input_block_offset,
                                 actual_copied);

        LOG_TRACE(Format("Segment: {}, Block: {}, Column: {} is appended with {} rows",
                         this->segment_entry_->segment_id(),
                         this->block_id_,
                         column_id,
                         actual_copied));
    }

    this->row_count_ += actual_copied;
    return actual_copied;
}

void BlockEntry::DeleteData(u64 txn_id, TxnTimeStamp commit_ts, const Vector<RowID> &rows) {
    UniqueLock<RWMutex> lck(this->rw_locker_);
    if (this->using_txn_id_ != 0 && this->using_txn_id_ != txn_id) {
        Error<StorageException>(Format("Multiple transactions are changing data of Segment: {}, Block: {}",
                                       this->segment_entry_->segment_id(),
                                       this->block_id_));
    }

    this->using_txn_id_ = txn_id;

    u32 segment_id = this->segment_entry_->segment_id();
    u16 block_id = this->block_id_;

    auto &block_version = this->block_version_;
    for (RowID row_id : rows) {
        u16 block_offset = row_id.segment_offset_ % DEFAULT_BLOCK_CAPACITY;
        block_version->deleted_[block_offset] = commit_ts;
    }

    LOG_TRACE(Format("Segment {} Block {} has deleted {} rows", segment_id, block_id, rows.size()));
}

// A txn may invoke AppendData() multiple times, and then invoke CommitAppend() once.
void BlockEntry::CommitAppend(u64 txn_id, TxnTimeStamp commit_ts) {
    UniqueLock<RWMutex> lck(this->rw_locker_);
    if (this->using_txn_id_ != txn_id) {
        Error<StorageException>(Format("Multiple transactions are changing data of Segment: {}, Block: {}",
                                       this->segment_entry_->segment_id(),
                                       this->block_id_));
    }
    this->using_txn_id_ = 0;
    if (this->min_row_ts_ == 0) {
        this->min_row_ts_ = commit_ts;
    }
    this->max_row_ts_ = Max(this->max_row_ts_, commit_ts);

    auto &block_version = this->block_version_;
    block_version->created_.push_back({commit_ts, int32_t(this->row_count_)});
}

void BlockEntry::CommitDelete(u64 txn_id, TxnTimeStamp commit_ts) {
    UniqueLock<RWMutex> lck(this->rw_locker_);
    if (this->using_txn_id_ != 0 && this->using_txn_id_ != txn_id) {
        Error<StorageException>(Format("Multiple transactions are changing data of Segment: {}, Block: {}",
                                       this->segment_entry_->segment_id(),
                                       this->block_id_));
    }

    if (this->using_txn_id_ == 0) {
        // Which case will come here?
        return;
    }

    this->using_txn_id_ = 0;
    if (this->min_row_ts_ == 0) {
        this->min_row_ts_ = commit_ts;
    }
    this->max_row_ts_ = Max(this->max_row_ts_, commit_ts);
}

void BlockEntry::FlushData(int64_t checkpoint_row_count) {
    SizeT column_count = this->columns_.size();
    SizeT column_idx = 0;
    while (column_idx < column_count) {
        BlockColumnEntry *block_column_entry = this->columns_[column_idx].get();
        BlockColumnEntry::Flush(block_column_entry, checkpoint_row_count);
        LOG_TRACE(Format("ColumnData {} is flushed", block_column_entry->column_id()));
        ++column_idx;
    }
}

void BlockEntry::FlushVersion(BlockVersion &checkpoint_version) {
    checkpoint_version.SaveToFile(this->VersionFilePath());
}

void BlockEntry::Flush(TxnTimeStamp checkpoint_ts) {
    LOG_TRACE(Format("Segment: {}, Block: {} is being flushing", this->segment_entry_->segment_id(), this->block_id_));
    if (checkpoint_ts < this->checkpoint_ts_) {
        Error<StorageException>(
            Format("BlockEntry checkpoint_ts skew! checkpoint_ts: {}, this->checkpoint_ts_: {}", checkpoint_ts, this->checkpoint_ts_));
    }
    int checkpoint_row_count = 0;

    BlockVersion checkpoint_version(this->block_version_->deleted_.size());
    {
        SharedLock<RWMutex> lock(this->rw_locker_);
        // Skip if entry has been flushed at some previous checkpoint, or is invisible at current checkpoint.
        if (this->max_row_ts_ <= this->checkpoint_ts_ || this->min_row_ts_ > checkpoint_ts)
            return;

        checkpoint_row_count = this->block_version_->GetRowCount(checkpoint_ts);
        if (checkpoint_row_count == 0) {
            Error<StorageException>("BlockEntry is empty at checkpoint_ts!");
        }
        const Vector<TxnTimeStamp> &deleted = this->block_version_->deleted_;
        if (checkpoint_row_count <= this->checkpoint_row_count_) {
            // BlockEntry doesn't append rows between the previous checkpoint and checkpoint_ts.
            bool deleted_between = false;
            for (int i = 0; i < checkpoint_row_count; i++) {
                if (deleted[i] > this->checkpoint_ts_ && deleted[i] <= checkpoint_ts) {
                    deleted_between = true;
                    break;
                }
            }
            if (!deleted_between) // BlockEntry doesn't change between the previous checkpoint and checkpoint_ts.
                return;
        }
        checkpoint_version.created_ = this->block_version_->created_;
        checkpoint_version.deleted_.reserve(checkpoint_row_count);
        Memcpy(checkpoint_version.deleted_.data(), deleted.data(), checkpoint_row_count * sizeof(TxnTimeStamp));
    }
    for (int i = 0; i < checkpoint_row_count; i++) {
        if (checkpoint_version.deleted_[i] > checkpoint_ts) {
            checkpoint_version.deleted_[i] = 0;
        }
    }

    FlushVersion(checkpoint_version);
    FlushData(checkpoint_row_count);
    this->checkpoint_ts_ = checkpoint_ts;
    this->checkpoint_row_count_ = checkpoint_row_count;
    LOG_TRACE(Format("Segment: {}, Block {} is flushed", this->segment_entry_->segment_id(), this->block_id_));
    return;
}

// TODO: introduce BlockColumnMeta
Json BlockEntry::Serialize(BlockEntry *block_entry, TxnTimeStamp) {
    Json json_res;
    SharedLock<RWMutex> lck(block_entry->rw_locker_);

    json_res["block_id"] = block_entry->block_id_;
    json_res["checkpoint_ts"] = block_entry->checkpoint_ts_;
    json_res["row_count"] = block_entry->checkpoint_row_count_;
    json_res["row_capacity"] = block_entry->row_capacity_;
    json_res["block_dir"] = *block_entry->base_dir_;
    for (const auto &block_column_entry : block_entry->columns_) {
        json_res["columns"].emplace_back(BlockColumnEntry::Serialize(block_column_entry.get()));
    }
    json_res["min_row_ts"] = block_entry->min_row_ts_;
    json_res["max_row_ts"] = block_entry->checkpoint_ts_;
    return json_res;
}

UniquePtr<BlockEntry> BlockEntry::Deserialize(const Json &block_entry_json, SegmentEntry *segment_entry, BufferManager *buffer_mgr) {
    u64 block_id = block_entry_json["block_id"];
    TxnTimeStamp checkpoint_ts = block_entry_json["checkpoint_ts"];
    UniquePtr<BlockEntry> block_entry = MakeUnique<BlockEntry>(segment_entry, block_id, checkpoint_ts, 0, buffer_mgr);

    *block_entry->base_dir_ = block_entry_json["block_dir"];
    block_entry->row_capacity_ = block_entry_json["row_capacity"];
    block_entry->row_count_ = block_entry_json["row_count"];
    block_entry->min_row_ts_ = block_entry_json["min_row_ts"];
    block_entry->max_row_ts_ = block_entry_json["max_row_ts"];
    block_entry->checkpoint_row_count_ = block_entry->row_count_;

    for (const auto &block_column_json : block_entry_json["columns"]) {
        block_entry->columns_.emplace_back(BlockColumnEntry::Deserialize(block_column_json, block_entry.get(), buffer_mgr));
    }
    block_entry->block_version_->LoadFromFile(block_entry->VersionFilePath());
    // if not found version file
    if (block_entry->block_version_->created_.empty()) {
        block_entry->block_version_->created_.emplace_back(block_entry->max_row_ts_, block_entry->row_count_);
        // FIXME Need to flush the version file?
        // FlushVersion(block_entry.get(), *block_entry->block_version_);
    }

    return block_entry;
}

i32 BlockEntry::GetAvailableCapacity() {
    SharedLock<RWMutex> lck(this->rw_locker_);
    return this->row_capacity_ - this->row_count_;
}

SharedPtr<String> BlockEntry::DetermineDir(const String &parent_dir, u64 block_id) {
    LocalFileSystem fs;
    SharedPtr<String> base_dir;
    base_dir = MakeShared<String>(Format("{}/blk_{}", parent_dir, block_id));
    fs.CreateDirectoryNoExp(*base_dir);
    return base_dir;
}

void BlockEntry::MergeFrom(BaseEntry &other) {
    auto block_entry2 = dynamic_cast<BlockEntry *>(&other);
    if (block_entry2 == nullptr) {
        Error<StorageException>("MergeFrom requires the same type of BaseEntry");
    }
    // // No locking here since only the load stage needs MergeFrom.
    if (*this->base_dir_ != *block_entry2->base_dir_) {
        Error<StorageException>("BlockEntry::MergeFrom requires base_dir_ match");
    }
    if (this->block_id_ != block_entry2->block_id_) {
        Error<StorageException>("BlockEntry::MergeFrom requires block_id_ match");
    }
    if (this->row_capacity_ != block_entry2->row_capacity_) {
        Error<StorageException>("BlockEntry::MergeFrom requires row_capacity_ match");
    }
    if (this->min_row_ts_ != block_entry2->min_row_ts_) {
        Error<StorageException>("BlockEntry::MergeFrom requires min_row_ts_ match");
    }
    if (this->row_count_ > block_entry2->row_count_) {
        Error<StorageException>("BlockEntry::MergeFrom requires source block entry rows not more than target block entry rows");
    }
    if (this->checkpoint_ts_ > block_entry2->checkpoint_ts_) {
        Error<StorageException>(
            "BlockEntry::MergeFrom requires source block entry checkpoint timestamp not more than target block entry checkpoint timestamp");
    }
    if (this->checkpoint_row_count_ > block_entry2->checkpoint_row_count_) {
        Error<StorageException>(
            "BlockEntry::MergeFrom requires source block entry checkpoint row count not more than target block entry checkpoint row count");
    }
    if (columns_.size() > block_entry2->columns_.size()) {
        Error<StorageException>("BlockEntry::MergeFrom: Attempt to merge two block entries with difference column count.");
    }

    if (this->checkpoint_ts_ >= block_entry2->checkpoint_ts_)
        return;
    this->row_count_ = block_entry2->row_count_;
    this->columns_.swap(block_entry2->columns_);
    *this->block_version_ = *block_entry2->block_version_;
    this->max_row_ts_ = block_entry2->max_row_ts_;
    this->checkpoint_ts_ = block_entry2->checkpoint_ts_;
    this->checkpoint_row_count_ = block_entry2->checkpoint_row_count_;
}

const SharedPtr<DataType> BlockEntry::GetColumnType(u64 column_id) const {
    return this->segment_entry_->GetTableEntry()->GetColumnDefByID(column_id)->type();
}

u32 BlockEntry::segment_id() const { return segment_entry_->segment_id(); }

} // namespace infinity
