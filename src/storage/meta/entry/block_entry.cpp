//
// Created by jinhai on 23-10-2.
//

module;

#include <filesystem>
#include <memory>

import stl;
import base_entry;
import segment_entry;
import default_values;
import block_entry;
import block_column_entry;
import logger;
import third_party;
import defer_op;
import local_file_system;
import serialize;

import infinity_exception;
import table_collection_entry;
import parser;

module block_entry;
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
    int idx = created_.size() - 1;
    for (; idx >= 0; idx--) {
        if (created_[idx].first <= begin_ts)
            break;
    }
    if (idx < 0)
        return 0;
    return created_[idx].second;
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
    Memcpy(created_.data(), ptr, created_size * sizeof(Pair<TxnTimeStamp, int32_t>));
    ptr += created_size * sizeof(Pair<TxnTimeStamp, int32_t>);
    Memcpy(deleted_.data(), ptr, deleted_size * sizeof(TxnTimeStamp));
    ptr += deleted_.size() * sizeof(TxnTimeStamp);
    Assert<StorageException>(ptr - buf.data() == buf_len, "Failed to load block_version file: " + version_path);
}

void BlockVersion::SaveToFile(const String &version_path) {
    int32_t exp_size = sizeof(int32_t) + created_.size() * sizeof(Pair<TxnTimeStamp, int32_t>);
    exp_size += sizeof(int32_t) + deleted_.size() * sizeof(TxnTimeStamp);
    Vector<char> buf(exp_size);
    char *ptr = buf.data();
    WriteBufAdv<int32_t>(ptr, int32_t(created_.size()));
    WriteBufAdv<int32_t>(ptr, int32_t(deleted_.size()));
    Memcpy(ptr, created_.data(), created_.size() * sizeof(Pair<TxnTimeStamp, int32_t>));
    ptr += created_.size() * sizeof(Pair<TxnTimeStamp, int32_t>);
    Memcpy(ptr, deleted_.data(), deleted_.size() * sizeof(TxnTimeStamp));
    ptr += deleted_.size() * sizeof(TxnTimeStamp);
    Assert<StorageException>(ptr - buf.data() == exp_size, "Failed to save block_version file: " + version_path);
    std::ofstream ofs = std::ofstream(version_path, std::ios::trunc | std::ios::binary);
    if (!ofs.is_open()) {
        Error<StorageException>("Failed to open block_version file: " + version_path);
    }
    ofs.write(buf.data(), ptr - buf.data());
    ofs.flush();
    ofs.close();
}

