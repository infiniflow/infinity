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

import stl;
import txn;
import query_context;
import parser;
import physical_operator_type;
import operator_state;
import logger;

import infinity_exception;
import merge_knn_data;
import faiss;
import merge_knn;
import block_index;
import column_buffer;
import block_column_entry;
import third_party;
import block_entry;
import default_values;
import data_block;

module physical_merge_knn;

namespace infinity {

void PhysicalMergeKnn::Init() {}

bool PhysicalMergeKnn::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto merge_knn_op_state = static_cast<MergeKnnOperatorState *>(operator_state);
    if (merge_knn_op_state->input_complete_) {
        LOG_TRACE("PhysicalMergeKnn::Input is complete");
    }

    if(merge_knn_op_state->data_block_array_.empty()) {
        merge_knn_op_state->data_block_array_.emplace_back(DataBlock::MakeUniquePtr());
        merge_knn_op_state->data_block_array_[0]->Init(*GetOutputTypes());
    }

    auto &merge_knn_data = *merge_knn_op_state->merge_knn_function_data_;
    switch (merge_knn_data.elem_type_) {
        case kElemInvalid: {
            Error<ExecutorException>("Invalid elem type");
        }
        case kElemFloat: {
            switch (merge_knn_data.heap_type_) {
                case MergeKnnHeapType::kInvalid: {
                    Error<ExecutorException>("Invalid heap type");
                }
                case MergeKnnHeapType::kMaxHeap: {
                    ExecuteInner<f32, CompareMax>(query_context, merge_knn_op_state);
                    break;
                }
                case MergeKnnHeapType::kMinHeap: {
                    ExecuteInner<f32, CompareMin>(query_context, merge_knn_op_state);
                    break;
                }
            }
            break;
        }
        default: {
            Error<NotImplementException>("Not implemented");
        }
    }
    return true;
}

template <typename DataType, template <typename, typename> typename C>
void PhysicalMergeKnn::ExecuteInner(QueryContext *query_context, MergeKnnOperatorState *merge_knn_state) {
    auto &merge_knn_data = *merge_knn_state->merge_knn_function_data_;

    auto &input_data = *merge_knn_state->input_data_block_;
    Assert<ExecutorException>(input_data.Finalized(), "data should finalized here.");

    auto merge_knn = static_cast<MergeKnn<DataType, C> *>(merge_knn_data.merge_knn_base_.get());

    int column_n = input_data.column_count() - 2;
    Assert<ExecutorException>(column_n >= 0, "Error. The input data block is invalid");
    auto &dist_column = *input_data.column_vectors[column_n];
    auto &row_id_column = *input_data.column_vectors[column_n + 1];

    auto dists = reinterpret_cast<DataType *>(dist_column.data());
    auto row_ids = reinterpret_cast<RowID *>(row_id_column.data());
    SizeT row_n = input_data.row_count();
    merge_knn->Search(dists, row_ids, row_n);

    if(merge_knn_state->input_complete_) {
        merge_knn->End(); // reorder the heap

        BlockIndex *block_index = merge_knn_data.table_ref_->block_index_.get();

        u64 output_row_count{0};
        i64 result_n = Min(merge_knn_data.topk_, merge_knn->total_count());
        for (i64 query_idx = 0; query_idx < merge_knn_data.query_count_; ++query_idx) {
            DataType *result_dists = merge_knn->GetDistancesByIdx(query_idx);
            RowID *result_row_ids = merge_knn->GetIDsByIdx(query_idx);
            SizeT column_n = 0;
            for (i64 top_idx = 0; top_idx < result_n; ++top_idx) {
                u32 segment_id = result_row_ids[top_idx].segment_id_;
                u32 segment_offset = result_row_ids[top_idx].segment_offset_;
                u16 block_id = segment_offset / DEFAULT_BLOCK_CAPACITY;
                u16 block_offset = segment_offset % DEFAULT_BLOCK_CAPACITY;
                LOG_TRACE(Format("Row offset: {}: {}: {}, distance: {}", segment_id, block_id, block_offset, result_dists[top_idx]));

                BlockEntry *block_entry = block_index->GetBlockEntry(segment_id, block_id);
                if (block_entry == nullptr) {
                    Error<ExecutorException>(Format("Cannot find block segment id: {}, block id: {}", segment_id, block_id));
                }

                DataBlock* output_data_block = merge_knn_state->data_block_array_.back().get();
                if(output_row_count == DEFAULT_BLOCK_CAPACITY) {
                    output_data_block->Finalize();
                    merge_knn_state->data_block_array_.emplace_back(DataBlock::MakeUniquePtr());
                    merge_knn_state->data_block_array_.back()->Init(*GetOutputTypes());
                    output_data_block = merge_knn_state->data_block_array_.back().get();
                    output_row_count -= DEFAULT_BLOCK_CAPACITY;
                }

                column_n = table_ref_->column_ids_.size();
                for (SizeT i = 0; i < column_n; ++i) {
                    SizeT column_id = table_ref_->column_ids_[i];
                    ColumnBuffer column_buffer =
                        BlockColumnEntry::GetColumnData(block_entry->columns_[column_id].get(), query_context->storage()->buffer_manager());

                    const_ptr_t ptr = column_buffer.GetValueAt(block_offset, *output_types_->at(column_id));
                    output_data_block->AppendValueByPtr(column_id, ptr);
                }
                output_data_block->AppendValueByPtr(column_n, (ptr_t)&result_dists[top_idx]);
                output_data_block->AppendValueByPtr(column_n + 1, (ptr_t)&result_row_ids[top_idx]);
            }
            for (SizeT column_id = 0; column_id < column_n; ++column_id) {
                LOG_TRACE(Format("Output Column ID: {}, Name: {}", merge_knn_data.table_ref_->column_ids_[column_id], output_names_->at(column_id)));
            }
        }

        merge_knn_state->data_block_array_.back()->Finalize();
        merge_knn_state->SetComplete();
    }
}

} // namespace infinity
