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
import compact_state_data;
import cleanup_scanner;
import background_process;
import wal_entry;
import infinity_context;
import roaring_bitmap;

namespace infinity {

Vector<std::string_view> SegmentEntry::DecodeIndex(std::string_view encode) {
    SizeT delimiter_i = encode.rfind('#');
    if (delimiter_i == String::npos) {
        String error_message = fmt::format("Invalid segment entry encode: {}", encode);
        UnrecoverableError(error_message);
    }
    auto decodes = TableEntry::DecodeIndex(encode.substr(0, delimiter_i));
    decodes.push_back(encode.substr(delimiter_i + 1));
    return decodes;
}

String SegmentEntry::EncodeIndex(const SegmentID segment_id, const TableEntry *table_entry) {
    if (table_entry == nullptr) {
        return ""; // unit test
    }
    return fmt::format("{}#{}", table_entry->encode(), segment_id);
}

SegmentEntry::SegmentEntry(TableEntry *table_entry,
                           SharedPtr<String> segment_dir,
                           SegmentID segment_id,
                           SizeT row_capacity,
                           SizeT column_count,
                           SegmentStatus status)
    : BaseEntry(EntryType::kSegment, false, SegmentEntry::EncodeIndex(segment_id, table_entry)), table_entry_(table_entry), segment_dir_(segment_dir),
      segment_id_(segment_id), row_capacity_(row_capacity), column_count_(column_count), status_(status) {}

SharedPtr<SegmentEntry> SegmentEntry::NewSegmentEntry(TableEntry *table_entry, SegmentID segment_id, Txn *txn) {
    SharedPtr<SegmentEntry> segment_entry = MakeShared<SegmentEntry>(table_entry,
                                                                     SegmentEntry::DetermineSegmentDir(*table_entry->TableEntryDir(), segment_id),
                                                                     segment_id,
                                                                     DEFAULT_SEGMENT_CAPACITY,
                                                                     table_entry->ColumnCount(),
                                                                     SegmentStatus::kUnsealed);
    segment_entry->begin_ts_ = txn->BeginTS();
    segment_entry->txn_id_ = txn->TxnID();
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
                                                            TxnTimeStamp first_delete_ts,
                                                            TxnTimeStamp deprecate_ts,
                                                            TxnTimeStamp begin_ts,
                                                            TransactionID txn_id) {
    auto segment_dir = SegmentEntry::DetermineSegmentDir(*table_entry->TableEntryDir(), segment_id);
    auto segment_entry = MakeShared<SegmentEntry>(table_entry, std::move(segment_dir), segment_id, row_capacity, column_count, status);
    segment_entry->min_row_ts_ = min_row_ts;
    segment_entry->max_row_ts_ = max_row_ts;
    segment_entry->commit_ts_ = commit_ts;
    segment_entry->first_delete_ts_ = first_delete_ts;
    segment_entry->deprecate_ts_ = deprecate_ts;
    segment_entry->begin_ts_ = begin_ts;
    segment_entry->row_count_ = row_count;
    segment_entry->actual_row_count_ = actual_row_count;
    segment_entry->txn_id_ = txn_id;
    return segment_entry;
}

void SegmentEntry::UpdateSegmentReplay(SharedPtr<SegmentEntry> segment_entry, String segment_filter_binary_data) {
    status_ = segment_entry->status_;
    row_count_ = segment_entry->row_count_;
    actual_row_count_ = segment_entry->actual_row_count_;
    min_row_ts_ = segment_entry->min_row_ts_;
    max_row_ts_ = segment_entry->max_row_ts_;
    deprecate_ts_ = segment_entry->deprecate_ts_;
    if (!segment_filter_binary_data.empty()) {
        LoadFilterBinaryData(segment_filter_binary_data);
    }
}

bool SegmentEntry::SetSealed() {
    std::unique_lock lock(rw_locker_);
    if (status_ != SegmentStatus::kUnsealed) {
        return false;
    }
    status_ = SegmentStatus::kSealed;
    return true;
}

void SegmentEntry::AddBlockReplay(SharedPtr<BlockEntry> block_entry) {
    BlockID block_id = block_entry->block_id();
    BlockID cur_blocks_size = block_entries_.size();
    if (block_id >= cur_blocks_size) {
        block_entries_.resize(block_id + 1);
    }
    if (block_entries_[block_id].get() != nullptr) {
        String error_message = fmt::format("BlockEntry {} already exists in SegmentEntry {}", block_id, segment_id_);
        UnrecoverableError(error_message);
    }
    block_entries_[block_id] = std::move(block_entry);
}

void SegmentEntry::UpdateBlockReplay(SharedPtr<BlockEntry> new_block, String block_filter_binary_data) {
    BlockID block_id = new_block->block_id();
    if (block_id >= block_entries_.size() || block_entries_[block_id].get() == nullptr) {
        String error_message = fmt::format("BlockEntry {} does not exist in SegmentEntry {}", block_id, segment_id_);
        UnrecoverableError(error_message);
    }
    block_entries_[block_id]->UpdateBlockReplay(new_block, std::move(block_filter_binary_data));
}

bool SegmentEntry::TrySetCompacting(CompactStateData *compact_state_data) {
    std::unique_lock lock(rw_locker_);
    if (status_ != SegmentStatus::kSealed) {
        return false;
    }
    compact_state_data_ = compact_state_data;
    status_ = SegmentStatus::kCompacting;
    return true;
}

bool SegmentEntry::SetNoDelete() {
    std::unique_lock lock(rw_locker_);
    if (status_ != SegmentStatus::kCompacting && status_ != SegmentStatus::kNoDelete) {
        String error_message = "Assert: kNoDelete is only allowed to set on compacting segment.";
        UnrecoverableError(error_message);
    }
    status_ = SegmentStatus::kNoDelete;
    if (!delete_txns_.empty()) {
        std::stringstream ss;
        for (auto txn_id : delete_txns_) {
            ss << txn_id << " ";
        }
        LOG_WARN(fmt::format("Segment {} cannot set no delete, because has delete txns: {}", segment_id_, ss.str()));
        return false;
    }
    compact_state_data_ = nullptr;
    return true;
}

void SegmentEntry::SetDeprecated(TxnTimeStamp deprecate_ts) {
    std::unique_lock lock(rw_locker_);
    if (status_ != SegmentStatus::kNoDelete) {
        String error_message = "Assert: kDeprecated is only allowed to set on compacting segment.";
        UnrecoverableError(error_message);
    }
    status_ = SegmentStatus::kDeprecated;
    deprecate_ts_ = deprecate_ts;
}

void SegmentEntry::RollbackCompact() {
    std::unique_lock lock(rw_locker_);
    if (status_ != SegmentStatus::kNoDelete) {
        String error_message = "Assert: Rollbacked segment should be in No Delete state.";
        UnrecoverableError(error_message);
    }
    status_ = SegmentStatus::kSealed;
    deprecate_ts_ = UNCOMMIT_TS;
}

bool SegmentEntry::CheckDeleteConflict(Vector<Pair<SegmentEntry *, Vector<SegmentOffset>>> &&segments, TransactionID txn_id) {
    // hold all locks and check
    Vector<std::unique_lock<std::shared_mutex>> locks;
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

bool SegmentEntry::CheckRowVisible(SegmentOffset segment_offset, TxnTimeStamp check_ts, bool check_append) const {
    // FIXME: get the block_capacity from config?
    u32 block_capacity = DEFAULT_BLOCK_CAPACITY;
    BlockID block_id = segment_offset / block_capacity;
    BlockOffset block_offset = segment_offset % block_capacity;

    auto *block_entry = GetBlockEntryByID(block_id).get();
    if (block_entry == nullptr || block_entry->commit_ts_ > check_ts) {
        return false;
    }
    return block_entry->CheckRowVisible(block_offset, check_ts, check_append);
}

void SegmentEntry::CheckRowsVisible(Vector<u32> &segment_offsets, TxnTimeStamp check_ts) const {
    std::shared_lock lock(rw_locker_);
    if (first_delete_ts_ >= check_ts)
        return;

    Vector<u32> segment_offsets2;
    segment_offsets2.reserve(segment_offsets.size());
    Map<BlockID, Vector<u32>> block_offsets_map;
    for (const auto segment_offset : segment_offsets) {
        BlockID block_id = segment_offset >> BLOCK_OFFSET_SHIFT;
        block_offsets_map[block_id].push_back(segment_offset);
    }

    for (auto &[block_id, block_offsets] : block_offsets_map) {
        auto *block_entry = GetBlockEntryByID(block_id).get();
        if (block_entry == nullptr || block_entry->commit_ts_ > check_ts) {
            continue;
        }
        block_entry->CheckRowsVisible(block_offsets, check_ts);
        std::copy(block_offsets.begin(), block_offsets.end(), std::back_inserter(segment_offsets2));
    }
    segment_offsets = std::move(segment_offsets2);
}

void SegmentEntry::CheckRowsVisible(Bitmask &segment_offsets, TxnTimeStamp check_ts) const {
    std::shared_lock lock(rw_locker_);
    if (first_delete_ts_ >= check_ts)
        return;
    for (auto &block_entry : block_entries_) {
        block_entry->CheckRowsVisible(segment_offsets, check_ts);
    }
}

bool SegmentEntry::CheckVisible(Txn *txn) const {
    TxnTimeStamp begin_ts = txn->BeginTS();
    std::shared_lock lock(rw_locker_);
    return begin_ts < deprecate_ts_ && BaseEntry::CheckVisible(txn);
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

SizeT SegmentEntry::row_count(TxnTimeStamp check_ts) const {
    std::shared_lock lock(rw_locker_);
    if (status_ == SegmentStatus::kDeprecated && check_ts > deprecate_ts_) {
        return 0;
    }
    if (status_ == SegmentStatus::kSealed) {
        return row_count_; // FIXME
    }
    SizeT row_count = 0;
    for (const auto &block_entry : block_entries_) {
        row_count += block_entry->row_count(check_ts);
    }
    return row_count;
}

// One writer
u64 SegmentEntry::AppendData(TransactionID txn_id, TxnTimeStamp commit_ts, AppendState *append_state_ptr, BufferManager *buffer_mgr, Txn *txn) {
    // create table store in `Txn::Append`
    TxnTableStore *txn_store = txn->GetExistTxnTableStore(table_entry_);

    std::unique_lock lck(this->rw_locker_); // FIXME: lock scope too large
    if (this->status_ != SegmentStatus::kUnsealed) {
        String error_message = "AppendData to sealed/compacting/no_delete/deprecated segment";
        UnrecoverableError(error_message);
    }

    if (this->row_capacity_ <= this->row_count_) {
        String error_message = fmt::format("Segment {} error, row_count {}, capacity {}", segment_id_, row_count_, row_capacity_);
        UnrecoverableError(error_message);
    }

    //    SizeT start_row = this->row_count_;
    SizeT append_block_count = append_state_ptr->blocks_.size();
    u64 total_copied{0};
    Vector<BlockEntry *> mut_blocks;

    while (append_state_ptr->current_block_ < append_block_count && this->row_count_ < this->row_capacity_) {
        DataBlock *input_block = append_state_ptr->blocks_[append_state_ptr->current_block_].get();

        u16 to_append_rows = input_block->row_count() - append_state_ptr->current_block_offset_;
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
                String error_message = fmt::format("Attempt to append rows: {}, but rows: {} are appended", to_append_rows, actual_appended);
                UnrecoverableError(error_message);
            }

            append_state_ptr->append_ranges_.emplace_back(range_segment_id, range_block_id, range_block_start_row, actual_appended);

            total_copied += actual_appended;
            to_append_rows -= actual_appended;
            append_state_ptr->current_count_ += actual_appended;
            append_state_ptr->current_block_offset_ += actual_appended;
            IncreaseRowCount(actual_appended);

            if (this->row_count_ == this->row_capacity_) {
                LOG_INFO(fmt::format("Segment {} is full.", segment_id_));
                break;
            }

            if (this->row_count_ > this->row_capacity_) {
                String error_message = "Not implemented: append data exceed segment row capacity";
                UnrecoverableError(error_message);
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
SizeT SegmentEntry::DeleteData(TransactionID txn_id,
                               TxnTimeStamp commit_ts,
                               const HashMap<BlockID, Vector<BlockOffset>> &block_row_hashmap,
                               Txn *txn) {
    // create table store in `Txn::Delete`
    TxnTableStore *txn_store = txn->GetExistTxnTableStore(table_entry_);
    SizeT delete_row_n = 0;

    for (const auto &[block_id, delete_rows] : block_row_hashmap) {
        BlockEntry *block_entry = nullptr;
        {
            std::shared_lock lck(this->rw_locker_);
            block_entry = block_entries_.at(block_id).get();
        }

        delete_row_n += block_entry->DeleteData(txn_id, commit_ts, delete_rows);
        txn_store->AddBlockStore(this, block_entry);
        if (delete_rows.size() > block_entry->row_capacity()) {
            String error_message = "Delete rows exceed block capacity";
            UnrecoverableError(error_message);
        }
    }
    this->DecreaseRemainRow(delete_row_n);
    return delete_row_n;
}

void SegmentEntry::CommitFlushed(TxnTimeStamp commit_ts, WalSegmentInfo *segment_info) {
    std::shared_lock w_lock(rw_locker_);
    for (auto &block_entry : block_entries_) {
        WalBlockInfo *block_info_ptr = nullptr;
        for (auto &block_info : segment_info->block_infos_) {
            if (block_info.block_id_ == block_entry->block_id()) {
                block_info_ptr = &block_info;
                break;
            }
        }
        block_entry->CommitFlushed(commit_ts, block_info_ptr);
    }
}

void SegmentEntry::CommitSegment(TransactionID txn_id,
                                 TxnTimeStamp commit_ts,
                                 const TxnSegmentStore &segment_store,
                                 const DeleteState *delete_state) {
    std::unique_lock w_lock(rw_locker_);
    if (status_ == SegmentStatus::kDeprecated) {
        return;
    }

    if (delete_state != nullptr) {
        auto iter = delete_state->rows_.find(segment_id_);
        if (iter != delete_state->rows_.end()) {
            const auto &block_row_hashmap = iter->second;
            if (!block_row_hashmap.empty() && this->first_delete_ts_ == UNCOMMIT_TS) {
                this->first_delete_ts_ = commit_ts;
            }
            delete_txns_.erase(txn_id);

            if (compact_state_data_ != nullptr) {
                if (status_ != SegmentStatus::kCompacting && status_ != SegmentStatus::kNoDelete) {
                    String error_message = "Assert: compact_task is not nullptr means segment is being compacted";
                    UnrecoverableError(error_message);
                }
                Vector<SegmentOffset> segment_offsets;
                for (const auto &[block_id, block_offsets] : block_row_hashmap) {
                    for (auto block_offset : block_offsets) {
                        segment_offsets.push_back(block_id * DEFAULT_BLOCK_CAPACITY + block_offset);
                    }
                }
                LOG_INFO(fmt::format("Append {} rows to to_delete_list in compact list", segment_offsets.size()));
                compact_state_data_->AddToDelete(commit_ts, segment_id_, std::move(segment_offsets));
            }
        }
    }

    min_row_ts_ = std::min(min_row_ts_, commit_ts);
    if (commit_ts < max_row_ts_) {
        String error_message = fmt::format("SegmentEntry commit_ts {} is less than max_row_ts {}", commit_ts, max_row_ts_);
        UnrecoverableError(error_message);
    }
    max_row_ts_ = commit_ts;
    if (!this->Committed()) {
        this->Commit(commit_ts);
    }
    // hold the lock here
    for (const auto &[block_id, block_entry] : segment_store.block_entries_) {
        block_entry->CommitBlock(txn_id, commit_ts);
    }
}

void SegmentEntry::RollbackBlocks(TxnTimeStamp commit_ts, const HashMap<BlockID, BlockEntry *> &block_entries) {
    std::unique_lock w_lock(rw_locker_);
    Vector<Pair<BlockID, BlockEntry *>> rollback_blocks(block_entries.begin(), block_entries.end());
    std::sort(rollback_blocks.begin(), rollback_blocks.end(), [](const auto &a, const auto &b) { return a.first > b.first; });
    for (auto [block_id, block_entry] : rollback_blocks) {
        if (!block_entry->Committed()) {
            if (block_entries_.empty() || block_entries_.back()->block_id() != block_entry->block_id()) {
                String error_message = "BlockEntry rollback order is not correct";
                UnrecoverableError(error_message);
            }
            auto &rollback_block = block_entries_.back();
            rollback_block->Cleanup();
            block_entries_.pop_back();
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

    this->checkpoint_row_count_ = 0;

    // const field
    json_res["segment_dir"] = *this->segment_dir_;
    json_res["row_capacity"] = this->row_capacity_;
    json_res["segment_id"] = this->segment_id_;
    json_res["column_count"] = this->column_count_;
    {
        std::shared_lock<std::shared_mutex> lck(this->rw_locker_);

        json_res["min_row_ts"] = this->min_row_ts_;
        json_res["max_row_ts"] = std::min(this->max_row_ts_, max_commit_ts);
        json_res["first_delete_ts"] = this->first_delete_ts_;
        json_res["deleted"] = this->deleted_;
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
                this->checkpoint_row_count_ += block_entry->checkpoint_row_count();
            }
        }

        json_res["row_count"] = this->checkpoint_row_count_;
    }
    return json_res;
}

SharedPtr<SegmentEntry> SegmentEntry::Deserialize(const nlohmann::json &segment_entry_json, TableEntry *table_entry, BufferManager *buffer_mgr) {
    std::underlying_type_t<SegmentStatus> saved_status = segment_entry_json["status"];
    SegmentStatus segment_status = static_cast<SegmentStatus>(saved_status);
    SharedPtr<String> segment_dir = MakeShared<String>(segment_entry_json["segment_dir"]);
    SharedPtr<SegmentEntry> segment_entry = MakeShared<SegmentEntry>(table_entry,
                                                                     segment_dir,
                                                                     segment_entry_json["segment_id"],
                                                                     segment_entry_json["row_capacity"],
                                                                     segment_entry_json["column_count"],
                                                                     segment_status);
    segment_entry->min_row_ts_ = segment_entry_json["min_row_ts"];
    segment_entry->max_row_ts_ = segment_entry_json["max_row_ts"];
    if (const auto fd_iter = segment_entry_json.find("first_delete_ts"); fd_iter != segment_entry_json.end()) {
        segment_entry->first_delete_ts_ = *fd_iter;
    }
    segment_entry->row_count_ = segment_entry_json["row_count"];
    segment_entry->actual_row_count_ = segment_entry_json["actual_row_count"];
    segment_entry->checkpoint_row_count_ = 0;

    segment_entry->commit_ts_ = segment_entry_json["commit_ts"];
    segment_entry->begin_ts_ = segment_entry_json["begin_ts"];
    segment_entry->txn_id_ = segment_entry_json["txn_id"];

    if (segment_entry_json.contains("block_entries")) {
        for (const auto &block_json : segment_entry_json["block_entries"]) {
            UniquePtr<BlockEntry> block_entry = BlockEntry::Deserialize(block_json, segment_entry.get(), buffer_mgr);
            auto block_entries_size = segment_entry->block_entries_.size();
            segment_entry->block_entries_.resize(std::max(static_cast<SizeT>(block_entries_size), static_cast<SizeT>(block_entry->block_id() + 1)));
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

void SegmentEntry::FlushNewData() {
    for (const auto &block_entry : this->block_entries_) {
        block_entry->FlushForImport();
    }
}

SharedPtr<String> SegmentEntry::DetermineSegmentDir(const String &parent_dir, u32 seg_id) {
    return MakeShared<String>(fmt::format("{}/seg_{}", parent_dir, std::to_string(seg_id)));
}

void SegmentEntry::Cleanup() {
    for (auto &block_entry : block_entries_) {
        block_entry->Cleanup();
    }

    String full_segment_dir = Path(InfinityContext::instance().config()->DataDir()) / *segment_dir_;
    LOG_DEBUG(fmt::format("Cleaning up segment dir: {}", full_segment_dir));
    CleanupScanner::CleanupDir(full_segment_dir);
    LOG_DEBUG(fmt::format("Cleaned segment dir: {}", full_segment_dir));
}

void SegmentEntry::PickCleanup(CleanupScanner *scanner) {}

// used in:
// 1. record minmax filter and optional bloom filter created for sealed segment created by append, import and compact
// 2. record optional bloom filter created by requirement when the properties of the table is changed ? (maybe will support it in the future)
void SegmentEntry::LoadFilterBinaryData(const String &segment_filter_data) {
    std::unique_lock lock(rw_locker_);
    if (status_ == SegmentStatus::kUnsealed) {
        String error_message = "Should not call LoadFilterBinaryData from Unsealed segment";
        UnrecoverableError(error_message);
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

String SegmentEntry::ToString() const {
    return fmt::format("Segment path: {}, id: {}, row_count: {}, block_count: {}, status: {}", *segment_dir_, segment_id_, row_count_, block_entries_.size(), SegmentStatusToString(status_));
}

} // namespace infinity
