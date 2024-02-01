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
#include <string>
#include <vector>

module catalog;

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
import parser;
import txn_store;

import catalog_delta_entry;
import status;
import compact_segments_task;

namespace infinity {

SegmentEntry::SegmentEntry(const TableEntry *table_entry, SharedPtr<String> segment_dir, SegmentID segment_id, SizeT row_capacity, SizeT column_count)
    : BaseEntry(EntryType::kSegment), table_entry_(table_entry), segment_dir_(segment_dir), segment_id_(segment_id), row_capacity_(row_capacity),
      column_count_(column_count) {}

SharedPtr<SegmentEntry> SegmentEntry::NewSegmentEntry(const TableEntry *table_entry, SegmentID segment_id, Txn *txn) {
    SharedPtr<SegmentEntry> segment_entry = MakeShared<SegmentEntry>(table_entry,
                                                                     SegmentEntry::DetermineSegmentDir(*table_entry->TableEntryDir(), segment_id),
                                                                     segment_id,
                                                                     DEFAULT_SEGMENT_CAPACITY,
                                                                     table_entry->ColumnCount());

    auto operation = MakeUnique<AddSegmentEntryOp>(segment_entry.get());
    txn->AddCatalogDeltaOperation(std::move(operation));
    segment_entry->begin_ts_ = txn->BeginTS();

    return segment_entry;
}

SharedPtr<SegmentEntry> SegmentEntry::NewReplaySegmentEntry(const TableEntry *table_entry,
                                                            SegmentID segment_id,
                                                            const SharedPtr<String> &segment_dir,
                                                            TxnTimeStamp commit_ts) {
    auto segment_entry = MakeShared<SegmentEntry>(table_entry, segment_dir, segment_id, DEFAULT_SEGMENT_CAPACITY, table_entry->ColumnCount());
    segment_entry->min_row_ts_ = commit_ts;
    return segment_entry;
}

SharedPtr<SegmentEntry> SegmentEntry::NewReplayCatalogSegmentEntry(const TableEntry *table_entry,
                                                                   SegmentID segment_id,
                                                                   const SharedPtr<String> &segment_dir,
                                                                   u64 column_count,
                                                                   SizeT row_count,
                                                                   SizeT row_capacity,
                                                                   TxnTimeStamp min_row_ts,
                                                                   TxnTimeStamp deprecate_ts,
                                                                   TxnTimeStamp commit_ts,
                                                                   TxnTimeStamp begin_ts,
                                                                   TransactionID txn_id) {

    auto segment_entry = MakeShared<SegmentEntry>(table_entry, segment_dir, segment_id, row_capacity, column_count);
    segment_entry->min_row_ts_ = min_row_ts;
    segment_entry->deprecate_ts_ = deprecate_ts;
    segment_entry->commit_ts_ = commit_ts;
    segment_entry->begin_ts_ = begin_ts;
    segment_entry->row_count_ = row_count;
    segment_entry->txn_id_ = txn_id;
    return segment_entry;
}

int SegmentEntry::Room() {
    std::shared_lock<std::shared_mutex> lck(rw_locker_);
    return this->row_capacity_ - this->row_count_;
}

void SegmentEntry::FlushData() {
    for (const auto &block_entry : this->block_entries_) {
        block_entry->FlushData(block_entry->row_count());
    }
}

bool SegmentEntry::TrySetCompacting(CompactSegmentsTask *compact_task, TxnTimeStamp compacting_ts) {
    std::unique_lock lock(rw_locker_);
    if (min_row_ts_ > compacting_ts || deprecate_ts_ < compacting_ts) {
        return false;
    }
    compact_task_ = compact_task;
    compacting_ts_ = compacting_ts;
    return true;
}

void SegmentEntry::SetNoDelete(TxnTimeStamp no_delete_ts) {
    std::unique_lock lock(rw_locker_);
    no_delete_ts_ = no_delete_ts;
    compact_task_ = nullptr;
}

void SegmentEntry::SetDeprecated(TxnTimeStamp deprecate_ts) {
    std::unique_lock lock(rw_locker_);
    deprecate_ts_ = deprecate_ts;

    // this is not necessary, but for safety
    compact_task_ = nullptr;
}

void SegmentEntry::RollbackCompact() {
    std::unique_lock lock(rw_locker_);
    compacting_ts_ = UNCOMMIT_TS;
    no_delete_ts_ = UNCOMMIT_TS;
    deprecate_ts_ = UNCOMMIT_TS;

    // this is not necessary, but for safety
    compact_task_ = nullptr;
}

bool SegmentEntry::CheckDeleteConflict(Vector<Pair<SegmentEntry *, Vector<SegmentOffset>>> &&segments, Txn *delete_txn) {
    Vector<std::shared_lock<std::shared_mutex>> locks;
    TxnTimeStamp delete_begin_ts = delete_txn->BeginTS();
    Vector<SizeT> to_delete_idxes;
    for (SizeT idx = 0; const auto &[segment_entry, delete_offsets] : segments) {
        locks.emplace_back(segment_entry->rw_locker_);
        if (delete_begin_ts > segment_entry->compacting_ts_) {
            if (delete_begin_ts < segment_entry->no_delete_ts_) {
                to_delete_idxes.emplace_back(idx);
            } else {
                return true;
            }
        }
        ++idx;
    }
    SharedPtr<Txn> txn = delete_txn->shared_from_this();
    for (SizeT idx : to_delete_idxes) {
        auto &[segment_entry, segment_offsets] = segments[idx];
        segment_entry->compact_task_->AddToDelete(segment_entry->segment_id(), std::move(segment_offsets), txn);
    }
    return false;
}

u64 SegmentEntry::AppendData(TransactionID txn_id, AppendState *append_state_ptr, BufferManager *buffer_mgr, Txn *txn) {
    std::unique_lock<std::shared_mutex> lck(this->rw_locker_);
    if (this->row_capacity_ - this->row_count_ <= 0)
        return 0;
    //    SizeT start_row = this->row_count_;
    SizeT append_block_count = append_state_ptr->blocks_.size();
    u64 total_copied{0};

    while (append_state_ptr->current_block_ < append_block_count && this->row_count_ < this->row_capacity_) {
        DataBlock *input_block = append_state_ptr->blocks_[append_state_ptr->current_block_].get();

        u16 to_append_rows = input_block->row_count();
        while (to_append_rows > 0) {
            // Append to_append_rows into block
            if (block_entries_.empty() || block_entries_.back()->GetAvailableCapacity() <= 0){
                this->block_entries_.emplace_back(BlockEntry::NewBlockEntry(this, 0, 0, this->column_count_, txn));
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

void SegmentEntry::DeleteData(TransactionID txn_id, TxnTimeStamp commit_ts, const HashMap<BlockID, Vector<BlockOffset>> &block_row_hashmap) {
    std::unique_lock<std::shared_mutex> lck(this->rw_locker_);

    for (const auto &[block_id, delete_rows] : block_row_hashmap) {
        BlockEntry *block_entry = this->GetBlockEntryByID(block_id);
        if (block_entry == nullptr) {
            UnrecoverableError(fmt::format("The segment doesn't contain the given block: {}.", block_id));
        }

        block_entry->DeleteData(txn_id, commit_ts, delete_rows);
    }
}

void SegmentEntry::CommitAppend(TransactionID txn_id, TxnTimeStamp commit_ts, u16 block_id, u16, u16) {
    SharedPtr<BlockEntry> block_entry;
    {
        std::unique_lock<std::shared_mutex> lck(this->rw_locker_);
        if (this->min_row_ts_ == UNCOMMIT_TS) {
            this->min_row_ts_ = commit_ts;
        }
        this->deprecate_ts_ = std::max(this->deprecate_ts_, commit_ts);
        block_entry = this->block_entries_[block_id];
    }
    block_entry->CommitAppend(txn_id, commit_ts);
}

void SegmentEntry::CommitDelete(TransactionID txn_id, TxnTimeStamp commit_ts, const HashMap<u16, Vector<BlockOffset>> &block_row_hashmap) {
    std::unique_lock<std::shared_mutex> lck(this->rw_locker_);

    for (const auto &[block_id, delete_rows] : block_row_hashmap) {
        // TODO: block_id is u16, GetBlockEntryByID need to be modified accordingly.
        BlockEntry *block_entry = this->GetBlockEntryByID(block_id);
        if (delete_rows.size() > block_entry->row_capacity()) {
            UnrecoverableError("Delete rows exceed block capacity");
        }
        if (block_entry == nullptr) {
            UnrecoverableError(fmt::format("The segment doesn't contain the given block: {}.", block_id));
        }

        DecreaseRemainRow(delete_rows.size());
        block_entry->CommitDelete(txn_id, commit_ts);
        this->deprecate_ts_ = std::max(this->deprecate_ts_, commit_ts);
    }
}

BlockEntry *SegmentEntry::GetBlockEntryByID(BlockID block_id) const {
    if (block_id < block_entries_.size()) {
        return block_entries_[block_id].get();
    } else {
        return nullptr;
    }
}

nlohmann::json SegmentEntry::Serialize(TxnTimeStamp max_commit_ts, bool is_full_checkpoint) {
    nlohmann::json json_res;

    {
        std::shared_lock<std::shared_mutex> lck(this->rw_locker_);
        json_res["segment_dir"] = *this->segment_dir_;
        json_res["row_capacity"] = this->row_capacity_;
        json_res["segment_id"] = this->segment_id_;
        json_res["column_count"] = this->column_count_;
        json_res["min_row_ts"] = this->min_row_ts_;
        json_res["max_row_ts"] = this->deprecate_ts_;
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
                                                                     segment_entry_json["column_count"]);

    segment_entry->min_row_ts_ = segment_entry_json["min_row_ts"];
    segment_entry->deprecate_ts_ = segment_entry_json["max_row_ts"];
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
    Vector<BlockEntry *> block_entries;
    for (auto &block_entry : this->block_entries()) {
        if (is_full_checkpoint || max_commit_ts > block_entry->checkpoint_ts()) {
            block_entries.push_back(static_cast<BlockEntry *>(block_entry.get()));
        }
    }
    if (block_entries.empty()) {
        return;
    }
    for (BlockEntry *block_entry : block_entries) {
        LOG_TRACE(fmt::format("Before Flush: block_entry checkpoint ts: {}, min_row_ts: {}, max_row_ts: {} || max_commit_ts: {}",
                              block_entry->checkpoint_ts(),
                              block_entry->min_row_ts(),
                              block_entry->max_row_ts(),
                              max_commit_ts));
        block_entry->Flush(max_commit_ts);
        LOG_TRACE(fmt::format("Finish Flush: block_entry checkpoint ts: {}, min_row_ts: {}, max_row_ts: {} || max_commit_ts: {}",
                              block_entry->checkpoint_ts(),
                              block_entry->min_row_ts(),
                              block_entry->max_row_ts(),
                              max_commit_ts));
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
    // TODO: actual_row_count
    this->deprecate_ts_ = std::max(this->deprecate_ts_, segment_entry2->deprecate_ts_);

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

} // namespace infinity
