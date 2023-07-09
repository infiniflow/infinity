//
// Created by jinhai on 23-6-4.
//

#include "txn_store.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

UniquePtr<String>
TxnTableStore::Append(const SharedPtr<DataBlock>& input_block) {
    SizeT column_count = table_def_->column_count();
    if(input_block->column_count() != column_count) {
        String err_msg = fmt::format("Attempt to insert different column count data block into transaction table store");
        LOG_ERROR(err_msg);
        return MakeUnique<String>(err_msg);
    }

    for(SizeT col_id = 0; col_id < column_count; ++ col_id) {
        if(table_def_->columns()[col_id]->type() != input_block->column_vectors[col_id]->data_type()) {
            String err_msg = fmt::format("Attempt to insert different type data into transaction table store");
            LOG_ERROR(err_msg);
            return MakeUnique<String>(err_msg);
        }
    }

    DataBlock* current_block {nullptr};
    if(blocks_.empty()) {
        blocks_.emplace_back(DataBlock::Make());
        current_block_id_ = 0;
        blocks_.back()->Init(input_block->column_vectors);
    }
    current_block = blocks_[current_block_id_].get();

    SizeT input_start_pos = 0;
    if(current_block->row_count() + input_block->row_count() > current_block->capacity()) {
        SizeT to_append = current_block->capacity() - current_block->row_count();
        current_block->AppendWith(input_block, input_start_pos, to_append);
        input_start_pos = to_append;
        to_append = input_block->row_count() - input_start_pos;
        blocks_.emplace_back(DataBlock::Make());
        ++ current_block_id_;
        current_block = blocks_[current_block_id_].get();
        current_block->AppendWith(input_block, input_start_pos, to_append);
    } else {
        SizeT to_append = input_block->row_count();
        current_block->AppendWith(input_block, input_start_pos, to_append);
    }

    return nullptr;
}

UniquePtr<String>
TxnTableStore::Delete(const Vector<RowID>& row_ids) {
    NotImplementError("TxnTableStore::Delete")
}

}
