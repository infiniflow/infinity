//
// Created by jinhai on 23-9-22.
//

module;

#include <vector>

import stl;
import query_context;
import parser;
import logger;
import operator_state;
import global_block_id;
import block_index;
import base_table_ref;
import block_entry;
import knn_scan_data;
import column_buffer;
import block_column_entry;
import knn_distance;
import third_party;
import infinity_assert;
import infinity_exception;
import table_collection_entry;

module physical_knn_scan;

namespace infinity {

void PhysicalKnnScan::Init() {}

void PhysicalKnnScan::Execute(QueryContext *query_context) {}

void PhysicalKnnScan::Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) {
    auto *knn_scan_input_state = static_cast<KnnScanInputState *>(input_state);
    auto *knn_scan_output_state = static_cast<KnnScanOutputState *>(output_state);

    switch (knn_scan_input_state->knn_scan_function_data_->elem_type_) {
        case kElemFloat: {
            ExecuteInternal<f32>(query_context, knn_scan_input_state, knn_scan_output_state);
            break;
        }
        case kElemInvalid: {
            Error<ExecutorException>("Invalid element data type", __FILE_NAME__, __LINE__);
            break;
        }
        default: {
            Error<ExecutorException>("Not implemented", __FILE_NAME__, __LINE__);
        }
    }
}

TableCollectionEntry *PhysicalKnnScan::table_collection_ptr() const { return base_table_ref_->table_entry_ptr_; }

String PhysicalKnnScan::TableAlias() const { return base_table_ref_->alias_; }

BlockIndex *PhysicalKnnScan::GetBlockIndex() const { return base_table_ref_->block_index_.get(); }

Vector<SizeT> &PhysicalKnnScan::ColumnIDs() const { return base_table_ref_->column_ids_; }

Vector<SharedPtr<Vector<GlobalBlockID>>> PhysicalKnnScan::PlanBlockEntries(i64 parallel_count) const {
    BlockIndex *block_index = base_table_ref_->block_index_.get();

    u64 all_block_count = block_index->BlockCount();
    u64 block_per_task = all_block_count / parallel_count;
    u64 residual = all_block_count % parallel_count;

    Vector<SharedPtr<Vector<GlobalBlockID>>> result(parallel_count, nullptr);
    for (u64 task_id = 0, global_block_id = 0, residual_idx = 0; task_id < parallel_count; ++task_id) {
        result[task_id] = MakeShared<Vector<GlobalBlockID>>();
        for (u64 block_id_in_task = 0; block_id_in_task < block_per_task; ++block_id_in_task) {
            result[task_id]->emplace_back(block_index->global_blocks_[global_block_id++]);
        }
        if (residual_idx < residual) {
            result[task_id]->emplace_back(block_index->global_blocks_[global_block_id++]);
            ++residual_idx;
        }
    }
    return result;
}

SizeT PhysicalKnnScan::BlockEntryCount() const { return base_table_ref_->block_index_->BlockCount(); }

template <typename T>
void PhysicalKnnScan::ExecuteInternal(QueryContext *query_context, KnnScanInputState *input_state, KnnScanOutputState *output_state) {
    auto *knn_scan_function_data_ptr = input_state->knn_scan_function_data_.get();
    BlockIndex *block_index = knn_scan_function_data_ptr->block_index_;
    Vector<GlobalBlockID> *block_ids = knn_scan_function_data_ptr->global_block_ids_.get();
    const Vector<SizeT> &knn_column_ids = knn_scan_function_data_ptr->knn_column_ids_;
    if (knn_column_ids.size() != 1) {
        Error<ExecutorException>("More than one knn column", __FILE_NAME__, __LINE__);
    }

    SizeT knn_column_id = knn_column_ids[0];

    i64 &block_ids_idx = knn_scan_function_data_ptr->current_block_ids_idx_;
    i32 segment_id = block_ids->at(block_ids_idx).segment_id_;
    i16 block_id = block_ids->at(block_ids_idx).block_id_;

    BlockEntry *current_block_entry = block_index->GetBlockEntry(segment_id, block_id);
    i16 row_count = current_block_entry->row_count_;

    Vector<ColumnBuffer> columns_buffer;
    columns_buffer.reserve(current_block_entry->columns_.size());
    SizeT column_count = current_block_entry->columns_.size();
    for (SizeT column_id = 0; column_id < column_count; ++column_id) {
        columns_buffer.emplace_back(
            BlockColumnEntry::GetColumnData(current_block_entry->columns_[column_id].get(), query_context->storage()->buffer_manager()));
    }

    auto knn_flat = static_cast<KnnDistance<T> *>(knn_scan_function_data_ptr->knn_distance_.get());
    knn_flat->Search((T *)(columns_buffer[knn_column_id].GetAll()), row_count, segment_id, block_id);

    block_ids_idx++;
    if (block_ids_idx == block_ids->size()) {
        // Last block, Get the result according to the topk row.
        knn_flat->End();

        for (i64 query_idx = 0; query_idx < knn_flat->QueryCount(); ++query_idx) {

            T *top_distance = knn_flat->GetDistanceByIdx(query_idx);
            RowID *row_id = knn_flat->GetIDByIdx(query_idx);

            i64 result_count = Min(knn_flat->TotalBaseCount(), knn_flat->TopK());

            for (i64 top_idx = 0; top_idx < result_count; ++top_idx) {
                // Bug here? id = top_idx?
                SizeT id = query_idx * knn_flat->QueryCount() + top_idx;
                LOG_TRACE(Format("Row offset: {}: {}: {}, distance {}",
                                 row_id[id].segment_id_,
                                 row_id[id].block_id_,
                                 row_id[id].block_offset_,
                                 top_distance[id]));

                BlockEntry *block_entry = block_index->GetBlockEntry(row_id[id].segment_id_, row_id[id].block_id_);
                if (block_entry == nullptr) {
                    Error<ExecutorException>(Format("Cannot find block segment id: {}, block id: {}", row_id[id].segment_id_, row_id[id].block_id_),
                                             __FILE_NAME__,
                                             __LINE__);
                }

                SizeT column_id = 0;
                for (; column_id < column_count; ++column_id) {
                    ColumnBuffer column_buffer =
                        BlockColumnEntry::GetColumnData(block_entry->columns_[column_id].get(), query_context->storage()->buffer_manager());

                    const_ptr_t ptr = column_buffer.GetValueAt(row_id[id].block_offset_, *output_types_->at(column_id));
                    output_state->data_block_->AppendValueByPtr(column_id, ptr);
                }

                output_state->data_block_->AppendValueByPtr(column_id++, (ptr_t)&top_distance[id]);
                output_state->data_block_->AppendValueByPtr(column_id, (ptr_t)&row_id[id]);
            }

            for (SizeT column_id = 0; column_id < column_count; ++column_id) {
                LOG_TRACE(Format("Output Column ID: {}, Name: {}", base_table_ref_->column_ids_[column_id], output_names_->at(column_id)));
            }
        }

        // Last segment, Get the result according to the topk row.
        output_state->SetComplete();
    }

    output_state->data_block_->Finalize();
}

} // namespace infinity
