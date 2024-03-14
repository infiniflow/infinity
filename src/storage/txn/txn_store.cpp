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
import catalog_delta_entry;
import internal_types;
import data_type;
import background_process;
import bg_task;
import compact_segments_task;

namespace infinity {

TxnSegmentStore TxnSegmentStore::AddSegmentStore(SegmentEntry *segment_entry) {
    TxnSegmentStore txn_segment_store(segment_entry);
    for (auto &block_entry : segment_entry->block_entries()) {
        txn_segment_store.block_entries_.emplace_back(block_entry.get());
    }
    return txn_segment_store;
}

TxnSegmentStore::TxnSegmentStore(SegmentEntry *segment_entry) : segment_entry_(segment_entry) {}

///-----------------------------------------------------------------------------

TxnIndexStore::TxnIndexStore(TableIndexEntry *table_index_entry) : table_index_entry_(table_index_entry) {}

void TxnIndexStore::AddCatalogDeltaOperation(CatalogDeltaEntry *local_catalog_delta_ops_entry) const {
    local_catalog_delta_ops_entry->AddOperation(MakeUnique<AddTableIndexEntryOp>(table_index_entry_));
}

///-----------------------------------------------------------------------------

TxnCompactStore::TxnCompactStore() : task_type_(CompactSegmentsTaskType::kInvalid) {}

TxnCompactStore::TxnCompactStore(CompactSegmentsTaskType type) : task_type_(type) {}

///-----------------------------------------------------------------------------

Tuple<UniquePtr<String>, Status> TxnTableStore::Import(SharedPtr<SegmentEntry> segment_entry) {
    this->AddSegmentStore(segment_entry.get());
    this->AddSealedSegment(segment_entry.get());
    table_entry_->Import(std::move(segment_entry));
    return {nullptr, Status::OK()};
}

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

void TxnTableStore::AddIndexStore(TableIndexEntry *table_index_entry) { txn_indexes_.insert(table_index_entry); }

void TxnTableStore::AddSegmentIndexesStore(TableIndexEntry *table_index_entry, const Vector<SegmentIndexEntry *> &segment_index_entries) {
    auto *txn_index_store = this->GetIndexStore(table_index_entry);
    for (auto *segment_index_entry : segment_index_entries) {
        auto [iter, insert_ok] = txn_index_store->index_entry_map_.emplace(segment_index_entry->segment_id(), segment_index_entry);
        if (!insert_ok) {
            UnrecoverableError(fmt::format("Attempt to add segment index of segment {} store twice", segment_index_entry->segment_id()));
        }
    }
}

void TxnTableStore::AddFulltextIndexStore(TableIndexEntry *table_index_entry, FulltextIndexEntry *fulltext_index_entry) {
    auto *txn_index_store = this->GetIndexStore(table_index_entry);
    if (txn_index_store->fulltext_index_entry_ != nullptr) {
        UnrecoverableError("Attempt to add fulltext index store twice");
    }
    txn_index_store->fulltext_index_entry_ = fulltext_index_entry;
}

void TxnTableStore::DropIndexStore(TableIndexEntry *table_index_entry) {
    if (txn_indexes_.contains(table_index_entry)) {
        txn_indexes_.erase(table_index_entry);
        txn_indexes_store_.erase(*table_index_entry->GetIndexName());
        table_index_entry->Cleanup();
    } else {
        txn_indexes_.insert(table_index_entry);
    }
}

TxnIndexStore *TxnTableStore::GetIndexStore(TableIndexEntry *table_index_entry) {
    const String &index_name = *table_index_entry->GetIndexName();
    if (auto iter = txn_indexes_store_.find(index_name); iter != txn_indexes_store_.end()) {
        return iter->second.get();
    }
    auto txn_index_store = MakeUnique<TxnIndexStore>(table_index_entry);
    auto *ptr = txn_index_store.get();
    txn_indexes_store_.emplace(index_name, std::move(txn_index_store));
    return ptr;
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
    if (compact_state_.task_type_ != CompactSegmentsTaskType::kInvalid) {
        UnrecoverableError("Attempt to compact table store twice");
    }
    compact_state_ = TxnCompactStore(type);
    for (auto &[new_segment, old_segments] : segment_data) {
        auto txn_segment_store = TxnSegmentStore::AddSegmentStore(new_segment.get());
        compact_state_.compact_data_.emplace_back(std::move(txn_segment_store), std::move(old_segments));

        this->AddSegmentStore(new_segment.get());
        table_entry_->AddCompactNew(std::move(new_segment));
    }
    return {nullptr, Status::OK()};
}

void TxnTableStore::Scan(SharedPtr<DataBlock> &) {}

void TxnTableStore::Rollback() {
    TransactionID txn_id = txn_->TxnID();
    if (append_state_.get() != nullptr) {
        // Rollback the data already been appended.
        Catalog::RollbackAppend(table_entry_, txn_->TxnID(), txn_->CommitTS(), this);
        LOG_TRACE(fmt::format("Rollback prepare appended data in table: {}", *table_entry_->GetTableName()));
    }
    for (auto &[index_name, txn_index_store] : txn_indexes_store_) {
        Catalog::RollbackCreateIndex(txn_index_store.get());
        auto *table_index_entry = txn_index_store->table_index_entry_;
        table_index_entry->Cleanup();
        Catalog::RemoveIndexEntry(index_name, table_index_entry, txn_id);
    }
    Catalog::RollbackCompact(table_entry_, txn_->TxnID(), txn_->CommitTS(), compact_state_);
    blocks_.clear();
}

void TxnTableStore::PrepareCommit() {
    // Init append state
    append_state_ = MakeUnique<AppendState>(this->blocks_);

    // Start to append
    LOG_TRACE(fmt::format("Transaction local storage table: {}, Start to prepare commit", *table_entry_->GetTableName()));
    Catalog::Append(table_entry_, txn_->TxnID(), this, txn_->CommitTS(), txn_->buffer_manager());

    // Attention: "compact" needs to be ahead of "delete"
    if (compact_state_.task_type_ != CompactSegmentsTaskType::kInvalid) {
        LOG_INFO(fmt::format("Commit compact, table dir: {}, commit ts: {}", *table_entry_->TableEntryDir(), txn_->CommitTS()));
        Catalog::CommitCompact(table_entry_, txn_->TxnID(), this, txn_->CommitTS(), compact_state_);
    }

    Catalog::Delete(table_entry_, txn_->TxnID(), this, txn_->CommitTS(), delete_state_);

    LOG_TRACE(fmt::format("Transaction local storage table: {}, Complete commit preparing", *table_entry_->GetTableName()));
}

/**
 * @brief Call for really commit the data to disk.
 */
void TxnTableStore::Commit() const {
    Catalog::CommitWrite(table_entry_, txn_->TxnID(), txn_->CommitTS(), txn_segments_);
    // Catalog::CommitAppend(table_entry_, txn_->TxnID(), txn_->CommitTS(), append_state_.get());
    // Catalog::CommitDelete(table_entry_, txn_->TxnID(), txn_->CommitTS(), delete_state_);
    for (const auto &[index_name, txn_index_store] : txn_indexes_store_) {
        Catalog::CommitCreateIndex(txn_index_store.get(), txn_->CommitTS());
    }
    for (auto *table_index_entry : txn_indexes_) {
        table_index_entry->Commit(txn_->CommitTS());
    }
}

void TxnTableStore::TryTriggerCompaction(BGTaskProcessor *bg_task_processor, TxnManager *txn_mgr) {
    std::function<Txn *()> generate_txn = [txn_mgr]() { return txn_mgr->CreateTxn(); };

    // FIXME OPT: trigger compaction one time for all segments
    for (auto *sealed_segment : set_sealed_segments_) {
        auto ret = table_entry_->TryCompactAddSegment(sealed_segment, generate_txn);
        if (!ret.has_value()) {
            continue;
        }
        auto &[to_compacts, txn] = *ret;
        auto compact_task = CompactSegmentsTask::MakeTaskWithPickedSegments(table_entry_, std::move(to_compacts), txn);
        bg_task_processor->Submit(std::move(compact_task));
    }
    for (const auto &[segment_id, delete_map] : delete_state_.rows_) {
        auto ret = table_entry_->TryCompactDeleteRow(segment_id, generate_txn);
        if (!ret.has_value()) {
            continue;
        }
        auto &[to_compacts, txn] = *ret;
        auto compact_task = CompactSegmentsTask::MakeTaskWithPickedSegments(table_entry_, std::move(to_compacts), txn);
        bg_task_processor->Submit(std::move(compact_task));
    }
}

void TxnTableStore::AddSegmentStore(SegmentEntry *segment_entry) {
    auto [iter, insert_ok] = txn_segments_.emplace(segment_entry->segment_id(), TxnSegmentStore::AddSegmentStore(segment_entry));
    if (!insert_ok) {
        UnrecoverableError(fmt::format("Attempt to add segment store twice"));
    }
}

void TxnTableStore::AddBlockStore(SegmentEntry *segment_entry, BlockEntry *block_entry) {
    auto iter = txn_segments_.find(segment_entry->segment_id());
    if (iter == txn_segments_.end()) {
        iter = txn_segments_.emplace(segment_entry->segment_id(), TxnSegmentStore::AddSegmentStore(segment_entry)).first;
    }
    iter->second.block_entries_.emplace_back(block_entry);
}

void TxnTableStore::AddSealedSegment(SegmentEntry *segment_entry) { set_sealed_segments_.emplace_back(segment_entry); }

void TxnTableStore::AddCatalogDeltaOperation(CatalogDeltaEntry *local_catalog_delta_ops_entry) const {
    for (const auto &[index_name, index_store] : txn_indexes_store_) {
        index_store->AddCatalogDeltaOperation(local_catalog_delta_ops_entry);
    }
}

} // namespace infinity
