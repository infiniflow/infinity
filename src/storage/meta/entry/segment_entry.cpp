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

import txn_store;
import segment_iter;
import catalog_delta_entry;
import status;
import compact_segments_task;
import cleanup_scanner;
import background_process;
import wal_entry;

namespace infinity {

SegmentEntry::SegmentEntry(TableEntry *table_entry,
                           SharedPtr<String> segment_dir,
                           SegmentID segment_id,
                           SizeT row_capacity,
                           SizeT column_count,
                           SegmentStatus status)
    : BaseEntry(EntryType::kSegment, false), table_entry_(table_entry), segment_dir_(segment_dir), segment_id_(segment_id),
      row_capacity_(row_capacity), column_count_(column_count), status_(status) {}

SharedPtr<SegmentEntry> SegmentEntry::NewSegmentEntry(TableEntry *table_entry, SegmentID segment_id, Txn *txn) {
    SharedPtr<SegmentEntry> segment_entry = MakeShared<SegmentEntry>(table_entry,
                                                                     SegmentEntry::DetermineSegmentDir(*table_entry->TableEntryDir(), segment_id),
                                                                     segment_id,
                                                                     DEFAULT_SEGMENT_CAPACITY,
                                                                     table_entry->ColumnCount(),
                                                                     SegmentStatus::kUnsealed);
    segment_entry->begin_ts_ = txn->BeginTS();
    return segment_entry;
}

SharedPtr<SegmentEntry> SegmentEntry::NewReplaySegmentEntry(TableEntry *table_entry,
                                                            SegmentID segment_id,
                                                            SegmentStatus status,
                                                            u64 column_count,
                                                            SizeT row_count,
                                                            SizeT actual_row_count,
                                                            SizeT row_capacity,
                                                            TxnTimeStamp min_row_ts,
                                                            TxnTimeStamp max_row_ts,
                                                            TxnTimeStamp commit_ts,
                                                            TxnTimeStamp deprecate_ts,
                                                            TxnTimeStamp begin_ts,
                                                            TransactionID txn_id) {
    auto segment_dir = SegmentEntry::DetermineSegmentDir(*table_entry->TableEntryDir(), segment_id);
    auto segment_entry = MakeShared<SegmentEntry>(table_entry, std::move(segment_dir), segment_id, row_capacity, column_count, status);
    segment_entry->min_row_ts_ = min_row_ts;
    segment_entry->max_row_ts_ = max_row_ts;
    segment_entry->commit_ts_ = commit_ts;
    segment_entry->deprecate_ts_ = deprecate_ts;
    segment_entry->begin_ts_ = begin_ts;
    segment_entry->row_count_ = row_count;
    segment_entry->actual_row_count_ = actual_row_count;
    segment_entry->txn_id_ = txn_id;
    return segment_entry;
}

bool SegmentEntry::SetSealed() {
    std::unique_lock lock(rw_locker_);
    if (status_ != SegmentStatus::kUnsealed) {
        return false;
    }
    status_ = SegmentStatus::kSealed;
    return true;
}

void SegmentEntry::AddBlockReplay(SharedPtr<BlockEntry> block_entry, BlockID block_id) {
    BlockID cur_blocks_size = block_entries_.size();
    if (block_id >= cur_blocks_size) {
        block_entries_.resize(block_id + 1);
    }
    block_entries_[block_id] = std::move(block_entry);
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

    auto *block_entry = GetBlockEntryByID(block_id).get();
    return block_entry->CheckRowVisible(block_offset, check_ts);
}

bool SegmentEntry::CheckVisible(TxnTimeStamp check_ts) const {
    std::shared_lock lock(rw_locker_);
    return min_row_ts_ <= check_ts && check_ts <= deprecate_ts_;
}

bool SegmentEntry::CheckDeprecate(TxnTimeStamp check_ts) const {
    std::shared_lock lock(rw_locker_);
    return status_ == SegmentStatus::kDeprecated && check_ts > deprecate_ts_;
}

bool SegmentEntry::CheckAnyDelete(TxnTimeStamp check_ts) const {
    std::shared_lock lock(rw_locker_);
    return first_delete_ts_ < check_ts;
}

// called by one thread
BlockID SegmentEntry::GetNextBlockID() const { return block_entries_.size(); }

void SegmentEntry::AppendBlockEntry(UniquePtr<BlockEntry> block_entry) {
    std::unique_lock lock(this->rw_locker_);
    IncreaseRowCount(block_entry->row_count());
    block_entries_.emplace_back(std::move(block_entry));
}

// One writer
u64 SegmentEntry::AppendData(TransactionID txn_id, TxnTimeStamp commit_ts, AppendState *append_state_ptr, BufferManager *buffer_mgr, Txn *txn) {
    TxnTableStore *txn_store = txn->GetTxnTableStore(table_entry_);

    std::unique_lock lck(this->rw_locker_); // FIXME: lock scope too large
    if (this->status_ != SegmentStatus::kUnsealed) {
        UnrecoverableError("AppendData to sealed/compacting/no_delete/deprecated segment");
    }

    if (this->row_capacity_ <= this->row_count_) {
        UnrecoverableError(fmt::format("Segment {} error, row_count {}, capacity {}", segment_id_, row_count_, row_capacity_));
    }

    //    SizeT start_row = this->row_count_;
    SizeT append_block_count = append_state_ptr->blocks_.size();
    u64 total_copied{0};
    Vector<BlockEntry *> mut_blocks;

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
            if (mut_blocks.empty() || last_block_entry->block_id() != mut_blocks.back()->block_id()) {
                mut_blocks.push_back(last_block_entry);
            }

            SegmentID range_segment_id = this->segment_id_;
            BlockID range_block_id = last_block_entry->block_id();
            u16 range_block_start_row = last_block_entry->row_count();

            u16 actual_appended =
                last_block_entry->AppendData(txn_id, commit_ts, input_block, append_state_ptr->current_block_offset_, to_append_rows, buffer_mgr);
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

    for (auto *block_entry : mut_blocks) {
        txn_store->AddBlockStore(this, block_entry);
    }
    return total_copied;
}

// One writer
void SegmentEntry::DeleteData(TransactionID txn_id,
                              TxnTimeStamp commit_ts,
                              const HashMap<BlockID, Vector<BlockOffset>> &block_row_hashmap,
                              Txn *txn) {
    TxnTableStore *txn_store = txn->GetTxnTableStore(table_entry_);

    for (const auto &[block_id, delete_rows] : block_row_hashmap) {
        BlockEntry *block_entry = nullptr;
        {
            std::shared_lock lck(this->rw_locker_);
            block_entry = block_entries_.at(block_id).get();
        }

        block_entry->DeleteData(txn_id, commit_ts, delete_rows);
        txn_store->AddBlockStore(this, block_entry);
        if (delete_rows.size() > block_entry->row_capacity()) {
            UnrecoverableError("Delete rows exceed block capacity");
        }
        this->DecreaseRemainRow(delete_rows.size());
    }
    {
        std::unique_lock w_lock(rw_locker_);
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
    }
}

void SegmentEntry::CommitSegment(TransactionID txn_id, TxnTimeStamp commit_ts) {
    std::unique_lock w_lock(rw_locker_);
    min_row_ts_ = std::min(min_row_ts_, commit_ts);
    if (commit_ts < max_row_ts_) {
        UnrecoverableError(fmt::format("SegmentEntry commit_ts {} is less than max_row_ts {}", commit_ts, max_row_ts_));
    }
    max_row_ts_ = commit_ts;
    if (!this->Committed()) {
        this->txn_id_ = txn_id;
        this->Commit(commit_ts);
    }
}

void SegmentEntry::RollbackBlocks(TxnTimeStamp commit_ts, const Vector<BlockEntry *> &block_entry) {
    {
        std::unique_lock w_lock(rw_locker_);
        for (auto iter = block_entry.rbegin(); iter != block_entry.rend(); ++iter) {
            BlockEntry *block = *iter;
            if (!block->Committed()) {
                if (block_entries_.empty() || block_entries_.back()->block_id() != block->block_id()) {
                    UnrecoverableError("BlockEntry rollback order is not correct");
                }
                auto &rollback_block = block_entries_.back();
                rollback_block->Cleanup();
                block_entries_.pop_back();
            }
        }
    }
}

SharedPtr<BlockEntry> SegmentEntry::GetBlockEntryByID(BlockID block_id) const {
    std::shared_lock lock(rw_locker_);
    if (block_id >= block_entries_.size()) {
        return nullptr;
    }
    return block_entries_[block_id];
}

nlohmann::json SegmentEntry::Serialize(TxnTimeStamp max_commit_ts) {
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
        json_res["status"] = static_cast<std::underlying_type_t<SegmentStatus>>(this->status_);
        if (status_ != SegmentStatus::kUnsealed) {
            LOG_TRACE(fmt::format("SegmentEntry::Serialize: Begin try to save FastRoughFilter to json file"));
            this->GetFastRoughFilter()->SaveToJsonFile(json_res);
            LOG_TRACE(fmt::format("SegmentEntry::Serialize: End try to save FastRoughFilter to json file"));
        }
        for (auto &block_entry : this->block_entries_) {
            if (block_entry->commit_ts_ <= max_commit_ts) {
                block_entry->Flush(max_commit_ts);
                json_res["block_entries"].emplace_back(block_entry->Serialize(max_commit_ts));
            }
        }
    }
    return json_res;
}

