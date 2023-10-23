//
// Created by jinhai on 23-10-2.
//

module;

#include <memory>

import stl;
import base_entry;
import segment_entry;
import default_values;
import block_column_entry;
import logger;
import third_party;
import defer_op;
import local_file_system;

module block_entry;

namespace infinity {

BlockEntry::BlockEntry(const SegmentEntry *segment_entry, i16 block_id, u64 column_count, SizeT start_row, BufferManager *buffer_mgr)
    : BaseEntry(EntryType::kBlock), segment_entry_(segment_entry), block_id_(block_id), start_row_(start_row), row_capacity_(DEFAULT_VECTOR_SIZE),
      row_count_(0) {
    base_dir_ = BlockEntry::DetermineFilename(*segment_entry->segment_dir_, block_id);
    columns_.reserve(column_count);
    for (SizeT column_id = 0; column_id < column_count; ++column_id) {
        columns_.emplace_back(BlockColumnEntry::MakeNewBlockColumnEntry(this, column_id, buffer_mgr));
    }

    block_version_ = MakeUnique<BlockVersion>(row_capacity_);
}

i16 BlockEntry::AppendData(BlockEntry *block_entry, Txn *txn_ptr, DataBlock *input_data_block, offset_t input_offset, i16 append_rows) {
    block_entry->rw_locker_.lock();
    DeferFn defer_fn([&]() { block_entry->rw_locker_.unlock(); });

    i16 actual_copied = append_rows;
    if (block_entry->row_count_ + append_rows > block_entry->row_capacity_) {
        actual_copied = block_entry->row_capacity_ - block_entry->row_count_;
    }

    SizeT column_count = block_entry->columns_.size();
    for (SizeT column_id = 0; column_id < column_count; ++column_id) {
        BlockColumnEntry::Append(block_entry->columns_[column_id].get(),
                                 block_entry->row_count_,
                                 input_data_block->column_vectors[column_id].get(),
                                 input_offset,
                                 actual_copied);

        LOG_TRACE(Format("Segment: {}, Block: {}, Column: {} is appended with {} rows",
                         block_entry->segment_entry_->segment_id_,
                         block_entry->block_id_,
                         column_id,
                         actual_copied));
    }

    for (SizeT i = 0; i < actual_copied; ++i) {
        block_entry->block_version_->txn_ptr_[i] = (u64)txn_ptr;
    }

    block_entry->row_count_ += actual_copied;
    return actual_copied;
}

void BlockEntry::CommitAppend(BlockEntry *block_entry, u64 committing_txn_id) {
    block_entry->rw_locker_.lock();
    DeferFn defer_fn([&]() { block_entry->rw_locker_.unlock(); });

    if (block_entry->min_row_ts_ == MAX_TXN_ID) {
        block_entry->min_row_ts_ = committing_txn_id;
    }

    if (committing_txn_id < block_entry->min_row_ts_) {
        block_entry->min_row_ts_ = committing_txn_id;
    }

    if (block_entry->max_row_ts_ == MAX_TXN_ID) {
        block_entry->max_row_ts_ = committing_txn_id;
    }

    if (committing_txn_id > block_entry->max_row_ts_) {
        block_entry->max_row_ts_ = committing_txn_id;
    }

    if (block_entry->txn_ptr_ != nullptr) {
        block_entry->start_ts_ = committing_txn_id;
        block_entry->txn_ptr_ = nullptr;
    }
}

void BlockEntry::CommitDelete(BlockEntry *block_entry, Txn *txn_ptr, u64 start_row_in_segment, u64 row_count) {
    u64 start_offset = start_row_in_segment - block_entry->start_row_;
    u64 end_offset = start_offset + row_count;
    Vector<au64> &deleted_vector = block_entry->block_version_->deleted_;
    for (SizeT i = start_offset; i < end_offset; ++i) {
        deleted_vector[i] = txn_ptr->CommitTS();
    }
}

bool BlockEntry::PrepareFlush(BlockEntry *block_entry) {
    BlockEntryStatus expected = BlockEntryStatus::kBlockOpen;
    return block_entry->status_.compare_exchange_strong(expected, BlockEntryStatus::kBlockFlushing, std::memory_order_seq_cst);
}

bool BlockEntry::Flush(BlockEntry *block_entry) {
    LOG_TRACE(Format("Segment: {}, Block: {} is being flushing", block_entry->segment_entry_->segment_id_, block_entry->block_id_));
    for (SizeT column_id = 0; const auto &column_data : block_entry->columns_) {
        column_data->Flush(column_data.get(), block_entry->row_count_);
        LOG_TRACE(Format("ColumnData: {} is flushed", column_id));
        ++column_id;
    }

    BlockEntryStatus expected = BlockEntryStatus::kBlockFlushing;
    if (!block_entry->status_.compare_exchange_strong(expected, BlockEntryStatus::kBlockClosed, std::memory_order_seq_cst)) {
        LOG_WARN("Data block is expected as flushing status");
        return false;
    }
    LOG_TRACE(Format("Segment: {}, Block: {} is being flushed", block_entry->segment_entry_->segment_id_, block_entry->block_id_));

    return true;
}

Json BlockEntry::Serialize(const BlockEntry *block_entry) {
    Json json_res;

    json_res["start_row"] = block_entry->start_row_;
    json_res["row_count"] = block_entry->row_count_;
    json_res["row_capacity"] = block_entry->row_capacity_;
    json_res["block_id"] = block_entry->block_id_;
    json_res["block_dir"] = *block_entry->base_dir_;
    for (const auto &block_column_entry : block_entry->columns_) {
        json_res["columns"].emplace_back(BlockColumnEntry::Serialize(block_column_entry.get()));
    }
    json_res["min_row_ts"] = block_entry->min_row_ts_.load();
    json_res["max_row_ts"] = block_entry->max_row_ts_.load();
    return json_res;
}

UniquePtr<BlockEntry> BlockEntry::Deserialize(const Json &block_entry_json, SegmentEntry *segment_entry, BufferManager *buffer_mgr) {
    u64 block_id = block_entry_json["block_id"];
    SizeT start_row = block_entry_json["start_row"];
    UniquePtr<BlockEntry> block_entry = MakeUnique<BlockEntry>(segment_entry, block_id, 0, start_row, buffer_mgr);

    block_entry->row_capacity_ = block_entry_json["row_capacity"];
    block_entry->row_count_ = block_entry_json["row_count"];
    block_entry->min_row_ts_ = block_entry_json["min_row_ts"];
    block_entry->max_row_ts_ = block_entry_json["max_row_ts"];

    for (const auto &block_column_json : block_entry_json["columns"]) {
        block_entry->columns_.emplace_back(BlockColumnEntry::Deserialize(block_column_json, block_entry.get(), buffer_mgr));
    }

    return block_entry;
}

bool BlockEntry::IsFull(BlockEntry *block_entry) { return block_entry->row_count_ >= block_entry->row_capacity_; }

SharedPtr<String> BlockEntry::DetermineFilename(const String &parent_dir, u64 block_id) {
    LocalFileSystem fs;
    SharedPtr<String> base_dir;
    base_dir = MakeShared<String>(parent_dir + '/' + "blk_" + std::to_string(block_id));
    fs.CreateDirectoryNoExp(*base_dir);
    return base_dir;
}

} // namespace infinity
