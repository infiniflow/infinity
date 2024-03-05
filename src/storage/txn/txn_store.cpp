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

#include <vector>

module txn_store;

import stl;
import third_party;

import status;
import infinity_exception;
import data_block;
import logger;
import data_access_state;
import txn;
import default_values;
import table_entry;
import catalog;
import internal_types;
import data_type;
import background_process;
import bg_task;
import compact_segments_task;

namespace infinity {

TxnIndexStore::TxnIndexStore(TableIndexEntry *table_index_entry) : table_index_entry_(table_index_entry) {}

Tuple<UniquePtr<String>, Status> TxnTableStore::Append(const SharedPtr<DataBlock> &input_block) {
    SizeT column_count = table_entry_->ColumnCount();
    if (input_block->column_count() != column_count) {
        String err_msg = fmt::format("Attempt to insert different column count data block into transaction table store");
        LOG_ERROR(err_msg);
        return {MakeUnique<String>(err_msg), Status::ColumnCountMismatch(err_msg)};
    }

    Vector<SharedPtr<DataType>> column_types;
    for (SizeT col_id = 0; col_id < column_count; ++col_id) {
        column_types.emplace_back(table_entry_->GetColumnDefByID(col_id)->type());
        if (*column_types.back() != *input_block->column_vectors[col_id]->data_type()) {
            String err_msg = fmt::format("Attempt to insert different type data into transaction table store");
            LOG_ERROR(err_msg);
            return {MakeUnique<String>(err_msg),
                    Status::DataTypeMismatch(column_types.back()->ToString(), input_block->column_vectors[col_id]->data_type()->ToString())};
        }
    }

    DataBlock *current_block{nullptr};
    if (blocks_.empty()) {
        blocks_.emplace_back(DataBlock::Make());
        current_block_id_ = 0;
        blocks_.back()->Init(column_types);
    }
    current_block = blocks_[current_block_id_].get();

    if (current_block->row_count() + input_block->row_count() > current_block->capacity()) {
        SizeT to_append = current_block->capacity() - current_block->row_count();
        current_block->AppendWith(input_block.get(), 0, to_append);
        current_block->Finalize();

        blocks_.emplace_back(DataBlock::Make());
        blocks_.back()->Init(column_types);
        ++current_block_id_;
        current_block = blocks_[current_block_id_].get();
        current_block->AppendWith(input_block.get(), to_append, input_block->row_count() - to_append);
    } else {
        SizeT to_append = input_block->row_count();
        current_block->AppendWith(input_block.get(), 0, to_append);
    }
    current_block->Finalize();

    return {nullptr, Status::OK()};
}

Tuple<UniquePtr<String>, Status> TxnTableStore::Import(const SharedPtr<SegmentEntry> &segment) {
    uncommitted_segments_.emplace_back(segment);
    return {nullptr, Status::OK()};
}

Tuple<UniquePtr<String>, Status>
TxnTableStore::CreateIndexFile(TableIndexEntry *table_index_entry, u64 column_id, u32 segment_id, SharedPtr<SegmentColumnIndexEntry> index) {
    const String &index_name = *table_index_entry->index_base()->index_name_;
    if (auto column_index_iter = txn_indexes_store_.find(index_name); column_index_iter != txn_indexes_store_.end()) {
        column_index_iter->second.index_entry_map_[column_id][segment_id] = index;
    } else {
        TxnIndexStore index_store(table_index_entry);
        index_store.index_entry_map_[column_id][segment_id] = index;
        txn_indexes_store_.emplace(index_name, std::move(index_store));
    }
    return {nullptr, Status::OK()};
}

Tuple<UniquePtr<String>, Status> TxnTableStore::Delete(const Vector<RowID> &row_ids) {
    HashMap<SegmentID, HashMap<BlockID, Vector<BlockOffset>>> &row_hash_table = delete_state_.rows_;
    for (auto row_id : row_ids) {
        BlockID block_id = row_id.segment_offset_ / DEFAULT_BLOCK_CAPACITY;
        BlockOffset block_offset = row_id.segment_offset_ % DEFAULT_BLOCK_CAPACITY;
        auto &seg_map = row_hash_table[row_id.segment_id_];
        auto &block_vec = seg_map[block_id];
        block_vec.emplace_back(block_offset);
        if (block_vec.size() > DEFAULT_BLOCK_CAPACITY) {
            UnrecoverableError("Delete row exceed block capacity");
        }
    }

    return {nullptr, Status::OK()};
}

Tuple<UniquePtr<String>, Status> TxnTableStore::Compact(Vector<Pair<SharedPtr<SegmentEntry>, Vector<SegmentEntry *>>> &&segment_data,
                                                        CompactSegmentsTaskType type) {
    if (!compact_state_.segment_data_.empty()) {
        UnrecoverableError("Attempt to compact table store twice");
    }
    compact_state_ = TxnCompactStore{std::move(segment_data), type};
    return {nullptr, Status::OK()};
}

void TxnTableStore::Scan(SharedPtr<DataBlock> &) {}

void TxnTableStore::Rollback() {
    if (append_state_.get() != nullptr) {
        // Rollback the data already been appended.
        Catalog::RollbackAppend(table_entry_, txn_->TxnID(), txn_->CommitTS(), this);
        LOG_TRACE(fmt::format("Rollback prepare appended data in table: {}", *table_entry_->GetTableName()));
    }

    Catalog::RollbackCompact(table_entry_, txn_->TxnID(), txn_->CommitTS(), compact_state_);
    blocks_.clear();
}

void TxnTableStore::PrepareCommit() {
    // Init append state
    append_state_ = MakeUnique<AppendState>(this->blocks_);

    // Start to append
    LOG_TRACE(fmt::format("Transaction local storage table: {}, Start to prepare commit", *table_entry_->GetTableName()));
    Txn *txn_ptr = (Txn *)txn_;
    Catalog::Append(table_entry_, txn_->TxnID(), this, txn_ptr->GetBufferMgr());

    SizeT segment_count = uncommitted_segments_.size();
    for (SizeT seg_idx = 0; seg_idx < segment_count; ++seg_idx) {
        const SharedPtr<SegmentEntry> &uncommitted = uncommitted_segments_[seg_idx];
        // Segments in `uncommitted_segments_` are already persisted. Import them to memory catalog.
        Catalog::CommitImport(table_entry_, txn_->CommitTS(), uncommitted);
    }
    // Attention: "compact" needs to be ahead of "delete"
    Catalog::CommitCompact(table_entry_, txn_->TxnID(), txn_->CommitTS(), compact_state_);

    Catalog::Delete(table_entry_, txn_->TxnID(), txn_->CommitTS(), delete_state_);
    Catalog::CommitCreateIndex(txn_indexes_store_);

    LOG_TRACE(fmt::format("Transaction local storage table: {}, Complete commit preparing", *table_entry_->GetTableName()));
}

/**
 * @brief Call for really commit the data to disk.
 */
void TxnTableStore::Commit() const {
    Catalog::CommitAppend(table_entry_, txn_->TxnID(), txn_->CommitTS(), append_state_.get());
    Catalog::CommitDelete(table_entry_, txn_->TxnID(), txn_->CommitTS(), delete_state_);
}

void TxnTableStore::TryTriggerCompaction(BGTaskProcessor *bg_task_processor, TxnManager *txn_mgr) {
    std::function<Txn *()> generate_txn = [txn_mgr]() { return txn_mgr->CreateTxn(); };

    // FIXME OPT: trigger compaction one time for all segments
    for (const SharedPtr<SegmentEntry> &new_segment : uncommitted_segments_) {
        auto ret = table_entry_->TryCompactAddSegment(new_segment.get(), generate_txn);
        if (!ret.has_value()) {
            continue;
        }
        auto [to_compacts, txn] = *ret;
        auto compact_task = CompactSegmentsTask::MakeTaskWithPickedSegments(table_entry_, std::move(to_compacts), txn);
        bg_task_processor->Submit(compact_task);
    }
    for (const auto &[segment_id, delete_map] : delete_state_.rows_) {
        auto ret = table_entry_->TryCompactDeleteRow(segment_id, generate_txn);
        if (!ret.has_value()) {
            continue;
        }
        auto [to_compacts, txn] = *ret;
        if (to_compacts.empty()) {
            continue; // delete down layer but not trigger compaction
        }
        auto compact_task = CompactSegmentsTask::MakeTaskWithPickedSegments(table_entry_, std::move(to_compacts), txn);
        bg_task_processor->Submit(compact_task);
    }
}

} // namespace infinity
