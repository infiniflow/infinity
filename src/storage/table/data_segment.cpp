//
// Created by jinhai on 23-7-1.
//


#if 0

#include "data_segment.h"
#include "common/utility/defer_op.h"
#include "storage/txn/txn.h"

namespace infinity {

void
DataSegment::Init(const Vector<SharedPtr<ColumnDef>>& column_defs,
                  const SharedPtr<String>& base_dir,
                  void* buffer_mgr) {
    dir_ = MakeShared<String>(*base_dir + "/seg" + std::to_string(segment_id_));

    SizeT column_count = column_defs.size();
    for(SizeT column_id = 0; column_id < column_count; ++ column_id) {
        columns_.emplace_back(MakeShared<ColumnData>(dir_, column_id, (BufferManager*)buffer_mgr));
        columns_.back()->Init(column_defs[column_id], row_capacity_);
    }

    status_ = DataSegmentStatus::kOpen;
}

void
DataSegment::Append(void* txn_ptr, AppendState* append_state_ptr) {
    if(status_ != DataSegmentStatus::kOpen) {
        StorageError("Attempt to append data into Non-Open status data segment");
    }

    rw_locker_.lock();
    DeferFn defer_fn([&]() {
        rw_locker_.unlock();
    });

    if(AvailableCapacity() >= append_state_ptr->total_count_) {
        // All appended data will be inserted into this segment;
        SizeT block_count = append_state_ptr->blocks_.size();
        SizeT column_count = columns_.size();

        while(append_state_ptr->current_block_ < block_count) {

            const SharedPtr<DataBlock>& input_block = append_state_ptr->blocks_[append_state_ptr->current_block_];

            u64 range_segment_id = this->segment_id_;
            u64 range_segment_start_pos = current_row_;
            u64 range_segment_row_count = input_block->row_count();
            append_state_ptr->append_ranges_.emplace_back(range_segment_id,
                                                          range_segment_start_pos,
                                                          range_segment_row_count);

            for(SizeT column_id = 0; column_id < column_count; ++ column_id) {
                columns_[column_id]->Append(input_block->column_vectors[column_id],
                                            append_state_ptr->current_block_offset_,
                                            current_row_,
                                            input_block->row_count());
                LOG_TRACE("Column: {} is appended with {} rows", column_id, input_block->row_count())
            }

            for(SizeT i = 0; i < input_block->row_count(); ++ i) {
                block_verions_.txn_ptr_[i] = (u64)txn_ptr;
            }

            ++ append_state_ptr->current_block_;
            append_state_ptr->current_block_offset_ = 0;
            current_row_ += input_block->row_count();
        }

        append_state_ptr->current_count_ = append_state_ptr->total_count_;
    } else {
        SizeT block_count = append_state_ptr->blocks_.size();
        SizeT column_count = columns_.size();
        while(append_state_ptr->current_block_ < block_count) {

            const SharedPtr<DataBlock>& input_block = append_state_ptr->blocks_[append_state_ptr->current_block_];

            u64 range_segment_id = this->segment_id_;
            u64 range_segment_start_pos = current_row_;
            u64 range_segment_row_count = input_block->row_count();
            append_state_ptr->append_ranges_.emplace_back(range_segment_id,
                                                          range_segment_start_pos,
                                                          range_segment_row_count);

            if(current_row_ + input_block->row_count() <= row_capacity_) {
                for(SizeT column_id = 0; column_id < column_count; ++ column_id) {
                    columns_[column_id]->Append(input_block->column_vectors[column_id],
                                                append_state_ptr->current_block_offset_,
                                                current_row_,
                                                input_block->row_count());

                    LOG_TRACE("Column: {} is appended with {} rows", column_id, input_block->row_count())
                }

                for(SizeT i = 0; i < input_block->row_count(); ++ i) {
                    block_verions_.txn_ptr_[i] = (u64)txn_ptr;
                }

                ++ append_state_ptr->current_block_;
                append_state_ptr->current_block_offset_ = 0;
                current_row_ += input_block->row_count();
            } else {

                SizeT to_copy_rows = row_capacity_ - current_row_;

                for(SizeT column_id = 0; column_id < column_count; ++ column_id) {
                    columns_[column_id]->Append(input_block->column_vectors[column_id],
                                                append_state_ptr->current_block_offset_,
                                                current_row_,
                                                to_copy_rows);

                    LOG_TRACE("Column: {} is appended with {} rows", column_id, to_copy_rows)
                }

                for(SizeT i = 0; i < to_copy_rows; ++ i) {
                    block_verions_.txn_ptr_[i] = (u64)txn_ptr;
                }

                current_row_ += to_copy_rows;
                append_state_ptr->current_block_offset_ += to_copy_rows;

                break;
            }
        }
        append_state_ptr->current_count_ += row_capacity_;
    }
}

UniquePtr<String>
DataSegment::Delete(void* txn_ptr, DeleteState& delete_state) {

}

UniquePtr<String>
DataSegment::InitScan(void* txn_ptr, ScanState& scan_state) {

}

UniquePtr<String>
DataSegment::Scan(void* txn_ptr, ScanState* scan_state) {

}

void
DataSegment::CommitAppend(void* ptr, u64 start_pos, u64 row_count) {
    Txn* txn_ptr = (Txn*)ptr;
    u64 end_pos = start_pos + row_count;
    for(SizeT i = start_pos; i < end_pos; ++ i) {
        block_verions_.created_[i] = txn_ptr->CommitTS();
    }
}

UniquePtr<String>
DataSegment::RevertAppend(void* ptr, u64 start_pos, u64 row_count) {

}

UniquePtr<String>
DataSegment::CommitDelete(void* txn_ptr, const Vector<RowID>& row_ids) {

}

bool
DataSegment::PrepareFlush() {
    DataSegmentStatus expected = DataSegmentStatus::kOpen;
    return status_.compare_exchange_strong(expected, DataSegmentStatus::kFlushing, std::memory_order_seq_cst);
}

UniquePtr<String>
DataSegment::Flush() {
    LOG_TRACE("DataSegment: {} is being flushed", this->segment_id_);
    for(SizeT column_id = 0; const auto& column_data: columns_) {
        column_data->Flush(current_row_);
        LOG_TRACE("ColumnData: {} is flushed", column_id);
        ++ column_id;
    }

    DataSegmentStatus expected = DataSegmentStatus::kFlushing;
    if(!status_.compare_exchange_strong(expected, DataSegmentStatus::kClosed, std::memory_order_seq_cst)) {
        return MakeUnique<String>("Data segment is expected as flushing status");
    }
    LOG_TRACE("DataSegment: {} is being flushed", this->segment_id_);

    return nullptr;
}

}

#endif
