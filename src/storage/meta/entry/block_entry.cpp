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

module catalog;

import stl;
import default_values;
import logger;
import third_party;
import defer_op;
import local_file_system;
import serialize;
import catalog_delta_entry;

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
        LOG_WARN(fmt::format("Failed to open block_version file: {}", version_path));
        // load the block_version file not exist return and create version
        return;
    }
    int buf_len = std::filesystem::file_size(version_path);
    Vector<char> buf(buf_len);
    ifs.read(buf.data(), buf_len);
    ifs.close();
    char *ptr = buf.data();
    i32 created_size = ReadBufAdv<i32>(ptr);
    i32 deleted_size = ReadBufAdv<i32>(ptr);
    created_.resize(created_size);
    deleted_.resize(deleted_size);
    std::memcpy(created_.data(), ptr, created_size * sizeof(CreateField));
    ptr += created_size * sizeof(CreateField);
    std::memcpy(deleted_.data(), ptr, deleted_size * sizeof(TxnTimeStamp));
    ptr += deleted_.size() * sizeof(TxnTimeStamp);
    if (ptr - buf.data() != buf_len) {
        UnrecoverableError(fmt::format("Failed to load block_version file: {}", version_path));
    }
}

void BlockVersion::SaveToFile(const String &version_path) {
    i32 exp_size = sizeof(i32) + created_.size() * sizeof(CreateField);
    exp_size += sizeof(i32) + deleted_.size() * sizeof(TxnTimeStamp);
    Vector<char> buf(exp_size, 0);
    char *ptr = buf.data();
    WriteBufAdv<i32>(ptr, i32(created_.size()));
    WriteBufAdv<i32>(ptr, i32(deleted_.size()));
    std::memcpy(ptr, created_.data(), created_.size() * sizeof(CreateField));
    ptr += created_.size() * sizeof(CreateField);
    std::memcpy(ptr, deleted_.data(), deleted_.size() * sizeof(TxnTimeStamp));
    ptr += deleted_.size() * sizeof(TxnTimeStamp);
    if (ptr - buf.data() != exp_size) {
        UnrecoverableError(fmt::format("Failed to save block_version file: {}", version_path));
    }
    std::ofstream ofs = std::ofstream(version_path, std::ios::trunc | std::ios::binary);
    if (!ofs.is_open()) {
        UnrecoverableError(fmt::format("Failed to open block_version file: {}", version_path));
    }
    ofs.write(buf.data(), ptr - buf.data());
    ofs.flush();
    ofs.close();
}

/// class BlockEntry
BlockEntry::BlockEntry(const SegmentEntry *segment_entry, BlockID block_id, TxnTimeStamp checkpoint_ts)
    : BaseEntry(EntryType::kBlock), segment_entry_(segment_entry), block_id_(block_id), row_count_(0), row_capacity_(DEFAULT_VECTOR_SIZE),
      checkpoint_ts_(checkpoint_ts) {}

UniquePtr<BlockEntry>
BlockEntry::NewBlockEntry(const SegmentEntry *segment_entry, BlockID block_id, TxnTimeStamp checkpoint_ts, u64 column_count, Txn *txn) {
    auto block_entry = MakeUnique<BlockEntry>(segment_entry, block_id, checkpoint_ts);

    {
        auto operation = MakeUnique<AddBlockEntryOp>(block_entry.get());
        txn->AddCatalogDeltaOperation(std::move(operation));
    }

    auto begin_ts = txn->BeginTS();
    block_entry->begin_ts_ = begin_ts;

    block_entry->block_dir_ = BlockEntry::DetermineDir(*segment_entry->segment_dir(), block_id);
    block_entry->columns_.reserve(column_count);
    for (SizeT column_id = 0; column_id < column_count; ++column_id) {
        auto column_entry = BlockColumnEntry::NewBlockColumnEntry(block_entry.get(), column_id, txn);
        block_entry->columns_.emplace_back(std::move(column_entry));
    }
    block_entry->block_version_ = MakeUnique<BlockVersion>(block_entry->row_capacity_);
    return block_entry;
}

