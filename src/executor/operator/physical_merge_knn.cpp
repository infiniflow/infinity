//
// Created by jinhai on 23-10-1.
//

#include "physical_merge_knn.h"
#include "common/column_vector/column_vector.h"
#include "common/types/alias/primitives.h"
#include "common/utility/infinity_assert.h"
#include "executor/operator_state.h"
#include "main/query_context.h"
#include "storage/buffer/column_buffer.h"
#include "storage/common/block_index.h"
#include "storage/data_block.h"
#include "storage/knnindex/merge_knn.h"
#include "storage/meta/entry/block_entry.h"
#include "storage/storage.h"

namespace infinity {

void PhysicalMergeKnn::Init() {}

void PhysicalMergeKnn::Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) {
    if (input_state->Complete()) {
        LOG_TRACE("PhysicalMergeKnn::Execute complete")
    }
    auto merge_knn_input = static_cast<MergeKnnInputState *>(input_state);
    auto merge_knn_output = static_cast<MergeKnnOutputState *>(output_state);

    switch (merge_knn_input->merge_knn_function_data_->elem_type_) {
        case kElemInvalid: {
            ExecutorError("Invalid elem type");
        }
        case kElemFloat: {
            ExecuteInner<f32>(query_context, merge_knn_input, merge_knn_output);
            break;
        }
        default: {
            NotImplementError("Not implemented");
        }
    }
}

void PhysicalMergeKnn::Execute(QueryContext *query_context) { NotImplementError("Deprecated function"); }

template <typename T>
void PhysicalMergeKnn::ExecuteInner(QueryContext *query_context, MergeKnnInputState *input_state, MergeKnnOutputState *output_state) {
    auto &merge_knn_data = *input_state->merge_knn_function_data_;
    if (merge_knn_data.current_parallel_idx_ == merge_knn_data.total_parallel_n_) {
        return;
    }

    auto &input_data = *input_state->input_data_block_;

    auto merge_knn = static_cast<MergeKnn<T> *>(merge_knn_data.merge_knn_base_.get());

    int column_n = input_data.column_count() - 2;
    ExecutorAssert(column_n >= 0, "Error. The input data block is invalid");
    auto &dist_column = *input_data.column_vectors[column_n];
    auto &row_id_column = *input_data.column_vectors[column_n + 1];

    auto dists = reinterpret_cast<T *>(dist_column.data());
    auto row_ids = reinterpret_cast<RowID *>(row_id_column.data());
    SizeT row_n = input_data.row_count();
    merge_knn->Search(dists, row_ids, row_n);

    merge_knn_data.current_parallel_idx_++;
    if (merge_knn_data.current_parallel_idx_ == merge_knn_data.total_parallel_n_) {
        BlockIndex *block_index = merge_knn_data.table_ref_->block_index_.get();
        auto &output_data = *output_state->data_block_;

        i64 result_n = std::min(merge_knn_data.topk_, merge_knn->total_count());
        for (i64 query_idx = 0; query_idx < merge_knn_data.query_count_; query_idx++) {
            T *result_dists = merge_knn->GetDistancesByIdx(query_idx);
            RowID *result_row_ids = merge_knn->GetIDsByIdx(query_idx);
            for (i64 top_idx = result_n - 1; top_idx >= 0; top_idx--) {
                i32 segment_id = result_row_ids[top_idx].segment_id_;
                i16 block_id = result_row_ids[top_idx].block_id_;
                i16 block_offset = result_row_ids[top_idx].block_offset_;
                LOG_TRACE("Row offset: {}: {}: {}, distance: {}", segment_id, block_id, block_offset, result_dists[top_idx]);

                BlockEntry *block_entry = block_index->GetBlockEntry(segment_id, block_id);
                if (block_entry == nullptr) {
                    ExecutorError(fmt::format("Cannot find block segment id: {}, block id: {}", segment_id, block_id));
                }

                SizeT column_id = 0;
                for (; column_id < column_n; column_id++) {
                    ColumnBuffer column_buffer =
                        BlockColumnEntry::GetColumnData(block_entry->columns_[column_id].get(), query_context->storage()->buffer_manager());

                    ptr_t ptr = column_buffer.GetValueAt(block_offset, *output_types_->at(column_id));
                    output_data.AppendValueByPtr(column_id, ptr);
                }
                output_data.AppendValueByPtr(column_id++, (ptr_t)&result_dists[top_idx]);
                output_data.AppendValueByPtr(column_id, (ptr_t)&result_row_ids[top_idx]);
            }
            for (SizeT column_id = 0; column_id < column_n; ++column_id) {
                LOG_TRACE("Output Column ID: {}, Name: {}", merge_knn_data.table_ref_->column_ids_[column_id], output_names_->at(column_id));
            }
        }
        output_data.Finalize();

        merge_knn->End();
        output_state->SetComplete();
    }
}

} // namespace infinity
