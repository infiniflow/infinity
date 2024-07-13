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

module block_entry;

import stl;
import default_values;
import logger;
import third_party;
import defer_op;
import local_file_system;
import serialize;
import catalog_delta_entry;
import internal_types;
import infinity_exception;
import data_type;
import segment_entry;
import version_file_worker;
import column_vector;
import bitmask;
import block_version;
import cleanup_scanner;
import buffer_manager;
import buffer_obj;
import logical_type;

namespace infinity {

Vector<std::string_view> BlockEntry::DecodeIndex(std::string_view encode) {
    SizeT delimiter_i = encode.rfind('#');
    if (delimiter_i == String::npos) {
        String error_message = fmt::format("Invalid block entry encode: {}", encode);
        UnrecoverableError(error_message);
    }
    auto decodes = SegmentEntry::DecodeIndex(encode.substr(0, delimiter_i));
    decodes.push_back(encode.substr(delimiter_i + 1));
    return decodes;
}

String BlockEntry::EncodeIndex(const BlockID block_id, const SegmentEntry *segment_entry) {
    return fmt::format("{}#{}", segment_entry->encode(), block_id);
}

/// class BlockEntry
BlockEntry::BlockEntry(const SegmentEntry *segment_entry, BlockID block_id, TxnTimeStamp checkpoint_ts)
    : BaseEntry(EntryType::kBlock, false, segment_entry->base_dir_, BlockEntry::EncodeIndex(block_id, segment_entry)), segment_entry_(segment_entry),
      block_id_(block_id), row_count_(0), row_capacity_(DEFAULT_VECTOR_SIZE), checkpoint_ts_(checkpoint_ts) {}

UniquePtr<BlockEntry>
BlockEntry::NewBlockEntry(const SegmentEntry *segment_entry, BlockID block_id, TxnTimeStamp checkpoint_ts, u64 column_count, Txn *txn) {
    auto block_entry = MakeUnique<BlockEntry>(segment_entry, block_id, checkpoint_ts);

    block_entry->begin_ts_ = txn->BeginTS();
    block_entry->txn_id_ = txn->TxnID();

    block_entry->block_dir_ = BlockEntry::DetermineDir(*block_entry->base_dir_, *segment_entry->segment_dir(), block_id);
    block_entry->columns_.reserve(column_count);
    for (SizeT column_id = 0; column_id < column_count; ++column_id) {
        auto column_entry = BlockColumnEntry::NewBlockColumnEntry(block_entry.get(), column_id, txn);
        block_entry->columns_.emplace_back(std::move(column_entry));
    }

    SharedPtr<String> file_dir = MakeShared<String>(fmt::format("{}/{}", *block_entry->base_dir_, *block_entry->block_dir_));
    auto version_file_worker = MakeUnique<VersionFileWorker>(file_dir, BlockVersion::FileName(), block_entry->row_capacity_);
    auto *buffer_mgr = txn->buffer_mgr();
    block_entry->block_version_ = buffer_mgr->AllocateBufferObject(std::move(version_file_worker));
    return block_entry;
}

UniquePtr<BlockEntry> BlockEntry::NewReplayBlockEntry(const SegmentEntry *segment_entry,
                                                      BlockID block_id,
                                                      u16 row_count,
                                                      u16 row_capacity,
                                                      TxnTimeStamp min_row_ts,
                                                      TxnTimeStamp max_row_ts,
                                                      TxnTimeStamp commit_ts,
                                                      TxnTimeStamp check_point_ts,
                                                      u16 checkpoint_row_count,
                                                      BufferManager *buffer_mgr,
                                                      TransactionID txn_id) {

    auto block_entry = MakeUnique<BlockEntry>(segment_entry, block_id, 0);

    block_entry->txn_id_ = txn_id;

    block_entry->row_count_ = row_count;
    block_entry->min_row_ts_ = min_row_ts;
    block_entry->max_row_ts_ = max_row_ts;
    block_entry->commit_ts_ = commit_ts;
    block_entry->block_dir_ = BlockEntry::DetermineDir(*segment_entry->base_dir_, *segment_entry->segment_dir(), block_id);

    SharedPtr<String> file_dir = MakeShared<String>(fmt::format("{}/{}", *segment_entry->base_dir_, *block_entry->block_dir_));
    auto version_file_worker = MakeUnique<VersionFileWorker>(file_dir, BlockVersion::FileName(), row_capacity);
    block_entry->block_version_ = buffer_mgr->GetBufferObject(std::move(version_file_worker));

    block_entry->checkpoint_ts_ = check_point_ts;
    block_entry->checkpoint_row_count_ = checkpoint_row_count;
    return block_entry;
}

void BlockEntry::UpdateBlockReplay(SharedPtr<BlockEntry> block_entry, String block_filter_binary_data) {
    row_count_ = block_entry->row_count_;
    min_row_ts_ = block_entry->min_row_ts_;
    max_row_ts_ = block_entry->max_row_ts_;
    checkpoint_ts_ = block_entry->checkpoint_ts_;
    checkpoint_row_count_ = block_entry->checkpoint_row_count_;
    if (!block_filter_binary_data.empty()) {
        LoadFilterBinaryData(block_filter_binary_data);
    }
}

SizeT BlockEntry::row_count(TxnTimeStamp check_ts) const {
    std::shared_lock lock(rw_locker_);
    if (check_ts >= max_row_ts_)
        return row_count_;

    auto block_version_handle = this->block_version_->Load();
    const auto *block_version = reinterpret_cast<const BlockVersion *>(block_version_handle.GetData());
    return block_version->GetRowCount(check_ts);
}

Pair<BlockOffset, BlockOffset> BlockEntry::GetVisibleRange(TxnTimeStamp begin_ts, u16 block_offset_begin) const {
    std::shared_lock lock(rw_locker_);
    begin_ts = std::min(begin_ts, this->max_row_ts_);

    auto block_version_handle = this->block_version_->Load();
    const auto *block_version = reinterpret_cast<const BlockVersion *>(block_version_handle.GetData());

    auto &deleted = block_version->deleted_;
    BlockOffset block_offset_end = block_version->GetRowCount(begin_ts);
    while (block_offset_begin < block_offset_end && deleted[block_offset_begin] != 0 && deleted[block_offset_begin] <= begin_ts) {
        block_offset_begin++;
    }
    BlockOffset row_idx;
    for (row_idx = block_offset_begin; row_idx < block_offset_end; ++row_idx) {
        if (deleted[row_idx] != 0 && deleted[row_idx] <= begin_ts) {
            break;
        }
    }
    return {block_offset_begin, row_idx};
}

bool BlockEntry::CheckRowVisible(BlockOffset block_offset, TxnTimeStamp check_ts, bool check_append) const {
    std::shared_lock lock(rw_locker_);

    auto block_version_handle = this->block_version_->Load();
    const auto *block_version = reinterpret_cast<const BlockVersion *>(block_version_handle.GetData());

    if (check_append && block_version->GetRowCount(check_ts) <= block_offset) {
        return false;
    }
    const auto &deleted = block_version->deleted_;
    return deleted[block_offset] == 0 || deleted[block_offset] > check_ts;
}

void BlockEntry::SetDeleteBitmask(TxnTimeStamp query_ts, Bitmask &bitmask) const {
    BlockOffset read_offset = 0;
    while (true) {
        auto [row_begin, row_end] = GetVisibleRange(query_ts, read_offset);
        if (row_begin == row_end) {
            break;
        }
        for (BlockOffset offset = read_offset; offset < row_begin; ++offset) {
            bitmask.SetFalse(offset);
        }
        read_offset = row_end;
    }
    // FIXME: read row_count_ is not thread safe
    for (BlockOffset offset = read_offset; offset < row_count_; ++offset) {
        bitmask.SetFalse(offset);
    }
}

u16 BlockEntry::AppendData(TransactionID txn_id,
                           TxnTimeStamp commit_ts,
                           DataBlock *input_data_block,
                           BlockOffset input_block_offset,
                           u16 append_rows,
                           BufferManager *buffer_mgr) {
    std::unique_lock<std::shared_mutex> lck(this->rw_locker_);
    if (this->using_txn_id_ != 0 && this->using_txn_id_ != txn_id) {
        String error_message = fmt::format("Multiple transactions are changing data of Segment: {}, Block: {}, using_txn_id_: {}, txn_id: {}",
                                           this->segment_entry_->segment_id(),
                                           this->block_id_,
                                           this->using_txn_id_,
                                           txn_id);
        UnrecoverableError(error_message);
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

    auto block_version_handle = block_version_->Load();
    auto *block_version = reinterpret_cast<BlockVersion *>(block_version_handle.GetDataMut());
    block_version->created_.emplace_back(commit_ts, this->row_count_);

    return actual_copied;
}

SizeT BlockEntry::DeleteData(TransactionID txn_id, TxnTimeStamp commit_ts, const Vector<BlockOffset> &rows) {
    std::unique_lock<std::shared_mutex> lck(this->rw_locker_);
    if (this->using_txn_id_ != 0 && this->using_txn_id_ != txn_id) {
        UnrecoverableError(
            fmt::format("Multiple transactions are changing data of Segment: {}, Block: {}", this->segment_entry_->segment_id(), this->block_id_));
    }

    this->using_txn_id_ = txn_id;

    u32 segment_id = this->segment_entry_->segment_id();
    u16 block_id = this->block_id_;

    auto block_version_handle = block_version_->Load();
    auto *block_version = reinterpret_cast<BlockVersion *>(block_version_handle.GetDataMut());

    SizeT delete_row_n = 0;
    for (BlockOffset block_offset : rows) {
        if (block_version->deleted_[block_offset] != 0) {
            String error_message = fmt::format("Segment {} Block {} Row {} is already deleted at {}, cur commit_ts: {}.",
                                               segment_id,
                                               block_id,
                                               block_offset,
                                               block_version->deleted_[block_offset],
                                               commit_ts);
            UnrecoverableError(error_message);
        }
        block_version->deleted_[block_offset] = commit_ts;
        delete_row_n++;
    }

    LOG_TRACE(fmt::format("Segment {} Block {} has deleted {} rows", segment_id, block_id, rows.size()));
    return delete_row_n;
}

void BlockEntry::CommitFlushed(TxnTimeStamp commit_ts) {
    std::unique_lock w_lock(rw_locker_);
    auto block_version_handle = block_version_->Load();
    auto *block_version = reinterpret_cast<BlockVersion *>(block_version_handle.GetDataMut());
    if (!block_version->created_.empty()) {
        String error_message = "BlockEntry::CommitFlushed: block_version->created_ is not empty";
        UnrecoverableError(error_message);
    }
    block_version->created_.emplace_back(commit_ts, this->row_count_);

    FlushVersionNoLock(commit_ts);
}

void BlockEntry::CommitBlock(TransactionID txn_id, TxnTimeStamp commit_ts) {
    std::unique_lock w_lock(this->rw_locker_);

    if (this->using_txn_id_ != 0 && this->using_txn_id_ != txn_id) {
        UnrecoverableError(
            fmt::format("Multiple transactions are changing data of Segment: {}, Block: {}", this->segment_entry_->segment_id(), this->block_id_));
    }
    this->using_txn_id_ = 0;

    min_row_ts_ = std::min(min_row_ts_, commit_ts);
    if (commit_ts < max_row_ts_) {
        String error_message = fmt::format("BlockEntry commit_ts {} is less than max_row_ts_ {}", commit_ts, max_row_ts_);
        UnrecoverableError(error_message);
    }
    max_row_ts_ = commit_ts;
    if (!this->Committed()) {
        this->Commit(commit_ts);
        for (auto &column : columns_) {
            column->CommitColumn(txn_id, commit_ts);
        }
    }
}

ColumnVector BlockEntry::GetCreateTSVector(BufferManager *buffer_mgr, SizeT offset, SizeT size) const {
    ColumnVector column_vector(MakeShared<DataType>(LogicalType::kBigInt));
    column_vector.Initialize(ColumnVectorType::kFlat, size);
    {
        std::shared_lock<std::shared_mutex> lock(this->rw_locker_);
        auto block_version_handle = this->block_version_->Load();
        const auto *block_version = reinterpret_cast<const BlockVersion *>(block_version_handle.GetData());
        block_version->GetCreateTS(offset, size, column_vector);
    }
    return column_vector;
}

ColumnVector BlockEntry::GetDeleteTSVector(BufferManager *buffer_mgr, SizeT offset, SizeT size) const {
    ColumnVector column_vector(MakeShared<DataType>(LogicalType::kBigInt));
    column_vector.Initialize(ColumnVectorType::kFlat, size);
    {
        std::shared_lock<std::shared_mutex> lock(this->rw_locker_);
        auto block_version_handle = this->block_version_->Load();
        const auto *block_version = reinterpret_cast<const BlockVersion *>(block_version_handle.GetData());
        for (SizeT i = offset; i < offset + size; ++i) {
            column_vector.AppendByPtr(reinterpret_cast<const char *>(&block_version->deleted_[i]));
        }
    }
    return column_vector;
}

void BlockEntry::FlushDataNoLock(SizeT start_row_count, SizeT checkpoint_row_count) {
    SizeT column_count = this->columns_.size();
    SizeT column_idx = 0;
    while (column_idx < column_count) {
        BlockColumnEntry *block_column_entry = this->columns_[column_idx].get();
        BlockColumnEntry::Flush(block_column_entry, start_row_count, checkpoint_row_count);
        LOG_TRACE(fmt::format("ColumnData {} is flushed", block_column_entry->column_id()));
        ++column_idx;
    }
}

bool BlockEntry::FlushVersionNoLock(TxnTimeStamp checkpoint_ts) {
    // Skip if entry has been flushed at some previous checkpoint, or is invisible at current checkpoint.
    if (this->max_row_ts_ != 0 && (this->max_row_ts_ <= this->checkpoint_ts_ || this->min_row_ts_ > checkpoint_ts)) {
        return false;
    }

    auto block_version_handle = block_version_->Load();
    // call GetDataMut to set BufferObj type to BufferType::kEphemeral
    auto *block_version = reinterpret_cast<BlockVersion *>(block_version_handle.GetDataMut());
    if (block_version->deleted_.size() != this->row_capacity_) {
        auto err_info = fmt::format("BlockEntry::FlushVersionNoLock: block_version->deleted_.size() {} != this->row_capacity_ {}",
                                    block_version->deleted_.size(),
                                    this->row_capacity_);
        UnrecoverableError(err_info);
    }
    auto *version_file_worker = static_cast<VersionFileWorker *>(block_version_->file_worker());
    version_file_worker->SetCheckpointTS(checkpoint_ts);
    block_version_->Save();

    return true;
}

void BlockEntry::Flush(TxnTimeStamp checkpoint_ts) {
    std::unique_lock w_lock(rw_locker_);
    LOG_TRACE(fmt::format("Segment: {}, Block: {} is flushing", this->segment_entry_->segment_id(), this->block_id_));
    if (checkpoint_ts < this->checkpoint_ts_) {
        UnrecoverableError(
            fmt::format("BlockEntry checkpoint_ts skew! checkpoint_ts: {}, this->checkpoint_ts_: {}", checkpoint_ts, this->checkpoint_ts_));
    }

    LOG_TRACE("Block entry flush before flush version");
    bool flush = FlushVersionNoLock(checkpoint_ts);
    if (flush) {
        auto block_version_handle = block_version_->Load();
        auto *block_version = static_cast<const BlockVersion *>(block_version_handle.GetData());
        SizeT checkpoint_row_count = block_version->GetRowCount(checkpoint_ts);

        LOG_TRACE("Block entry flush before flush data");
        FlushDataNoLock(this->checkpoint_row_count_, checkpoint_row_count);
        this->checkpoint_ts_ = checkpoint_ts;
        this->checkpoint_row_count_ = checkpoint_row_count;
        LOG_TRACE(fmt::format("Segment: {}, Block {} is flushed {} rows",
                              this->segment_entry_->segment_id(),
                              this->block_id_,
                              this->checkpoint_row_count_));
    }
}

void BlockEntry::FlushForImport() { FlushDataNoLock(0, this->row_count_); }

void BlockEntry::LoadFilterBinaryData(const String &block_filter_data) { fast_rough_filter_.DeserializeFromString(block_filter_data); }

void BlockEntry::Cleanup() {
    for (auto &block_column_entry : columns_) {
        block_column_entry->Cleanup();
    }
    block_version_->PickForCleanup();

    String full_block_dir = fmt::format("{}/{}", *base_dir(), *block_dir_);
    LOG_DEBUG(fmt::format("Cleaning up block dir: {}", full_block_dir));
    CleanupScanner::CleanupDir(*block_dir_);
    LOG_DEBUG(fmt::format("Cleaned block dir: {}", full_block_dir));
}

// TODO: introduce BlockColumnMeta
nlohmann::json BlockEntry::Serialize(TxnTimeStamp max_commit_ts) {
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
    json_res["max_row_ts"] = std::min(this->max_row_ts_, max_commit_ts);
    json_res["version_file"] = this->VersionFilePath();

    json_res["commit_ts"] = TxnTimeStamp(this->commit_ts_);
    json_res["begin_ts"] = TxnTimeStamp(this->begin_ts_);
    json_res["txn_id"] = TransactionID(this->txn_id_);

    // LOG_TRACE(fmt::format("BlockEntry::Serialize: Begin try to save FastRoughFilter to json file"));
    this->GetFastRoughFilter()->SaveToJsonFile(json_res);
    // LOG_TRACE(fmt::format("BlockEntry::Serialize: End try to save FastRoughFilter to json file"));

    return json_res;
}

UniquePtr<BlockEntry> BlockEntry::Deserialize(const nlohmann::json &block_entry_json, SegmentEntry *segment_entry, BufferManager *buffer_mgr) {
    u64 block_id = block_entry_json["block_id"];

    auto block_dir = block_entry_json["block_dir"];
    auto row_capacity = block_entry_json["row_capacity"];
    auto row_count = block_entry_json["row_count"];
    auto min_row_ts = block_entry_json["min_row_ts"];
    auto max_row_ts = block_entry_json["max_row_ts"];
    auto commit_ts = block_entry_json["commit_ts"];
    auto checkpoint_ts = block_entry_json["checkpoint_ts"];

    auto txn_id = block_entry_json["txn_id"];

    UniquePtr<BlockEntry> block_entry = BlockEntry::NewReplayBlockEntry(segment_entry,
                                                                        block_id,
                                                                        row_count,
                                                                        row_capacity,
                                                                        min_row_ts,
                                                                        max_row_ts,
                                                                        commit_ts,
                                                                        checkpoint_ts,
                                                                        row_count,
                                                                        buffer_mgr,
                                                                        txn_id);

    block_entry->begin_ts_ = block_entry_json["begin_ts"];

    for (const auto &block_column_json : block_entry_json["columns"]) {
        block_entry->columns_.emplace_back(BlockColumnEntry::Deserialize(block_column_json, block_entry.get(), buffer_mgr));
    }

    // Load FastRoughFilter from json file
    // reduce log
    if (block_entry->GetFastRoughFilter()->LoadFromJsonFile(block_entry_json)) {
        // LOG_TRACE("BlockEntry::Deserialize: Finish load FastRoughFilter from json file");
    } else {
        // LOG_TRACE("BlockEntry::Deserialize: Cannot load FastRoughFilter from json file");
    }

    return block_entry;
}

i32 BlockEntry::GetAvailableCapacity() {
    std::shared_lock<std::shared_mutex> lck(this->rw_locker_);
    return this->row_capacity_ - this->row_count_;
}

SharedPtr<String> BlockEntry::DetermineDir(const String &base_dir, const String &parent_dir, BlockID block_id) {
    LocalFileSystem fs;
    SharedPtr<String> relative_dir = MakeShared<String>(fmt::format("{}/blk_{}", parent_dir, block_id));
    SharedPtr<String> full_dir = MakeShared<String>(fmt::format("{}/{}", base_dir, *relative_dir));
    fs.CreateDirectoryNoExp(*full_dir);
    return relative_dir;
}

void BlockEntry::AddColumnReplay(UniquePtr<BlockColumnEntry> column_entry, ColumnID column_id) {
    if (column_id >= columns_.size()) {
        columns_.resize(column_id + 1);
    }
    columns_[column_id] = std::move(column_entry);
}

void BlockEntry::AppendBlock(const Vector<ColumnVector> &column_vectors, SizeT row_begin, SizeT read_size, BufferManager *buffer_mgr) {
    if (read_size + row_count_ > row_capacity_) {
        String error_message = "BlockEntry::AppendBlock: read_size + row_count_ > row_capacity_";
        UnrecoverableError(error_message);
    }
    for (ColumnID column_id = 0; column_id < columns_.size(); ++column_id) {
        columns_[column_id]->Append(&column_vectors[column_id], row_begin, read_size, buffer_mgr);
    }
    IncreaseRowCount(read_size);
}

const SharedPtr<DataType> BlockEntry::GetColumnType(u64 column_id) const {
    return this->segment_entry_->GetTableEntry()->GetColumnDefByID(column_id)->type();
}

u32 BlockEntry::segment_id() const { return segment_entry_->segment_id(); }

} // namespace infinity
