//
// Created by jinhai on 23-6-4.
//

module;

#include <vector>

import stl;
import third_party;
import parser;
import table_collection_entry;
import table_collection_meta;
import infinity_assert;
import infinity_exception;
import segment_entry;
import data_block;
import logger;
import data_access_state;
import txn;
import index_entry;
import default_values;

module txn_store;

namespace infinity {

UniquePtr<String> TxnTableStore::Append(const SharedPtr<DataBlock> &input_block) {
    SizeT column_count = table_entry_->columns_.size();
    if (input_block->column_count() != column_count) {
        String err_msg = Format("Attempt to insert different column count data block into transaction table store");
        LOG_ERROR(err_msg);
        return MakeUnique<String>(err_msg);
    }

    Vector<SharedPtr<DataType>> column_types;
    for (SizeT col_id = 0; col_id < column_count; ++col_id) {
        column_types.emplace_back(table_entry_->columns_[col_id]->type());
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

    SizeT input_start_pos = 0;
    if (current_block->row_count() + input_block->row_count() > current_block->capacity()) {
        SizeT to_append = current_block->capacity() - current_block->row_count();
        current_block->AppendWith(input_block, input_start_pos, to_append);
        current_block->Finalize();
        input_start_pos = to_append;
        to_append = input_block->row_count() - input_start_pos;
        blocks_.emplace_back(DataBlock::Make());
        blocks_.back()->Init(column_types);
        ++current_block_id_;
        current_block = blocks_[current_block_id_].get();
        current_block->AppendWith(input_block, input_start_pos, to_append);
    } else {
        SizeT to_append = input_block->row_count();
        current_block->AppendWith(input_block, input_start_pos, to_append);
    }
    current_block->Finalize();

    return nullptr;
}

UniquePtr<String> TxnTableStore::Import(const SharedPtr<SegmentEntry> &segment) {
    uncommitted_segments_.emplace_back(segment);
    return nullptr;
}

UniquePtr<String> TxnTableStore::CreateIndexFile(u32 segment_id, SharedPtr<IndexEntry> index) {
    uncommitted_indexes_.emplace(segment_id, Move(index));
    return nullptr;
}

UniquePtr<String> TxnTableStore::Delete(const Vector<RowID> &row_ids) {
//    auto &rows = delete_state_.rows_;
    HashMap<u32, HashMap<u16, Vector<RowID>>> &row_hash_table = delete_state_.rows_;
    for(auto row_id: row_ids) {
        auto seg_it = row_hash_table.find(row_id.segment_id_);
        if(seg_it != row_hash_table.end()) {
            u16 block_id = row_id.segment_offset_ / DEFAULT_BLOCK_CAPACITY;
            auto block_it = seg_it->second.find(block_id);
            if(block_it != seg_it->second.end()) {
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

void TxnTableStore::Scan(SharedPtr<DataBlock> &output_block) {}

void TxnTableStore::Rollback() {
    if (append_state_.get() != nullptr) {
        // Rollback the data already been appended.
        TableCollectionEntry::RollbackAppend(table_entry_, txn_, this);
        TableCollectionMeta *table_meta = (TableCollectionMeta *)TableCollectionEntry::GetTableMeta(table_entry_);
        LOG_TRACE(Format("Rollback prepare appended data in table: {}", *table_meta->table_collection_name_));
    }

    blocks_.clear();
}

void TxnTableStore::PrepareCommit() {
    // Init append state
    append_state_ = MakeUnique<AppendState>(this->blocks_);

    // Start to append
    LOG_TRACE(Format("Transaction local storage table: {}, Start to prepare commit", this->table_name_));
    Txn *txn_ptr = (Txn *)txn_;
    TableCollectionEntry::Append(table_entry_, txn_, this, txn_ptr->GetBufferMgr());

    SizeT segment_count = uncommitted_segments_.size();
    for (SizeT seg_idx = 0; seg_idx < segment_count; ++seg_idx) {
        const auto &uncommitted = uncommitted_segments_[seg_idx];
        // Segments in `uncommitted_segments_` are already persisted. Import them to memory catalog.
        TableCollectionEntry::ImportSegment(table_entry_, txn_, uncommitted);
    }

    TableCollectionEntry::Delete(table_entry_, txn_, delete_state_);

    TableCollectionEntry::CommitCreateIndex(table_entry_, uncommitted_indexes_);

    LOG_TRACE(Format("Transaction local storage table: {}, Complete commit preparing", this->table_name_));
}

/**
 * @brief Call for really commit the data to disk.
 */
void TxnTableStore::Commit() const {
    TableCollectionEntry::CommitAppend(table_entry_, txn_, append_state_.get());
    TableCollectionEntry::CommitDelete(table_entry_, txn_, delete_state_);
}

} // namespace infinity
