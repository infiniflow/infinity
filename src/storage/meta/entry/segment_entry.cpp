//
// Created by jinhai on 23-8-11.
//

#include "segment_entry.h"
#include "common/utility/defer_op.h"
#include "storage/txn/txn.h"

namespace infinity {

SharedPtr<SegmentEntry>
SegmentEntry::MakeNewSegmentEntry(const TableCollectionEntry* table_entry,
                                  u64 txn_id,
                                  u64 segment_id,
                                  BufferManager* buffer_mgr,
                                  SizeT segment_row) {

    SharedPtr<SegmentEntry> new_entry = MakeShared<SegmentEntry>(table_entry);
    new_entry->row_capacity_ = segment_row;
    new_entry->current_row_ = 0;
    new_entry->segment_id_ = segment_id;
    new_entry->status_ = DataSegmentStatus::kOpen;
    new_entry->segment_version_ = MakeUnique<SegmentVersion>(segment_row);

    const auto* table_ptr = (const TableCollectionEntry*)table_entry;
    new_entry->base_dir_ = MakeShared<String>(*table_ptr->base_dir_ + '/' + std::to_string(txn_id));

    const Vector<SharedPtr<ColumnDef>>& columns = table_ptr->columns_;
    new_entry->columns_.reserve(columns.size());
    for(const auto& column_def: columns) {
        new_entry->columns_.emplace_back(ColumnDataEntry::MakeNewColumnDataEntry(new_entry.get(),
                                                                                 column_def->id(),
                                                                                 segment_row,
                                                                                 column_def->type(),
                                                                                 buffer_mgr
        ));
    }
    return new_entry;
}

void
SegmentEntry::AppendData(SegmentEntry* segment_entry,
                         Txn* txn_ptr,
                         AppendState* append_state_ptr,
                         BufferManager* buffer_mgr) {
    if(segment_entry->status_ != DataSegmentStatus::kOpen) {
        StorageError("Attempt to append data into Non-Open status data segment");
    }

    segment_entry->rw_locker_.lock();
    DeferFn defer_fn([&]() {
        segment_entry->rw_locker_.unlock();
    });

    SizeT available_capacity = segment_entry->row_capacity_ - segment_entry->current_row_;
    if(available_capacity >= append_state_ptr->total_count_) {
        // All appended data will be inserted into this segment;
        SizeT block_count = append_state_ptr->blocks_.size();
        SizeT column_count = segment_entry->columns_.size();

        while(append_state_ptr->current_block_ < block_count) {

            const SharedPtr<DataBlock>& input_block = append_state_ptr->blocks_[append_state_ptr->current_block_];

            u64 range_segment_id = segment_entry->segment_id_;
            u64 range_segment_start_pos = segment_entry->current_row_;
            u64 range_segment_row_count = input_block->row_count();
            append_state_ptr->append_ranges_.emplace_back(range_segment_id,
                                                          range_segment_start_pos,
                                                          range_segment_row_count);

            for(SizeT column_id = 0; column_id < column_count; ++ column_id) {
                ColumnDataEntry::Append(segment_entry->columns_[column_id].get(),
                                        input_block->column_vectors[column_id],
                                        append_state_ptr->current_block_offset_,
                                        segment_entry->current_row_,
                                        input_block->row_count());
                LOG_TRACE("Column: {} is appended with {} rows", column_id, input_block->row_count())
            }

            for(SizeT i = 0; i < input_block->row_count(); ++ i) {
                segment_entry->segment_version_->txn_ptr_[i] = (u64)txn_ptr;
            }

            ++ append_state_ptr->current_block_;
            append_state_ptr->current_block_offset_ = 0;
            segment_entry->current_row_ += input_block->row_count();
        }

        append_state_ptr->current_count_ = append_state_ptr->total_count_;
    } else {
        SizeT block_count = append_state_ptr->blocks_.size();
        SizeT column_count = segment_entry->columns_.size();
        while(append_state_ptr->current_block_ < block_count) {

            const SharedPtr<DataBlock>& input_block = append_state_ptr->blocks_[append_state_ptr->current_block_];

            u64 range_segment_id = segment_entry->segment_id_;
            u64 range_segment_start_pos = segment_entry->current_row_;
            u64 range_segment_row_count = input_block->row_count();
            append_state_ptr->append_ranges_.emplace_back(range_segment_id,
                                                          range_segment_start_pos,
                                                          range_segment_row_count);

            if(segment_entry->current_row_ + input_block->row_count() <= segment_entry->row_capacity_) {
                for(SizeT column_id = 0; column_id < column_count; ++ column_id) {
                    ColumnDataEntry::Append(segment_entry->columns_[column_id].get(),
                                            input_block->column_vectors[column_id],
                                            append_state_ptr->current_block_offset_,
                                            segment_entry->current_row_,
                                            input_block->row_count());

                    LOG_TRACE("Column: {} is appended with {} rows", column_id, input_block->row_count())
                }

                for(SizeT i = 0; i < input_block->row_count(); ++ i) {
                    segment_entry->segment_version_->txn_ptr_[i] = (u64)txn_ptr;
                }

                ++ append_state_ptr->current_block_;
                append_state_ptr->current_block_offset_ = 0;
                segment_entry->current_row_ += input_block->row_count();
            } else {

                SizeT to_copy_rows = segment_entry->row_capacity_ - segment_entry->current_row_;

                for(SizeT column_id = 0; column_id < column_count; ++ column_id) {
                    ColumnDataEntry::Append(segment_entry->columns_[column_id].get(),
                                            input_block->column_vectors[column_id],
                                            append_state_ptr->current_block_offset_,
                                            segment_entry->current_row_,
                                            to_copy_rows);

                    LOG_TRACE("Column: {} is appended with {} rows", column_id, to_copy_rows)
                }

                for(SizeT i = 0; i < to_copy_rows; ++ i) {
                    segment_entry->segment_version_->txn_ptr_[i] = (u64)txn_ptr;
                }

                segment_entry->current_row_ += to_copy_rows;
                append_state_ptr->current_block_offset_ += to_copy_rows;

                break;
            }
        }
        append_state_ptr->current_count_ += segment_entry->row_capacity_;
    }
}

void
SegmentEntry::CommitAppend(SegmentEntry* segment_entry, Txn* txn_ptr, u64 start_pos, u64 row_count) {
    u64 end_pos = start_pos + row_count;
    Vector<au64>& create_vector = segment_entry->segment_version_->created_;
    for(SizeT i = start_pos; i < end_pos; ++ i) {
        create_vector[i] = txn_ptr->CommitTS();
    }
}

bool
SegmentEntry::PrepareFlush(SegmentEntry* segment_entry) {
    DataSegmentStatus expected = DataSegmentStatus::kOpen;
    return segment_entry->status_.compare_exchange_strong(expected, DataSegmentStatus::kFlushing, std::memory_order_seq_cst);
}

UniquePtr<String>
SegmentEntry::Flush(SegmentEntry* segment_entry) {
    LOG_TRACE("DataSegment: {} is being flushed", segment_entry->segment_id_);
    for(SizeT column_id = 0; const auto& column_data: segment_entry->columns_) {
        column_data->Flush(column_data.get(), segment_entry->current_row_);
        LOG_TRACE("ColumnData: {} is flushed", column_id);
        ++ column_id;
    }

    DataSegmentStatus expected = DataSegmentStatus::kFlushing;
    if(!segment_entry->status_.compare_exchange_strong(expected, DataSegmentStatus::kClosed, std::memory_order_seq_cst)) {
        return MakeUnique<String>("Data segment is expected as flushing status");
    }
    LOG_TRACE("DataSegment: {} is being flushed", segment_entry->segment_id_);

    return nullptr;
}

nlohmann::json
SegmentEntry::Serialize(const SegmentEntry* segment_entry) {
    nlohmann::json json_res;

    json_res["base_dir"] = *segment_entry->base_dir_;
    json_res["row_capacity"] = segment_entry->row_capacity_;
    i64 status_value = segment_entry->status_;
    json_res["status"] = status_value;
    json_res["segment_id"] = segment_entry->segment_id_;
    for(const auto& column: segment_entry->columns_) {
        json_res["columns"].emplace_back(column->Serialize(column.get()));
    }
    json_res["start_txn_id"] = segment_entry->start_txn_id_;
    json_res["end_txn_id"] = segment_entry->end_txn_id_;

    json_res["begin_ts"] = segment_entry->begin_ts_;
    json_res["commit_ts"] = segment_entry->commit_ts_.load();
    json_res["txn_id"] = segment_entry->txn_id_.load();
    json_res["deleted"] = segment_entry->deleted_;
    json_res["current_row"] = segment_entry->current_row_;
    return json_res;
}

SharedPtr<SegmentEntry>
SegmentEntry::Deserialize(const nlohmann::json& table_entry_json, TableCollectionEntry* table_entry, BufferManager* buffer_mgr) {
    SharedPtr<SegmentEntry> segment_entry = MakeShared<SegmentEntry>(table_entry);

    segment_entry->base_dir_ = MakeShared<String>(table_entry_json["base_dir"]);
    segment_entry->row_capacity_ = table_entry_json["row_capacity"];

    i64 status_value = table_entry_json["status"];
    segment_entry->status_ = static_cast<DataSegmentStatus>(status_value);
    segment_entry->segment_id_ = table_entry_json["segment_id"];
    segment_entry->start_txn_id_ = table_entry_json["start_txn_id"];
    segment_entry->end_txn_id_ = table_entry_json["end_txn_id"];
    segment_entry->current_row_ = table_entry_json["current_row"];

    for(const auto& column_json: table_entry_json["columns"]) {
        SharedPtr<ColumnDataEntry> column_data_entry = ColumnDataEntry::Deserialize(column_json, segment_entry.get(), buffer_mgr);
        segment_entry->columns_.emplace_back(column_data_entry);
    }

    return segment_entry;
}

}

