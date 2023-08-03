//
// Created by jinhai on 23-7-1.
//

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
        columns_.back()->Init(column_defs[column_id].get(), row_capacity_);
    }

    status_ = DataSegmentStatus::kOpen;
}

void
DataSegment::Append(void* ptr, AppendState& append_state) {
    if(status_ != DataSegmentStatus::kOpen) {
        StorageError("Attempt to append data into Non-Open status data segment");
    }

    rw_locker_.lock();
    DeferFn defer_fn([&]() {
        rw_locker_.unlock();
    });

    if(AvailableCapacity() >= append_state.total_count_) {
        // All appended data will be inserted into this segment;
        SizeT block_count = append_state.blocks_.size();
        SizeT column_count = columns_.size();
        while(append_state.current_block_ < block_count) {

            const SharedPtr<DataBlock>& input_block = append_state.blocks_[append_state.current_block_];

            for(SizeT column_id = 0; column_id < column_count; ++ column_id) {
                columns_[column_id]->Append(input_block->column_vectors[column_id],
                                            append_state.current_block_offset_,
                                            current_row_,
                                            input_block->row_count());
                LOG_TRACE("Column: {} is appended with {} rows", column_id, input_block->row_count())
            }

            for(SizeT i = 0; i < input_block->row_count(); ++ i) {
                block_verions_.txn_ptr_[i] = (u64)ptr;
            }

            ++ append_state.current_block_;
            append_state.current_block_offset_ = 0;
            current_row_ += input_block->row_count();
        }

        append_state.current_count_ = append_state.total_count_;
    } else {
        SizeT block_count = append_state.blocks_.size();
        SizeT column_count = columns_.size();
        while(append_state.current_block_ < block_count) {

            const SharedPtr<DataBlock>& input_block = append_state.blocks_[append_state.current_block_];

            if(current_row_ + input_block->row_count() <= row_capacity_) {
                for(SizeT column_id = 0; column_id < column_count; ++ column_id) {
                    columns_[column_id]->Append(input_block->column_vectors[column_id],
                                                append_state.current_block_offset_,
                                                current_row_,
                                                input_block->row_count());

                    LOG_TRACE("Column: {} is appended with {} rows", column_id, input_block->row_count())
                }

                for(SizeT i = 0; i < input_block->row_count(); ++ i) {
                    block_verions_.txn_ptr_[i] = (u64)ptr;
                }

                ++ append_state.current_block_;
                append_state.current_block_offset_ = 0;
                current_row_ += input_block->row_count();
            } else {

                SizeT to_copy_rows = row_capacity_ - current_row_;

                for(SizeT column_id = 0; column_id < column_count; ++ column_id) {
                    columns_[column_id]->Append(input_block->column_vectors[column_id],
                                                append_state.current_block_offset_,
                                                current_row_,
                                                to_copy_rows);

                    LOG_TRACE("Column: {} is appended with {} rows", column_id, to_copy_rows)
                }

                for(SizeT i = 0; i < to_copy_rows; ++ i) {
                    block_verions_.txn_ptr_[i] = (u64)ptr;
                }

                current_row_ += to_copy_rows;
                append_state.current_block_offset_ += to_copy_rows;

                break;
            }
        }
        append_state.current_count_ += row_capacity_;
    }
}

UniquePtr<String>
DataSegment::Delete(void* txn_ptr, DeleteState& delete_state) {

}

UniquePtr<String>
DataSegment::InitScan(void* txn_ptr, ScanState& scan_state) {

}

UniquePtr<String>
DataSegment::Scan(void* txn_ptr, ScanState scan_state) {

}

UniquePtr<String>
DataSegment::CommitAppend(void* txn_ptr, u32 block_id, u32 block_offset, u32 row_count) {

}

UniquePtr<String>
DataSegment::RevertAppend(void* txn_ptr, u32 block_id, u32 block_offset, u32 row_count) {

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
    for(const auto& column_data: columns_) {
        column_data->Flush();
    }

    DataSegmentStatus expected = DataSegmentStatus::kFlushing;
    if(!status_.compare_exchange_strong(expected, DataSegmentStatus::kClosed, std::memory_order_seq_cst)) {
        return MakeUnique<String>("Data segment is expected as flushing status");
    }
}

}