SharedPtr<SegmentEntry> SegmentEntry::Deserialize(const nlohmann::json &segment_entry_json, TableEntry *table_entry, BufferManager *buffer_mgr) {
    std::underlying_type_t<SegmentStatus> saved_status = segment_entry_json["status"];
    SegmentStatus segment_status = static_cast<SegmentStatus>(saved_status);
    SharedPtr<SegmentEntry> segment_entry = MakeShared<SegmentEntry>(table_entry,
                                                                     MakeShared<String>(segment_entry_json["segment_dir"]),
                                                                     segment_entry_json["segment_id"],
                                                                     segment_entry_json["row_capacity"],
                                                                     segment_entry_json["column_count"],
                                                                     segment_status);
    segment_entry->min_row_ts_ = segment_entry_json["min_row_ts"];
    segment_entry->max_row_ts_ = segment_entry_json["max_row_ts"];
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

    if (segment_entry->status_ != SegmentStatus::kUnsealed) {
        if (segment_entry->GetFastRoughFilter()->LoadFromJsonFile(segment_entry_json)) {
            LOG_TRACE("SegmentEntry::Deserialize: Finish load FastRoughFilter from json file");
        } else {
            LOG_TRACE("SegmentEntry::Deserialize: Cannot load FastRoughFilter from json file");
        }
    }

    LOG_TRACE(fmt::format("Segment: {}, Block count: {}", segment_entry->segment_id_, segment_entry->block_entries_.size()));

    return segment_entry;
}

