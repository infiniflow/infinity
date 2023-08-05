//
// Created by jinhai on 23-6-4.
//

#include "txn_store.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

UniquePtr<String>
TxnTableStore::Append(const SharedPtr<DataBlock>& input_block) {
    SizeT column_count = table_entry_->GetTableDesc()->column_count();
    if(input_block->column_count() != column_count) {
        String err_msg = fmt::format("Attempt to insert different column count data block into transaction table store");
        LOG_ERROR(err_msg);
        return MakeUnique<String>(err_msg);
    }

    Vector<DataType> column_types;
    for(SizeT col_id = 0; col_id < column_count; ++ col_id) {
        column_types.emplace_back(table_entry_->GetTableDesc()->columns()[col_id]->type());
        if(column_types.back() != input_block->column_vectors[col_id]->data_type()) {
            String err_msg = fmt::format("Attempt to insert different type data into transaction table store");
            LOG_ERROR(err_msg);
            return MakeUnique<String>(err_msg);
        }
    }

    DataBlock* current_block {nullptr};
    if(blocks_.empty()) {
        blocks_.emplace_back(DataBlock::Make());
        current_block_id_ = 0;
        blocks_.back()->Init(column_types);
    }
    current_block = blocks_[current_block_id_].get();

    SizeT input_start_pos = 0;
    if(current_block->row_count() + input_block->row_count() > current_block->capacity()) {
        SizeT to_append = current_block->capacity() - current_block->row_count();
        current_block->AppendWith(input_block, input_start_pos, to_append);
        current_block->Finalize();
        input_start_pos = to_append;
        to_append = input_block->row_count() - input_start_pos;
        blocks_.emplace_back(DataBlock::Make());
        blocks_.back()->Init(column_types);
        ++ current_block_id_;
        current_block = blocks_[current_block_id_].get();
        current_block->AppendWith(input_block, input_start_pos, to_append);
    } else {
        SizeT to_append = input_block->row_count();
        current_block->AppendWith(input_block, input_start_pos, to_append);
    }
    current_block->Finalize();

    return nullptr;
}

UniquePtr<String>
TxnTableStore::Delete(const Vector<RowID>& row_ids) {
    NotImplementError("TxnTableStore::Delete")
}

void
TxnTableStore::Scan(SharedPtr<DataBlock>& output_block) {

}

void
TxnTableStore::Rollback() {
    if(append_state_ != nullptr) {
        // Rollback the data already been appended.
        table_entry_->GetDataTable()->RollbackAppend(txn_, this);
        LOG_TRACE("Rollback prepare appended data in table: {}", table_entry_->GetTableDesc()->table_name());
    }

    blocks_.clear();
}

void
TxnTableStore::PrepareCommit() {
    // Init append state
    append_state_ = MakeUnique<AppendState>(this->blocks_);

    // Start to append
    LOG_TRACE("Transaction local storage table: {}, Start to prepare commit", this->table_name_);
    table_entry_->GetDataTable()->Append(txn_, this);
    LOG_TRACE("Transaction local storage table: {}, Complete commit preparing", this->table_name_);
}

void
TxnTableStore::Commit() {

    table_entry_->GetDataTable()->CommitAppend(txn_, append_state_.get());

//    for(const auto& range: append_state_->append_ranges_) {
//        LOG_TRACE("Commit, segment: {}, start: {}, count: {}", range.segment_id_, range.start_pos_, range.row_count_);
//    }
    LOG_ERROR("Not implements the txn table store commit, table {}", this->table_name_);
    LOG_ERROR("Not implement to flush the new appended data segment")
}

}