UniquePtr<BlockEntry> BlockEntry::NewReplayBlockEntry(const SegmentEntry *segment_entry,
                                                      BlockID block_id,
                                                      TxnTimeStamp checkpoint_ts,
                                                      u64 column_count,
                                                      BufferManager *buffer_mgr,
                                                      u16 row_count,
                                                      TxnTimeStamp min_row_ts,
                                                      TxnTimeStamp max_row_ts) {

    auto block_entry = MakeUnique<BlockEntry>(segment_entry, block_id, checkpoint_ts);

    block_entry->row_count_ = row_count;
    block_entry->min_row_ts_ = min_row_ts;
    block_entry->max_row_ts_ = max_row_ts;
    block_entry->block_dir_ = BlockEntry::DetermineDir(*segment_entry->segment_dir(), block_id);
    block_entry->columns_.reserve(column_count);
    for (SizeT column_id = 0; column_id < column_count; ++column_id) {
        block_entry->columns_.emplace_back(BlockColumnEntry::NewReplayBlockColumnEntry(block_entry.get(), column_id, buffer_mgr));
    }
    block_entry->block_version_ = MakeUnique<BlockVersion>(block_entry->row_capacity_);
    block_entry->block_version_->created_.emplace_back((TxnTimeStamp)block_entry->min_row_ts_, (i32)block_entry->row_count_);
    return block_entry;
}