void SegmentEntry::FlushNewData(TxnTimeStamp flush_ts) {
    if (this->min_row_ts_ == UNCOMMIT_TS) {
        this->min_row_ts_ = flush_ts;
    }
    for (const auto &block_entry : this->block_entries_) {
        block_entry->FlushForImport(flush_ts);
    }
}

SharedPtr<String> SegmentEntry::DetermineSegmentDir(const String &parent_dir, u32 seg_id) {
    return MakeShared<String>(fmt::format("{}/seg_{}", parent_dir, std::to_string(seg_id)));
}

void SegmentEntry::Cleanup() {
    for (auto &block_entry : block_entries_) {
        block_entry->Cleanup();
    }
    CleanupScanner::CleanupDir(*segment_dir_);
}

void SegmentEntry::PickCleanup(CleanupScanner *scanner) {}

// used in:
// 1. record minmax filter and optional bloom filter created for sealed segment created by append, import and compact
// 2. record optional bloom filter created by requirement when the properties of the table is changed ? (maybe will support it in the future)
void SegmentEntry::LoadFilterBinaryData(const String &segment_filter_data) {
    std::unique_lock lock(rw_locker_);
    if (status_ == SegmentStatus::kUnsealed) {
        UnrecoverableError("should not call LoadFilterBinaryData from Unsealed segment");
    }
    fast_rough_filter_.DeserializeFromString(segment_filter_data);
}
String SegmentEntry::SegmentStatusToString(const SegmentStatus &type) {
    switch (type) {
        case SegmentStatus::kUnsealed:
            return "Unsealed";
        case SegmentStatus::kSealed:
            return "Sealed";
        case SegmentStatus::kCompacting:
            return "Compacting";
        case SegmentStatus::kNoDelete:
            return "NoDelete";
        case SegmentStatus::kDeprecated:
            return "Deprecated";
        default:
            return "Invalid Status";
    }
}

} // namespace infinity
