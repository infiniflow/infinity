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

import stl;
import third_party;
import parser;

import infinity_exception;
import data_block;
import logger;
import data_access_state;
import txn;
import default_values;
import catalog;

module txn_store;

namespace infinity {

TxnIndexStore::TxnIndexStore(TableIndexEntry *table_index_entry) : table_index_entry_(table_index_entry) {}

UniquePtr<String> TxnTableStore::Append(const SharedPtr<DataBlock> &input_block) {
    SizeT column_count = table_entry_->ColumnCount();
    if (input_block->column_count() != column_count) {
        String err_msg = Format("Attempt to insert different column count data block into transaction table store");
        LOG_ERROR(err_msg);
        return MakeUnique<String>(err_msg);
    }

    Vector<SharedPtr<DataType>> column_types;
    for (SizeT col_id = 0; col_id < column_count; ++col_id) {
        column_types.emplace_back(table_entry_->GetColumnDefByID(col_id)->type());
        if (*column_types.back() != *input_block->column_vectors[col_id]->data_type()) {
            String err_msg = Format("Attempt to insert different type data into transaction table store");
            LOG_ERROR(err_msg);
            return MakeUnique<String>(err_msg);
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
        current_block->AppendWith(input_block, 0, to_append);
        current_block->Finalize();

        blocks_.emplace_back(DataBlock::Make());
        blocks_.back()->Init(column_types);
        ++current_block_id_;
        current_block = blocks_[current_block_id_].get();
        current_block->AppendWith(input_block, to_append, input_block->row_count() - to_append);
    } else {
        SizeT to_append = input_block->row_count();
        current_block->AppendWith(input_block, 0, to_append);
    }
    current_block->Finalize();

    return nullptr;
}

UniquePtr<String> TxnTableStore::Import(const SharedPtr<SegmentEntry> &segment) {
    uncommitted_segments_.emplace_back(segment);
    return nullptr;
}

UniquePtr<String>
TxnTableStore::CreateIndexFile(TableIndexEntry *table_index_entry, u64 column_id, u32 segment_id, SharedPtr<SegmentColumnIndexEntry> index) {
    const String &index_name = *table_index_entry->index_def()->index_name_;
    if (auto column_index_iter = txn_indexes_store_.find(index_name); column_index_iter != txn_indexes_store_.end()) {
        TxnIndexStore *txn_index_store = &(column_index_iter->second);
        if (auto segment_column_index_iter = txn_index_store->index_entry_map_.find(column_id);
            segment_column_index_iter != txn_index_store->index_entry_map_.end()) {
            segment_column_index_iter->second.emplace(segment_id, index);
            column_index_iter->second.index_entry_map_[column_id].emplace(segment_id, index);
        } else {
            column_index_iter->second.index_entry_map_[column_id][segment_id] = index;
        }
    } else {
        TxnIndexStore index_store(table_index_entry);
        index_store.index_entry_map_[column_id][segment_id] = index;
        txn_indexes_store_.emplace(index_name, Move(index_store));
    }
    return nullptr;
}

UniquePtr<String> TxnTableStore::Delete(const Vector<RowID> &row_ids) {
    //    auto &rows = delete_state_.rows_;
    HashMap<u32, HashMap<u16, Vector<RowID>>> &row_hash_table = delete_state_.rows_;
    for (auto row_id : row_ids) {
        auto seg_it = row_hash_table.find(row_id.segment_id_);
        if (seg_it != row_hash_table.end()) {
            u16 block_id = row_id.segment_offset_ / DEFAULT_BLOCK_CAPACITY;
            auto block_it = seg_it->second.find(block_id);
            if (block_it != seg_it->second.end()) {
                block_it->second.push_back(row_id);
            } else {
                seg_it->second.emplace(block_id, Vector<RowID>{row_id});
            }
        } else {
            HashMap<u16, Vector<RowID>> block_row_hash_table;
            u16 block_id = row_id.segment_offset_ / DEFAULT_BLOCK_CAPACITY;
            block_row_hash_table.emplace(block_id, Vector<RowID>{row_id});
            row_hash_table.emplace(row_id.segment_id_, block_row_hash_table);
        }
    }

    return nullptr;
}

void TxnTableStore::Scan(SharedPtr<DataBlock> &) {}

void TxnTableStore::Rollback() {
    if (append_state_.get() != nullptr) {
        // Rollback the data already been appended.
        NewCatalog::RollbackAppend(table_entry_, txn_->TxnID(), txn_->CommitTS(), this);
        LOG_TRACE(Format("Rollback prepare appended data in table: {}", *table_entry_->GetTableName()));
    }

    blocks_.clear();
}

void TxnTableStore::PrepareCommit() {
    // Init append state
    append_state_ = MakeUnique<AppendState>(this->blocks_);

    // Start to append
    LOG_TRACE(Format("Transaction local storage table: {}, Start to prepare commit", *table_entry_->GetTableName()));
    Txn *txn_ptr = (Txn *)txn_;
    NewCatalog::Append(table_entry_, txn_->TxnID(), this, txn_ptr->GetBufferMgr());

    SizeT segment_count = uncommitted_segments_.size();
    for (SizeT seg_idx = 0; seg_idx < segment_count; ++seg_idx) {
        const auto &uncommitted = uncommitted_segments_[seg_idx];
        // Segments in `uncommitted_segments_` are already persisted. Import them to memory catalog.
        NewCatalog::ImportSegment(table_entry_, txn_->CommitTS(), uncommitted);
    }

    NewCatalog::Delete(table_entry_, txn_->TxnID(), txn_->CommitTS(), delete_state_);
    NewCatalog::CommitCreateIndex(txn_indexes_store_);

    LOG_TRACE(Format("Transaction local storage table: {}, Complete commit preparing", *table_entry_->GetTableName()));
}

/**
 * @brief Call for really commit the data to disk.
 */
void TxnTableStore::Commit() const {
    NewCatalog::CommitAppend(table_entry_, txn_->TxnID(), txn_->CommitTS(), append_state_.get());
    NewCatalog::CommitDelete(table_entry_, txn_->TxnID(), txn_->CommitTS(), delete_state_);
}

} // namespace infinity
