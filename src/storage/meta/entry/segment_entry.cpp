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

#include <ctime>
#include <stdexcept>
#include <string>
#include <vector>

module segment_entry;

import stl;
import third_party;
import buffer_manager;
import default_values;
import data_block;
import txn;
import data_access_state;

import infinity_exception;
import defer_op;
import buffer_handle;
import logger;
import local_file_system;
import random;

import txn_store;
import segment_iter;
import catalog_delta_entry;
import status;
import compact_segments_task;
import cleanup_scanner;

namespace infinity {

SegmentEntry::SegmentEntry(const TableEntry *table_entry,
                           SharedPtr<String> segment_dir,
                           SegmentID segment_id,
                           SizeT row_capacity,
                           SizeT column_count,
                           SegmentStatus status)
    : BaseEntry(EntryType::kSegment), table_entry_(table_entry), segment_dir_(segment_dir), segment_id_(segment_id), row_capacity_(row_capacity),
      column_count_(column_count), status_(status) {}

SharedPtr<SegmentEntry> SegmentEntry::NewSegmentEntry(const TableEntry *table_entry, SegmentID segment_id, Txn *txn, bool sealed) {
    SharedPtr<SegmentEntry> segment_entry = MakeShared<SegmentEntry>(table_entry,
                                                                     SegmentEntry::DetermineSegmentDir(*table_entry->TableEntryDir(), segment_id),
                                                                     segment_id,
                                                                     DEFAULT_SEGMENT_CAPACITY,
                                                                     table_entry->ColumnCount(),
                                                                     sealed ? SegmentStatus::kSealed : SegmentStatus::kUnsealed);
    auto operation = MakeUnique<AddSegmentEntryOp>(segment_entry.get());
    txn->AddCatalogDeltaOperation(std::move(operation));
    segment_entry->begin_ts_ = txn->BeginTS();

    return segment_entry;
}

SharedPtr<SegmentEntry> SegmentEntry::NewReplaySegmentEntry(const TableEntry *table_entry,
                                                            SegmentID segment_id,
                                                            const SharedPtr<String> &segment_dir,
                                                            TxnTimeStamp commit_ts) {
    auto segment_entry =
        MakeShared<SegmentEntry>(table_entry, segment_dir, segment_id, DEFAULT_SEGMENT_CAPACITY, table_entry->ColumnCount(), SegmentStatus::kSealed);
    segment_entry->min_row_ts_ = commit_ts;
    return segment_entry;
}

SharedPtr<SegmentEntry> SegmentEntry::NewReplayCatalogSegmentEntry(const TableEntry *table_entry,
                                                                   SegmentID segment_id,
                                                                   const SharedPtr<String> &segment_dir,
                                                                   u64 column_count,
                                                                   SizeT row_count,
                                                                   SizeT actual_row_count,
                                                                   SizeT row_capacity,
                                                                   TxnTimeStamp min_row_ts,
                                                                   TxnTimeStamp max_row_ts,
                                                                   TxnTimeStamp commit_ts,
                                                                   TxnTimeStamp begin_ts,
                                                                   TransactionID txn_id) {
    auto segment_entry = MakeShared<SegmentEntry>(table_entry, segment_dir, segment_id, row_capacity, column_count, SegmentStatus::kSealed);
    segment_entry->min_row_ts_ = min_row_ts;
    segment_entry->max_row_ts_ = max_row_ts;
    segment_entry->commit_ts_ = commit_ts;
    segment_entry->begin_ts_ = begin_ts;
    segment_entry->row_count_ = row_count;
    segment_entry->actual_row_count_ = actual_row_count;
    segment_entry->txn_id_ = txn_id;
    return segment_entry;
}

void SegmentEntry::SetSealed() {
    std::unique_lock lock(rw_locker_);
    if (status_ != SegmentStatus::kUnsealed) {
        UnrecoverableError("SetSealed failed");
    }
    status_ = SegmentStatus::kSealed;
}

bool SegmentEntry::TrySetCompacting(CompactSegmentsTask *compact_task) {
    std::unique_lock lock(rw_locker_);
    if (status_ == SegmentStatus::kUnsealed) {
        UnrecoverableError("Assert: Compactable segment should be sealed.");
    }
    if (status_ != SegmentStatus::kSealed) {
        return false;
    }
    compact_task_ = compact_task;
    status_ = SegmentStatus::kCompacting;
    return true;
}

void SegmentEntry::SetNoDelete() {
    std::unique_lock lock(rw_locker_);
    if (status_ != SegmentStatus::kCompacting) {
        UnrecoverableError("Assert: kNoDelete is only allowed to set on compacting segment.");
    }
    status_ = SegmentStatus::kNoDelete;
    no_delete_complete_cv_.wait(lock, [this] { return delete_txns_.empty(); });
    compact_task_ = nullptr;
}

void SegmentEntry::SetDeprecated(TxnTimeStamp deprecate_ts) {
    std::unique_lock lock(rw_locker_);
    if (status_ != SegmentStatus::kNoDelete) {
        UnrecoverableError("Assert: kDeprecated is only allowed to set on kNoDelete segment.");
    }
    status_ = SegmentStatus::kDeprecated;
    deprecate_ts_ = deprecate_ts;
}

void SegmentEntry::RollbackCompact() {
    std::unique_lock lock(rw_locker_);
    if (status_ != SegmentStatus::kNoDelete) {
        UnrecoverableError("Assert: Rollbacked segment should be in No Delete state.");
    }
    status_ = SegmentStatus::kSealed;
    deprecate_ts_ = UNCOMMIT_TS;
}

bool SegmentEntry::CheckDeleteConflict(Vector<Pair<SegmentEntry *, Vector<SegmentOffset>>> &&segments, TransactionID txn_id) {
    // hold all locks and check
    Vector<std::shared_lock<std::shared_mutex>> locks;
    for (const auto &[segment_entry, delete_offsets] : segments) {
        locks.emplace_back(segment_entry->rw_locker_);
        if (segment_entry->status_ == SegmentStatus::kDeprecated || segment_entry->status_ == SegmentStatus::kNoDelete) {
            return true;
        }
    }
    for (const auto &[segment_entry, delete_offsets] : segments) {
        segment_entry->delete_txns_.insert(txn_id);
    }
    return false;
}

bool SegmentEntry::CheckRowVisible(SegmentOffset segment_offset, TxnTimeStamp check_ts) const {
    // FIXME: get the block_capacity from config?
    u32 block_capacity = DEFAULT_BLOCK_CAPACITY;
    BlockID block_id = segment_offset / block_capacity;
    BlockOffset block_offset = segment_offset % block_capacity;

    auto *block_entry = GetBlockEntryByID(block_id);
    return block_entry->CheckRowVisible(block_offset, check_ts);
}

bool SegmentEntry::CheckVisible(TxnTimeStamp check_ts) const {
    std::shared_lock lock(rw_locker_);
    return min_row_ts_ <= check_ts && check_ts <= deprecate_ts_;
}

bool SegmentEntry::CheckAnyDelete(TxnTimeStamp check_ts) const {
    std::shared_lock lock(rw_locker_);
    return first_delete_ts_ < check_ts;
}

// called by one thread
BlockID SegmentEntry::GetNextBlockID() const { return block_entries_.size(); }

// called by one thread
Pair<SizeT, BlockOffset> SegmentEntry::GetWalInfo() const { return {block_entries_.size(), block_entries_.back()->row_count()}; }

void SegmentEntry::AppendBlockEntry(UniquePtr<BlockEntry> block_entry) {
    std::unique_lock lock(this->rw_locker_);
    IncreaseRowCount(block_entry->row_count());
    block_entries_.emplace_back(std::move(block_entry));
}

// One writer
u64 SegmentEntry::AppendData(TransactionID txn_id, AppendState *append_state_ptr, BufferManager *buffer_mgr, Txn *txn) {
    std::unique_lock lck(this->rw_locker_); // FIXME: lock scope too large
    if (this->status_ != SegmentStatus::kUnsealed) {
        UnrecoverableError("AppendData to sealed/compacting/no_delete/deprecated segment");
    }
    if (this->row_capacity_ - this->row_count_ <= 0) {
        return 0;
    }
    //    SizeT start_row = this->row_count_;
    SizeT append_block_count = append_state_ptr->blocks_.size();
    u64 total_copied{0};

    while (append_state_ptr->current_block_ < append_block_count && this->row_count_ < this->row_capacity_) {
        DataBlock *input_block = append_state_ptr->blocks_[append_state_ptr->current_block_].get();

        u16 to_append_rows = input_block->row_count();
        while (to_append_rows > 0) {
            // Append to_append_rows into block
            if (block_entries_.empty() || block_entries_.back()->GetAvailableCapacity() <= 0) {
                BlockID new_block_id = this->block_entries_.size();
                this->block_entries_.emplace_back(BlockEntry::NewBlockEntry(this, new_block_id, 0, this->column_count_, txn));
            }
            BlockEntry *last_block_entry = this->block_entries_.back().get();

            SegmentID range_segment_id = this->segment_id_;
            BlockID range_block_id = last_block_entry->block_id();
            u16 range_block_start_row = last_block_entry->row_count();

            u16 actual_appended =
                last_block_entry->AppendData(txn_id, input_block, append_state_ptr->current_block_offset_, to_append_rows, buffer_mgr);
            if (to_append_rows < actual_appended) {
                UnrecoverableError(fmt::format("Attempt to append rows: {}, but rows: {} are appended", to_append_rows, actual_appended));
            }

            append_state_ptr->append_ranges_.emplace_back(range_segment_id, range_block_id, range_block_start_row, actual_appended);

            total_copied += actual_appended;
            to_append_rows -= actual_appended;
            append_state_ptr->current_count_ += actual_appended;
            IncreaseRowCount(actual_appended);
            if (this->row_count_ > this->row_capacity_) {
                UnrecoverableError("Not implemented: append data exceed segment row capacity");
            }
        }
        if (to_append_rows == 0) {
            append_state_ptr->current_block_++;
            append_state_ptr->current_block_offset_ = 0;
        }
    }
    return total_copied;
}

// One writer
void SegmentEntry::DeleteData(TransactionID txn_id, TxnTimeStamp commit_ts, const HashMap<BlockID, Vector<BlockOffset>> &block_row_hashmap) {
    for (const auto &[block_id, delete_rows] : block_row_hashmap) {
        BlockEntry *block_entry = nullptr;
        {
            std::shared_lock lck(this->rw_locker_);
            block_entry = block_entries_.at(block_id).get();
        }

        block_entry->DeleteData(txn_id, commit_ts, delete_rows);
    }
}

// One writer
void SegmentEntry::CommitAppend(TransactionID txn_id, TxnTimeStamp commit_ts, u16 block_id, u16, u16) {
    BlockEntry *block_entry = nullptr;
    {
        std::unique_lock lck(this->rw_locker_);
        if (this->status_ != SegmentStatus::kUnsealed) {
            UnrecoverableError("Assert: Should not commit append to sealed segment.");
        }
        if (this->min_row_ts_ == UNCOMMIT_TS) {
            this->min_row_ts_ = commit_ts;
        }
        this->max_row_ts_ = std::max(this->max_row_ts_, commit_ts);
        block_entry = this->block_entries_.at(block_id).get();
    }
    block_entry->CommitAppend(txn_id, commit_ts);
}

// One writer
void SegmentEntry::CommitDelete(TransactionID txn_id, TxnTimeStamp commit_ts, const HashMap<u16, Vector<BlockOffset>> &block_row_hashmap) {
    Vector<BlockEntry *> delete_blocks;
    delete_blocks.reserve(block_row_hashmap.size());
    {
        std::unique_lock lck(this->rw_locker_);
        this->max_row_ts_ = std::max(this->max_row_ts_, commit_ts);
        if (this->first_delete_ts_ == UNCOMMIT_TS) {
            this->first_delete_ts_ = commit_ts;
        }
        if (status_ == SegmentStatus::kDeprecated) {
            UnrecoverableError("Assert: Should not commit delete to deprecated segment.");
        }
        if (compact_task_ != nullptr) {
            if (status_ != SegmentStatus::kCompacting && status_ != SegmentStatus::kNoDelete) {
                UnrecoverableError("Assert: compact_task is not nullptr means segment is being compacted");
            }
            Vector<SegmentOffset> segment_offsets;
            for (const auto &[block_id, delete_rows] : block_row_hashmap) {
                for (BlockOffset block_offset : delete_rows) {
                    SegmentOffset segment_offset = block_id * DEFAULT_BLOCK_CAPACITY + block_offset;
                    segment_offsets.push_back(segment_offset);
                }
            }
            compact_task_->AddToDelete(segment_id_, std::move(segment_offsets));
        }
        {
            delete_txns_.erase(txn_id);
            if (delete_txns_.empty()) { // == 0 when replay
                no_delete_complete_cv_.notify_one();
            }
        }
        for (const auto &[block_id, delete_rows] : block_row_hashmap) {
            BlockEntry *block_entry = block_entries_.at(block_id).get();
            if (delete_rows.size() > block_entry->row_capacity()) {
                UnrecoverableError("Delete rows exceed block capacity");
            }
            DecreaseRemainRow(delete_rows.size());
            delete_blocks.push_back(block_entry);
        }
    }

    for (auto *block_entry : delete_blocks) {
        block_entry->CommitDelete(txn_id, commit_ts);
    }
}

BlockEntry *SegmentEntry::GetBlockEntryByID(BlockID block_id) const {
    std::shared_lock lock(rw_locker_);
    if (block_id >= block_entries_.size()) {
        return nullptr;
    }
    return block_entries_[block_id].get();
}

nlohmann::json SegmentEntry::Serialize(TxnTimeStamp max_commit_ts, bool is_full_checkpoint) {
    nlohmann::json json_res;

    // const field
    json_res["segment_dir"] = *this->segment_dir_;
    json_res["row_capacity"] = this->row_capacity_;
    json_res["segment_id"] = this->segment_id_;
    json_res["column_count"] = this->column_count_;
    {
        std::shared_lock<std::shared_mutex> lck(this->rw_locker_);

        json_res["min_row_ts"] = this->min_row_ts_;
        json_res["max_row_ts"] = this->max_row_ts_;
        json_res["deleted"] = this->deleted_;
        json_res["row_count"] = this->row_count_;
        json_res["actual_row_count"] = this->actual_row_count_;

        json_res["commit_ts"] = TxnTimeStamp(this->commit_ts_);
        json_res["begin_ts"] = TxnTimeStamp(this->begin_ts_);
        json_res["txn_id"] = TransactionID(this->txn_id_);

        for (auto &block_entry : this->block_entries_) {
            json_res["block_entries"].emplace_back(block_entry->Serialize(max_commit_ts));
        }
    }
    return json_res;
}

SharedPtr<SegmentEntry> SegmentEntry::Deserialize(const nlohmann::json &segment_entry_json, TableEntry *table_entry, BufferManager *buffer_mgr) {
    SharedPtr<SegmentEntry> segment_entry = MakeShared<SegmentEntry>(table_entry,
                                                                     MakeShared<String>(segment_entry_json["segment_dir"]),
                                                                     segment_entry_json["segment_id"],
                                                                     segment_entry_json["row_capacity"],
                                                                     segment_entry_json["column_count"],
                                                                     SegmentStatus::kSealed);
    segment_entry->min_row_ts_ = segment_entry_json["min_row_ts"];
    segment_entry->max_row_ts_ = segment_entry_json["max_row_ts"];
    segment_entry->deleted_ = segment_entry_json["deleted"];
    segment_entry->row_count_ = segment_entry_json["row_count"];
    segment_entry->actual_row_count_ = segment_entry_json["actual_row_count"];

    segment_entry->commit_ts_ = segment_entry_json["commit_ts"];
    segment_entry->begin_ts_ = segment_entry_json["begin_ts"];
    segment_entry->txn_id_ = segment_entry_json["txn_id"];

    if (segment_entry_json.contains("block_entries")) {
        for (const auto &block_json : segment_entry_json["block_entries"]) {
            UniquePtr<BlockEntry> block_entry = BlockEntry::Deserialize(block_json, segment_entry.get(), buffer_mgr);
            auto block_entries_size = segment_entry->block_entries_.size();
            segment_entry->block_entries_.resize(std::max(block_entries_size, static_cast<SizeT>(block_entry->block_id() + 1)));
            segment_entry->block_entries_[block_entry->block_id()] = std::move(block_entry);
        }
    }
    LOG_TRACE(fmt::format("Segment: {}, Block count: {}", segment_entry->segment_id_, segment_entry->block_entries_.size()));

    return segment_entry;
}

void SegmentEntry::FlushDataToDisk(TxnTimeStamp max_commit_ts, bool is_full_checkpoint) {
    auto block_entry_iter = BlockEntryIter(this);
    for (auto *block_entry = block_entry_iter.Next(); block_entry != nullptr; block_entry = block_entry_iter.Next()) {
        block_entry->Flush(max_commit_ts);
    }
}

void SegmentEntry::FlushNewData() {
    for (const auto &block_entry : this->block_entries_) {
        block_entry->FlushData(block_entry->row_count());
    }
}

SharedPtr<String> SegmentEntry::DetermineSegmentDir(const String &parent_dir, u32 seg_id) {
    LocalFileSystem fs;
    SharedPtr<String> segment_dir;
    do {
        u32 seed = time(nullptr);
        segment_dir = MakeShared<String>(parent_dir + '/' + RandomString(DEFAULT_RANDOM_NAME_LEN, seed) + "_seg_" + std::to_string(seg_id));
    } while (!fs.CreateDirectoryNoExp(*segment_dir));
    return segment_dir;
}

void SegmentEntry::MergeFrom(BaseEntry &other) {
    auto segment_entry2 = dynamic_cast<SegmentEntry *>(&other);
    if (segment_entry2 == nullptr) {
        UnrecoverableError("MergeFrom requires the same type of BaseEntry");
    }
    if (*this->segment_dir_ != *segment_entry2->segment_dir_) {
        UnrecoverableError("SegmentEntry::MergeFrom requires segment_dir_ match");
    }
    if (this->segment_id_ != segment_entry2->segment_id_) {
        UnrecoverableError("SegmentEntry::MergeFrom requires segment_id_ match");
    }
    if (this->column_count_ != segment_entry2->column_count_) {
        UnrecoverableError("SegmentEntry::MergeFrom requires column_count_ match");
    }
    if (this->row_capacity_ != segment_entry2->row_capacity_) {
        UnrecoverableError("SegmentEntry::MergeFrom requires row_capacity_ match");
    }
    if (this->min_row_ts_ != segment_entry2->min_row_ts_) {
        UnrecoverableError("SegmentEntry::MergeFrom requires min_row_ts_ match");
    }
    if (this->block_entries_.size() > segment_entry2->block_entries_.size()) {
        UnrecoverableError("SegmentEntry::MergeFrom requires source segment entry blocks not more than segment entry blocks");
    }

    this->row_count_ = std::max(this->row_count_, segment_entry2->row_count_);
    this->max_row_ts_ = std::max(this->max_row_ts_, segment_entry2->max_row_ts_);

    SizeT block_count = this->block_entries_.size();
    SizeT idx = 0;
    for (; idx < block_count; ++idx) {
        auto &block_entry1 = this->block_entries_[idx];
        auto &block_entry2 = segment_entry2->block_entries_[idx];
        if (block_entry1.get() == nullptr) {
            block_entry1 = block_entry2;
        } else if (block_entry2.get() != nullptr) {
            block_entry1->MergeFrom(*block_entry2);
        }
    }

    SizeT segment2_block_count = segment_entry2->block_entries_.size();
    for (; idx < segment2_block_count; ++idx) {
        this->block_entries_.emplace_back(segment_entry2->block_entries_[idx]);
    }

    // for (const auto &[index_name, index_entry] : segment_entry2->index_entry_map_) {
    //     if (this->index_entry_map_.find(index_name) == this->index_entry_map_.end()) {
    //         this->index_entry_map_.emplace(index_name, index_entry);
    //     } else {
    //         this->index_entry_map_[index_name]->MergeFrom(*index_entry);
    //     }
    // }
}

void SegmentEntry::Cleanup() && {
    for (auto &block_entry : block_entries_) {
        block_entry->Cleanup();
    }
    LocalFileSystem fs;
    fs.DeleteEmptyDirectory(*segment_dir_);
}

void SegmentEntry::PickCleanup(CleanupScanner *scanner) {
    // TODO(sys)
}

} // namespace infinity
