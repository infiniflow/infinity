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

module physical_scan_base;

import stl;
import physical_operator;
import physical_operator_type;
import global_block_id;
import base_table_ref;
import table_entry;
import block_index;
import data_block;
import operator_state;
import default_values;
import third_party;
import infinity_exception;
import block_entry;
import block_column_entry;
import logger;
import column_vector;
import query_context;

namespace infinity {

Vector<SharedPtr<Vector<GlobalBlockID>>> PhysicalScanBase::PlanBlockEntries(i64 parallel_count) const {
    BlockIndex *block_index = base_table_ref_->block_index_.get();

    u64 all_block_count = block_index->BlockCount();
    u64 block_per_task = all_block_count / parallel_count;
    u64 residual = all_block_count % parallel_count;

    Vector<GlobalBlockID> global_blocks;
    global_blocks.reserve(all_block_count);
    for (const auto &[segment_id, segment_info] : block_index->segment_block_index_) {
        for (const auto *block_entry : segment_info.block_map_) {
            global_blocks.emplace_back(segment_id, block_entry->block_id());
        }
    }
    Vector<SharedPtr<Vector<GlobalBlockID>>> result(parallel_count, nullptr);
    for (SizeT task_id = 0, global_block_id = 0, residual_idx = 0; (i64)task_id < parallel_count; ++task_id) {
        result[task_id] = MakeShared<Vector<GlobalBlockID>>();
        auto& task_result = result[task_id];
        task_result->reserve(block_per_task);

        for (u64 block_id_in_task = 0; block_id_in_task < block_per_task; ++block_id_in_task) {
            task_result->push_back(global_blocks[global_block_id++]);
        }
        if (residual_idx < residual) {
            task_result->push_back(global_blocks[global_block_id++]);
            ++residual_idx;
        }
    }
    return result;
}

SizeT PhysicalScanBase::TaskletCount() { return base_table_ref_->block_index_->BlockCount(); }

BlockIndex *PhysicalScanBase::GetBlockIndex() const { return base_table_ref_->block_index_.get(); }

void PhysicalScanBase::SetOutput(const Vector<char *> &raw_result_dists_list,
                                 const Vector<RowID *> &row_ids_list,
                                 SizeT result_size,
                                 i64 result_n,
                                 QueryContext *query_context,
                                 OperatorState *operator_state) {
    BlockIndex *block_index = base_table_ref_->block_index_.get();
    SizeT query_n = raw_result_dists_list.size();

    {
        SizeT total_data_row_count = query_n * result_n;
        SizeT row_idx = 0;
        do {
            auto data_block = DataBlock::MakeUniquePtr();
            data_block->Init(*GetOutputTypes());
            operator_state->data_block_array_.emplace_back(std::move(data_block));
            row_idx += DEFAULT_BLOCK_CAPACITY;
        } while (row_idx < total_data_row_count);
    }
    auto *buffer_mgr = query_context->storage()->buffer_manager();

    SizeT output_block_row_id = 0;
    SizeT output_block_idx = 0;
    DataBlock *output_block_ptr = operator_state->data_block_array_[output_block_idx].get();
    for (SizeT query_idx = 0; query_idx < query_n; ++query_idx) {
        char *raw_result_dists = raw_result_dists_list[query_idx];
        RowID *row_ids = row_ids_list[query_idx];
        for (i64 top_idx = 0; top_idx < result_n; ++top_idx) {
            SizeT id = query_n * query_idx + top_idx;

            SegmentID segment_id = row_ids[top_idx].segment_id_;
            SegmentOffset segment_offset = row_ids[top_idx].segment_offset_;
            BlockID block_id = segment_offset / DEFAULT_BLOCK_CAPACITY;
            BlockOffset block_offset = segment_offset % DEFAULT_BLOCK_CAPACITY;

            BlockEntry *block_entry = block_index->GetBlockEntry(segment_id, block_id);
            if (block_entry == nullptr) {
                String error_message = fmt::format("Cannot find segment id: {}, block id: {}", segment_id, block_id);
                UnrecoverableError(error_message);
            }

            if (output_block_row_id == DEFAULT_BLOCK_CAPACITY) {
                output_block_ptr->Finalize();
                ++output_block_idx;
                output_block_ptr = operator_state->data_block_array_[output_block_idx].get();
                output_block_row_id = 0;
            }

            SizeT column_n = base_table_ref_->column_ids_.size();
            for (SizeT i = 0; i < column_n; ++i) {
                SizeT column_id = base_table_ref_->column_ids_[i];
                auto *block_column_entry = block_entry->GetColumnBlockEntry(column_id);
                ColumnVector &&column_vector = block_column_entry->GetConstColumnVector(buffer_mgr);

                output_block_ptr->column_vectors[i]->AppendWith(column_vector, block_offset, 1);
            }
            output_block_ptr->AppendValueByPtr(column_n, raw_result_dists + id * result_size);
            output_block_ptr->AppendValueByPtr(column_n + 1, (ptr_t)&row_ids[id]);

            ++output_block_row_id;
        }
    }
    output_block_ptr->Finalize();
}

} // namespace infinity