UniquePtr<BlockEntry> BlockEntry::NewReplayCatalogBlockEntry(const SegmentEntry *segment_entry,
                                                             BlockID block_id,
                                                             u16 row_count,
                                                             u16 row_capacity,
                                                             TxnTimeStamp min_row_ts,
                                                             TxnTimeStamp max_row_ts,
                                                             TxnTimeStamp check_point_ts,
                                                             u16 checkpoint_row_count,
                                                             BufferManager *buffer_mgr) {

    auto block_entry = MakeUnique<BlockEntry>(segment_entry, block_id, 0);

    block_entry->row_count_ = row_count;
    block_entry->min_row_ts_ = min_row_ts;
    block_entry->max_row_ts_ = max_row_ts;
    block_entry->block_dir_ = BlockEntry::DetermineDir(*segment_entry->segment_dir(), block_id);
    block_entry->block_version_ = MakeUnique<BlockVersion>(block_entry->row_capacity_);
    block_entry->block_version_->created_.emplace_back((TxnTimeStamp)block_entry->min_row_ts_, (i32)block_entry->row_count_);
    block_entry->checkpoint_ts_ = check_point_ts;
    block_entry->checkpoint_row_count_ = checkpoint_row_count;
    return block_entry;
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

u16 BlockEntry::AppendData(TransactionID txn_id,
                           DataBlock *input_data_block,
                           BlockOffset input_block_offset,
                           u16 append_rows,
                           BufferManager *buffer_mgr) {
    std::unique_lock<std::shared_mutex> lck(this->rw_locker_);
    if (this->using_txn_id_ != 0 && this->using_txn_id_ != txn_id) {
        UnrecoverableError(
            fmt::format("Multiple transactions are changing data of Segment: {}, Block: {}", this->segment_entry_->segment_id(), this->block_id_));
    }

    this->using_txn_id_ = txn_id;
    u16 actual_copied = append_rows;
    if (this->row_count_ + append_rows > this->row_capacity_) {
        actual_copied = this->row_capacity_ - this->row_count_;
    }

    SizeT column_count = this->columns_.size();
    for (SizeT column_id = 0; column_id < column_count; ++column_id) {
        this->columns_[column_id]->Append(input_data_block->column_vectors[column_id].get(), input_block_offset, actual_copied, buffer_mgr);

        LOG_TRACE(fmt::format("Segment: {}, Block: {}, Column: {} is appended with {} rows",
                              this->segment_entry_->segment_id(),
                              this->block_id_,
                              column_id,
                              actual_copied));
    }

    this->row_count_ += actual_copied;
    return actual_copied;
}

void BlockEntry::DeleteData(TransactionID txn_id, TxnTimeStamp commit_ts, const Vector<RowID> &rows) {
    std::unique_lock<std::shared_mutex> lck(this->rw_locker_);
    if (this->using_txn_id_ != 0 && this->using_txn_id_ != txn_id) {
        UnrecoverableError(
            fmt::format("Multiple transactions are changing data of Segment: {}, Block: {}", this->segment_entry_->segment_id(), this->block_id_));
    }

    this->using_txn_id_ = txn_id;

    u32 segment_id = this->segment_entry_->segment_id();
    u16 block_id = this->block_id_;

    auto &block_version = this->block_version_;
    for (RowID row_id : rows) {
        u16 block_offset = row_id.segment_offset_ % DEFAULT_BLOCK_CAPACITY;
        block_version->deleted_[block_offset] = commit_ts;
    }

    LOG_TRACE(fmt::format("Segment {} Block {} has deleted {} rows", segment_id, block_id, rows.size()));
}

// A txn may invoke AppendData() multiple times, and then invoke CommitAppend() once.
void BlockEntry::CommitAppend(TransactionID txn_id, TxnTimeStamp commit_ts) {
    std::unique_lock<std::shared_mutex> lck(this->rw_locker_);
    if (this->using_txn_id_ != txn_id) {
        UnrecoverableError(
            fmt::format("Multiple transactions are changing data of Segment: {}, Block: {}", this->segment_entry_->segment_id(), this->block_id_));
    }
    this->using_txn_id_ = 0;
    if (this->min_row_ts_ == 0) {
        this->min_row_ts_ = commit_ts;
    }
    this->max_row_ts_ = std::max(this->max_row_ts_, commit_ts);

    auto &block_version = this->block_version_;
    block_version->created_.push_back({commit_ts, i32(this->row_count_)});
}

void BlockEntry::CommitDelete(TransactionID txn_id, TxnTimeStamp commit_ts) {
    std::unique_lock<std::shared_mutex> lck(this->rw_locker_);
    if (this->using_txn_id_ != 0 && this->using_txn_id_ != txn_id) {
        UnrecoverableError(
            fmt::format("Multiple transactions are changing data of Segment: {}, Block: {}", this->segment_entry_->segment_id(), this->block_id_));
    }

    if (this->using_txn_id_ == 0) {
        // Which case will come here?
        return;
    }

    this->using_txn_id_ = 0;
    if (this->min_row_ts_ == 0) {
        this->min_row_ts_ = commit_ts;
    }
    this->max_row_ts_ = std::max(this->max_row_ts_, commit_ts);
}

void BlockEntry::FlushData(int64_t checkpoint_row_count) {
    SizeT column_count = this->columns_.size();
    SizeT column_idx = 0;
    while (column_idx < column_count) {
        BlockColumnEntry *block_column_entry = this->columns_[column_idx].get();
        BlockColumnEntry::Flush(block_column_entry, checkpoint_row_count);
        LOG_TRACE(fmt::format("ColumnData {} is flushed", block_column_entry->column_id()));
        ++column_idx;
    }
}

void BlockEntry::FlushVersion(BlockVersion &checkpoint_version) { checkpoint_version.SaveToFile(this->VersionFilePath()); }

void BlockEntry::Flush(TxnTimeStamp checkpoint_ts) {
    LOG_TRACE(fmt::format("Segment: {}, Block: {} is being flushing", this->segment_entry_->segment_id(), this->block_id_));
    if (checkpoint_ts < this->checkpoint_ts_) {
        UnrecoverableError(
            fmt::format("BlockEntry checkpoint_ts skew! checkpoint_ts: {}, this->checkpoint_ts_: {}", checkpoint_ts, this->checkpoint_ts_));
    }
    int checkpoint_row_count = 0;

    BlockVersion checkpoint_version(this->block_version_->deleted_.size());
    {
        std::shared_lock<std::shared_mutex> lock(this->rw_locker_);
        // Skip if entry has been flushed at some previous checkpoint, or is invisible at current checkpoint.
        if (this->max_row_ts_ <= this->checkpoint_ts_ || this->min_row_ts_ > checkpoint_ts)
            return;

        checkpoint_row_count = this->block_version_->GetRowCount(checkpoint_ts);
        if (checkpoint_row_count == 0) {
            UnrecoverableError("BlockEntry is empty at checkpoint_ts!");
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
        std::memcpy(checkpoint_version.deleted_.data(), deleted.data(), checkpoint_row_count * sizeof(TxnTimeStamp));
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
    LOG_TRACE(fmt::format("Segment: {}, Block {} is flushed", this->segment_entry_->segment_id(), this->block_id_));
    return;
}

// TODO: introduce BlockColumnMeta
nlohmann::json BlockEntry::Serialize(TxnTimeStamp) {
    nlohmann::json json_res;
    std::shared_lock<std::shared_mutex> lck(this->rw_locker_);

    json_res["block_id"] = this->block_id_;
    json_res["checkpoint_ts"] = this->checkpoint_ts_;
    json_res["row_count"] = this->checkpoint_row_count_;
    json_res["row_capacity"] = this->row_capacity_;
    json_res["block_dir"] = *this->block_dir_;
    for (const auto &block_column_entry : this->columns_) {
        json_res["columns"].emplace_back(block_column_entry->Serialize());
    }
    json_res["min_row_ts"] = this->min_row_ts_;
    json_res["max_row_ts"] = this->checkpoint_ts_;
    json_res["version_file"] = this->VersionFilePath();

    json_res["commit_ts"] = TxnTimeStamp(this->commit_ts_);
    json_res["begin_ts"] = TxnTimeStamp(this->begin_ts_);
    json_res["txn_id"] = TransactionID(this->txn_id_);
    return json_res;
}

UniquePtr<BlockEntry> BlockEntry::Deserialize(const nlohmann::json &block_entry_json, SegmentEntry *segment_entry, BufferManager *buffer_mgr) {
    u64 block_id = block_entry_json["block_id"];
    TxnTimeStamp checkpoint_ts = block_entry_json["checkpoint_ts"];

    auto block_dir = block_entry_json["block_dir"];
    auto row_capacity = block_entry_json["row_capacity"];
    auto row_count = block_entry_json["row_count"];
    auto min_row_ts = block_entry_json["min_row_ts"];
    auto max_row_ts = block_entry_json["max_row_ts"];

    UniquePtr<BlockEntry> block_entry = BlockEntry::NewReplayCatalogBlockEntry(segment_entry,
                                                                               block_id,
                                                                               row_count,
                                                                               row_capacity,
                                                                               min_row_ts,
                                                                               max_row_ts,
                                                                               checkpoint_ts,
                                                                               row_count,
                                                                               buffer_mgr);

    block_entry->commit_ts_ = block_entry_json["commit_ts"];
    block_entry->begin_ts_ = block_entry_json["begin_ts"];
    block_entry->txn_id_ = block_entry_json["txn_id"];

    block_entry->block_version_->LoadFromFile(block_entry->VersionFilePath());
    if (block_entry->block_version_->created_.empty()) {
        block_entry->block_version_->created_.emplace_back(block_entry->max_row_ts_, block_entry->row_count_);
    }

    for (const auto &block_column_json : block_entry_json["columns"]) {
        block_entry->columns_.emplace_back(BlockColumnEntry::Deserialize(block_column_json, block_entry.get(), buffer_mgr));
    }

    return block_entry;
}

i32 BlockEntry::GetAvailableCapacity() {
    std::shared_lock<std::shared_mutex> lck(this->rw_locker_);
    return this->row_capacity_ - this->row_count_;
}

SharedPtr<String> BlockEntry::DetermineDir(const String &parent_dir, BlockID block_id) {
    LocalFileSystem fs;
    SharedPtr<String> base_dir;
    base_dir = MakeShared<String>(fmt::format("{}/blk_{}", parent_dir, block_id));
    fs.CreateDirectoryNoExp(*base_dir);
    return base_dir;
}

void BlockEntry::MergeFrom(BaseEntry &other) {
    auto block_entry2 = dynamic_cast<BlockEntry *>(&other);
    if (block_entry2 == nullptr) {
        UnrecoverableError("MergeFrom requires the same type of BaseEntry");
    }
    // // No locking here since only the load stage needs MergeFrom.
    if (*this->block_dir_ != *block_entry2->block_dir_) {
        UnrecoverableError("BlockEntry::MergeFrom requires base_dir_ match");
    }
    if (this->block_id_ != block_entry2->block_id_) {
        UnrecoverableError("BlockEntry::MergeFrom requires block_id_ match");
    }
    if (this->row_capacity_ != block_entry2->row_capacity_) {
        UnrecoverableError("BlockEntry::MergeFrom requires row_capacity_ match");
    }
    if (this->min_row_ts_ != block_entry2->min_row_ts_) {
        UnrecoverableError("BlockEntry::MergeFrom requires min_row_ts_ match");
    }
    if (this->row_count_ > block_entry2->row_count_) {
        UnrecoverableError("BlockEntry::MergeFrom requires source block entry rows not more than target block entry rows");
    }
    if (this->checkpoint_ts_ > block_entry2->checkpoint_ts_) {
        UnrecoverableError(
            "BlockEntry::MergeFrom requires source block entry checkpoint timestamp not more than target block entry checkpoint timestamp");
    }
    if (this->checkpoint_row_count_ > block_entry2->checkpoint_row_count_) {
        UnrecoverableError(
            "BlockEntry::MergeFrom requires source block entry checkpoint row count not more than target block entry checkpoint row count");
    }
    if (columns_.size() > block_entry2->columns_.size()) {
        UnrecoverableError("BlockEntry::MergeFrom: Attempt to merge two block entries with difference column count.");
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