BlockEntry::BlockEntry(const SegmentEntry *segment_entry, u16 block_id, TxnTimeStamp checkpoint_ts, u64 column_count, BufferManager *buffer_mgr)
    : BaseEntry(EntryType::kBlock), segment_entry_(segment_entry), block_id_(block_id), checkpoint_ts_(checkpoint_ts),
      row_capacity_(DEFAULT_VECTOR_SIZE), row_count_(0) {
    base_dir_ = BlockEntry::DetermineDir(*segment_entry->segment_dir_, block_id);
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
    : BaseEntry(EntryType::kBlock), segment_entry_(segment_entry), block_id_(block_id), checkpoint_ts_(checkpoint_ts),
      row_capacity_(DEFAULT_VECTOR_SIZE), row_count_(row_count_), min_row_ts_(min_row_ts_), max_row_ts_(max_row_ts_) {

    base_dir_ = BlockEntry::DetermineDir(*segment_entry->segment_dir_, block_id);

    columns_.reserve(column_count);
    for (SizeT column_id = 0; column_id < column_count; ++column_id) {
        // For replay purposes not create a new column buffer handler
        columns_.emplace_back(BlockColumnEntry::MakeNewBlockColumnEntry(this, column_id, buffer_mgr, true));
    }

    block_version_ = MakeUnique<BlockVersion>(row_capacity_);
    block_version_->created_.emplace_back(std::make_pair(min_row_ts_, row_count_));
}

Pair<u16, u16> BlockEntry::VisibleRange(BlockEntry *block_entry, TxnTimeStamp begin_ts, u16 block_offset_begin) {
    auto &block_version = block_entry->block_version_;
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

u16 BlockEntry::AppendData(BlockEntry *block_entry,
                           Txn *txn_ptr,
                           DataBlock *input_data_block,
                           u16 input_block_offset,
                           u16 append_rows,
                           BufferManager *buffer_mgr) {
    UniqueLock<RWMutex> lck(block_entry->rw_locker_);
    Assert<StorageException>(block_entry->txn_ptr_ == nullptr || block_entry->txn_ptr_ == txn_ptr,
                             Format("Multiple transactions are changing data of Segment: {}, Block: {}",
                                    block_entry->segment_entry_->segment_id_,
                                    block_entry->block_id_));
    block_entry->txn_ptr_ = txn_ptr;
    u16 actual_copied = append_rows;
    if (block_entry->row_count_ + append_rows > block_entry->row_capacity_) {
        actual_copied = block_entry->row_capacity_ - block_entry->row_count_;
    }

    SizeT column_count = block_entry->columns_.size();
    for (SizeT column_id = 0; column_id < column_count; ++column_id) {
        BlockColumnEntry::Append(block_entry->columns_[column_id].get(),
                                 block_entry->row_count_,
                                 input_data_block->column_vectors[column_id].get(),
                                 input_block_offset,
                                 actual_copied);

        LOG_TRACE(Format("Segment: {}, Block: {}, Column: {} is appended with {} rows",
                         block_entry->segment_entry_->segment_id_,
                         block_entry->block_id_,
                         column_id,
                         actual_copied));
    }

    block_entry->row_count_ += actual_copied;
    return actual_copied;
}

void BlockEntry::DeleteData(BlockEntry *block_entry, Txn *txn_ptr, const Vector<RowID> &rows) {
    UniqueLock<RWMutex> lck(block_entry->rw_locker_);
    Assert<StorageException>(block_entry->txn_ptr_ == nullptr || block_entry->txn_ptr_ == txn_ptr,
                             Format("Multiple transactions are changing data of Segment: {}, Block: {}",
                                    block_entry->segment_entry_->segment_id_,
                                    block_entry->block_id_));

    String *table_collect_name_ptr = block_entry->segment_entry_->table_entry_->table_collection_name_.get();
    u32 segment_id = block_entry->segment_entry_->segment_id_;
    u16 block_id = block_entry->block_id_;

    block_entry->txn_ptr_ = txn_ptr;
    auto &block_version = block_entry->block_version_;
    for (RowID row_id : rows) {
        u16 block_offset = row_id.segment_offset_ % DEFAULT_BLOCK_CAPACITY;
        block_version->deleted_[block_offset] = txn_ptr->CommitTS();
    }

    LOG_TRACE(Format("Table {} Segment {} Block {} has deleted {} rows", *table_collect_name_ptr, segment_id, block_id, rows.size()));
}

// A txn may invoke AppendData() multiple times, and then invoke CommitAppend() once.
void BlockEntry::CommitAppend(BlockEntry *block_entry, Txn *txn_ptr) {
    UniqueLock<RWMutex> lck(block_entry->rw_locker_);
    Assert<StorageException>(block_entry->txn_ptr_ == txn_ptr,
                             Format("Multiple transactions are changing data of Segment: {}, Block: {}",
                                    block_entry->segment_entry_->segment_id_,
                                    block_entry->block_id_));
    block_entry->txn_ptr_ = nullptr;
    TxnTimeStamp commit_ts = txn_ptr->CommitTS();
    if (block_entry->min_row_ts_ == 0) {
        block_entry->min_row_ts_ = commit_ts;
    }
    block_entry->max_row_ts_ = Max(block_entry->max_row_ts_, commit_ts);

    auto &block_version = block_entry->block_version_;
    block_version->created_.push_back({commit_ts, int32_t(block_entry->row_count_)});
}

void BlockEntry::CommitDelete(BlockEntry *block_entry, Txn *txn_ptr) {
    UniqueLock<RWMutex> lck(block_entry->rw_locker_);
    Assert<StorageException>(
        block_entry->txn_ptr_ == nullptr || block_entry->txn_ptr_ == txn_ptr,
        Format("Expect txn_ptr_ not be nullptr of Segment: {}, Block: {}", block_entry->segment_entry_->segment_id_, block_entry->block_id_));
    if (block_entry->txn_ptr_ == nullptr)
        return;
    block_entry->txn_ptr_ = nullptr;
    TxnTimeStamp commit_ts = txn_ptr->CommitTS();
    if (block_entry->min_row_ts_ == 0) {
        block_entry->min_row_ts_ = commit_ts;
    }
    block_entry->max_row_ts_ = Max(block_entry->max_row_ts_, commit_ts);
}

void BlockEntry::FlushData(BlockEntry *block_entry, int64_t checkpoint_row_count) {
    for (auto &column_data : block_entry->columns_) {
        column_data->Flush(column_data.get(), checkpoint_row_count);
        LOG_TRACE(Format("ColumnData {} is flushed", column_data->column_id_));
    }
}

void BlockEntry::FlushVersion(BlockEntry *block_entry, BlockVersion &checkpoint_version) {
    checkpoint_version.SaveToFile(*block_entry->base_dir_ + "/version");
}

void BlockEntry::Flush(BlockEntry *block_entry, TxnTimeStamp checkpoint_ts) {
    LOG_TRACE(Format("Segment: {}, Block: {} is being flushing", block_entry->segment_entry_->segment_id_, block_entry->block_id_));
    Assert<StorageException>(checkpoint_ts >= block_entry->checkpoint_ts_, "BlockEntry checkpoint_ts skew!");
    int checkpoint_row_count = 0;

    BlockVersion checkpoint_version(block_entry->block_version_->deleted_.size());
    {
        SharedLock<RWMutex> lock(block_entry->rw_locker_);
        // Skip if entry has been flushed at some previous checkpoint, or is invisible at current checkpoint.
        if (block_entry->max_row_ts_ <= block_entry->checkpoint_ts_ || block_entry->min_row_ts_ > checkpoint_ts)
            return;

        checkpoint_row_count = block_entry->block_version_->GetRowCount(checkpoint_ts);
        Assert<StorageException>(checkpoint_row_count > 0, "BlockEntry is empty at checkpoint_ts!");
        const Vector<TxnTimeStamp> &deleted = block_entry->block_version_->deleted_;
        if (checkpoint_row_count <= block_entry->checkpoint_row_count_) {
            // BlockEntry doesn't append rows between the previous checkpoint and checkpoint_ts.
            bool deleted_between = false;
            for (int i = 0; i < checkpoint_row_count; i++) {
                if (deleted[i] > block_entry->checkpoint_ts_ && deleted[i] <= checkpoint_ts) {
                    deleted_between = true;
                    break;
                }
            }
            if (!deleted_between) // BlockEntry doesn't change between the previous checkpoint and checkpoint_ts.
                return;
        }
        checkpoint_version.created_ = block_entry->block_version_->created_;
        checkpoint_version.deleted_.reserve(checkpoint_row_count);
        Memcpy(checkpoint_version.deleted_.data(), deleted.data(), checkpoint_row_count * sizeof(TxnTimeStamp));
    }
    for (int i = 0; i < checkpoint_row_count; i++) {
        if (checkpoint_version.deleted_[i] > checkpoint_ts) {
            checkpoint_version.deleted_[i] = 0;
        }
    }

    FlushVersion(block_entry, checkpoint_version);
    FlushData(block_entry, checkpoint_row_count);
    block_entry->checkpoint_ts_ = checkpoint_ts;
    block_entry->checkpoint_row_count_ = checkpoint_row_count;
    LOG_TRACE(Format("Segment: {}, Block {} is flushed", block_entry->segment_entry_->segment_id_, block_entry->block_id_));
    return;
}

// TODO: introduce BlockColumnMeta
Json BlockEntry::Serialize(BlockEntry *block_entry, TxnTimeStamp max_commit_ts) {
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
    block_entry->block_version_->LoadFromFile(*block_entry->base_dir_ + "/version");
    // if not found version file
    if (block_entry->block_version_->created_.empty()) {
        block_entry->block_version_->created_.emplace_back(block_entry->max_row_ts_, block_entry->row_count_);
        // FIXME Need to flush the version file?
        // FlushVersion(block_entry.get(), *block_entry->block_version_);
    }

    return block_entry;
}

i32 BlockEntry::Room(BlockEntry *block_entry) {
    SharedLock<RWMutex> lck(block_entry->rw_locker_);
    return block_entry->row_capacity_ - block_entry->row_count_;
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
    Assert<StorageException>(block_entry2 != nullptr, "MergeFrom requires the same type of BaseEntry");
    // No locking here since only the load stage needs MergeFrom.
    Assert<StorageException>(*this->base_dir_ == *block_entry2->base_dir_, "BlockEntry::MergeFrom requires base_dir_ match");
    Assert<StorageException>(this->block_id_ == block_entry2->block_id_, "BlockEntry::MergeFrom requires block_id_ match");
    Assert<StorageException>(this->row_capacity_ == block_entry2->row_capacity_, "BlockEntry::MergeFrom requires row_capacity_ match");
    Assert<StorageException>(this->min_row_ts_ == block_entry2->min_row_ts_, "BlockEntry::MergeFrom requires min_row_ts_ match");

    if (this->checkpoint_ts_ >= block_entry2->checkpoint_ts_)
        return;
    this->row_count_ = block_entry2->row_count_;
    this->columns_.swap(block_entry2->columns_);
    *this->block_version_ = *block_entry2->block_version_;
    this->max_row_ts_ = block_entry2->max_row_ts_;
    this->checkpoint_ts_ = block_entry2->checkpoint_ts_;
    this->checkpoint_row_count_ = block_entry2->checkpoint_row_count_;
}

} // namespace infinity
