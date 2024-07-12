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

module physical_merge_match_tensor;

import stl;
import txn;
import query_context;
import physical_operator_type;
import operator_state;
import logger;
import status;
import infinity_exception;
import buffer_manager;
import third_party;
import default_values;
import data_block;
import value;
import column_vector;

namespace infinity {

struct VectorBlockRawIndex {
    u32 left_row_cnt_ = 0;
    u32 block_id_ = 0;
    u32 block_offset_ = 0;
    VectorBlockRawIndex() = default;
    VectorBlockRawIndex(u32 left_row_cnt, u32 block_id, u32 block_offset)
        : left_row_cnt_(left_row_cnt), block_id_(block_id), block_offset_(block_offset) {}
    VectorBlockRawIndex &operator++() {
        if (left_row_cnt_ > 0) {
            --left_row_cnt_;
            if (++block_offset_ == DEFAULT_BLOCK_CAPACITY) {
                ++block_id_;
                block_offset_ = 0;
            }
        }
        return *this;
    }
    explicit operator bool() const { return left_row_cnt_ > 0; }
};

PhysicalMergeMatchTensor::PhysicalMergeMatchTensor(const u64 id,
                                                   UniquePtr<PhysicalOperator> left,
                                                   const u64 table_index,
                                                   SharedPtr<BaseTableRef> base_table_ref,
                                                   SharedPtr<MatchTensorExpression> match_tensor_expr,
                                                   const u32 topn,
                                                   SharedPtr<Vector<LoadMeta>> load_metas)
    : PhysicalOperator(PhysicalOperatorType::kMergeMatchTensor, std::move(left), nullptr, id, load_metas), table_index_(table_index),
      base_table_ref_(std::move(base_table_ref)), match_tensor_expr_(std::move(match_tensor_expr)), topn_(topn) {}

void PhysicalMergeMatchTensor::Init() { left()->Init(); }

SizeT PhysicalMergeMatchTensor::TaskletCount() {
    String error_message = "Not Expected: TaskletCount of PhysicalMergeMatchTensor?";
    UnrecoverableError(error_message);
    return 0;
}

bool PhysicalMergeMatchTensor::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto *merge_match_tensor_op_state = static_cast<MergeMatchTensorOperatorState *>(operator_state);
    if (merge_match_tensor_op_state->input_complete_) {
        LOG_TRACE("PhysicalMergeMatchTensor::Input is complete");
    }
    ExecuteInner(query_context, merge_match_tensor_op_state);
    return true;
}

void PhysicalMergeMatchTensor::ExecuteInner(QueryContext *query_context, MergeMatchTensorOperatorState *operator_state) const {
    auto &output_data_block_array = operator_state->data_block_array_;
    if (!output_data_block_array.empty()) {
        String error_message = "output data_block_array_ is not empty";
        UnrecoverableError(error_message);
    }
    auto &input_data_block_array = operator_state->input_data_blocks_;
    if (input_data_block_array.empty()) {

        String error_message = "PhysicalMergeMatchTensor: empty input";
        UnrecoverableError(error_message);
        return;
    }
    const auto output_type_ptr = GetOutputTypes();
    const u32 score_column_idx = output_type_ptr->size() - 2;
    auto &middle_data_block_array = operator_state->middle_sorted_data_blocks_;
    auto &middle_result_count = operator_state->middle_result_count_;
    u32 input_result_count =
        std::accumulate(input_data_block_array.begin(), input_data_block_array.end(), 0, [](u32 sum, const auto &data_block) -> u32 {
            return sum + data_block->row_count();
        });
    if (middle_data_block_array.empty()) {
        // first input
        middle_data_block_array = std::move(input_data_block_array);
        middle_result_count = input_result_count;
    } else {
        // merge sort by score, and keep topn
        const u32 topn = topn_;
        const u32 new_result_cnt = std::min(topn, input_result_count + middle_result_count);
        const u32 new_result_block_cnt = (new_result_cnt + DEFAULT_BLOCK_CAPACITY - 1) / DEFAULT_BLOCK_CAPACITY;
        const u32 middle_block_cnt = middle_data_block_array.size();
        // assume that middle and input are both valid
        auto choose_middle = [&](const VectorBlockRawIndex &middle, const VectorBlockRawIndex &input) -> bool {
            if (!middle) {
                return false;
            } else if (!input) {
                return true;
            } else {
                const float *middle_val_ptr =
                    reinterpret_cast<const float *>(middle_data_block_array[middle.block_id_]->column_vectors[score_column_idx]->data());
                const float *input_val_ptr = reinterpret_cast<const float *>(
                    input_data_block_array[input.block_id_ - middle_block_cnt]->column_vectors[score_column_idx]->data());
                return middle_val_ptr[middle.block_offset_] >= input_val_ptr[input.block_offset_];
            }
        };
        // 1. get merged topn ids
        auto new_result_ids = MakeUniqueForOverwrite<VectorBlockRawIndex[]>(new_result_cnt);
        {
            VectorBlockRawIndex middle_id(middle_result_count, 0, 0), input_id(input_result_count, middle_block_cnt, 0);
            for (u32 total_i = 0; total_i < new_result_cnt; ++total_i) {
                if (choose_middle(middle_id, input_id)) {
                    new_result_ids[total_i] = middle_id;
                    ++middle_id;
                } else {
                    new_result_ids[total_i] = input_id;
                    ++input_id;
                }
            }
        }
        // 2. update middle_data_block_array
        Vector<UniquePtr<DataBlock>> new_middle_data_block_array;
        new_middle_data_block_array.reserve(new_result_block_cnt);
        auto get_block_ptr = [&](const u32 block_id) -> DataBlock * {
            if (block_id < middle_block_cnt) {
                return middle_data_block_array[block_id].get();
            } else {
                return input_data_block_array[block_id - middle_block_cnt].get();
            }
        };
        for (u32 block_id = 0; block_id < new_result_block_cnt; ++block_id) {
            auto block_to_append = DataBlock::MakeUniquePtr();
            block_to_append->Init(*output_type_ptr);
            const u32 start_id = block_id * DEFAULT_BLOCK_CAPACITY;
            const u32 end_id = std::min(new_result_cnt, start_id + u32(DEFAULT_BLOCK_CAPACITY));
            for (u32 id = start_id; id < end_id; ++id) {
                auto [_, block_idx, block_offset] = new_result_ids[id];
                block_to_append->AppendWith(get_block_ptr(block_idx), block_offset, 1);
            }
            block_to_append->Finalize();
            new_middle_data_block_array.push_back(std::move(block_to_append));
        }
        middle_data_block_array = std::move(new_middle_data_block_array);
        middle_result_count = new_result_cnt;
    }
    input_data_block_array.clear();
    if (operator_state->input_complete_) {
        output_data_block_array = std::move(middle_data_block_array);
        if (output_data_block_array.empty()) {
            // provide an empty data block
            auto data_block = DataBlock::MakeUniquePtr();
            data_block->Init(*output_type_ptr);
            data_block->Finalize();
            output_data_block_array.push_back(std::move(data_block));
        }
        operator_state->SetComplete();
    }
}

} // namespace infinity
