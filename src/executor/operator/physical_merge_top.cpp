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

#include <memory>
#include <numeric>

module physical_merge_top;

import stl;

import query_context;
import operator_state;
import base_expression;
import value_expression;
import expression_state;
import infinity_exception;
import expression_evaluator;
import data_block;
import column_vector;
import default_values;
import physical_top;
import logger;

namespace infinity {

struct VectorBlockRawIndex {
    u32 left_row_cnt_{};
    u32 block_id_{};
    u32 block_offset_{};
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

void PhysicalMergeTop::Init() {
    left()->Init();
    // Initialize sort parameters
    sort_expr_count_ = order_by_types_.size();
    if (sort_expr_count_ != sort_expressions_.size()) {
        String error_message = "order_by_types_.size() != sort_expressions_.size()";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    // copy compare function from PhysicalTop
    prefer_left_function_ = (reinterpret_cast<PhysicalTop *>(left()))->GetInnerCompareFunction();
}

bool PhysicalMergeTop::Execute(QueryContext *, OperatorState *operator_state) {
    auto &output_data_block_array = operator_state->data_block_array_;
    if (!output_data_block_array.empty()) {
        String error_message = "output data_block_array_ is not empty";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    auto merge_top_op_state = static_cast<MergeTopOperatorState *>(operator_state);
    auto &input_data_block_array = merge_top_op_state->input_data_blocks_;
    if (input_data_block_array.empty()) {
        String error_message = "MergeTop: empty input";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
        return false;
    }
    auto &middle_data_block_array = merge_top_op_state->middle_sorted_data_blocks_;
    auto &middle_result_count = merge_top_op_state->middle_result_count_;
    u32 input_result_count =
        std::accumulate(input_data_block_array.begin(), input_data_block_array.end(), 0, [](u32 sum, const auto &data_block) -> u32 {
            return sum + data_block->row_count();
        });
    if (middle_data_block_array.empty()) {
        // first input
        middle_data_block_array = std::move(input_data_block_array);
        middle_result_count = input_result_count;
    } else {
        // merge in-place can be done by:
        // 1. merge 2 sorted array by VectorBlockRawIndex
        // 2. update middle_data_block_array from the end to the beginning, may avoid allocating extra temp data block
        if (input_result_count > middle_result_count) {
            std::swap(input_data_block_array, middle_data_block_array);
            std::swap(input_result_count, middle_result_count);
        }
        auto &expr_states = merge_top_op_state->expr_states_;
        auto eval_columns_middle = PhysicalTop::GetEvalColumns(sort_expressions_, expr_states, middle_data_block_array);
        auto eval_columns_input = PhysicalTop::GetEvalColumns(sort_expressions_, expr_states, input_data_block_array);
        u32 result_cnt = std::min(limit_, input_result_count + middle_result_count);
        u32 result_block_cnt = result_cnt / DEFAULT_BLOCK_CAPACITY + ((result_cnt % DEFAULT_BLOCK_CAPACITY) != 0);
        u32 middle_block_cnt = middle_data_block_array.size();
        // assume that middle and input are both valid
        auto choose_middle = [&](const VectorBlockRawIndex &middle, const VectorBlockRawIndex &input) -> bool {
            if (!middle) {
                return false;
            } else if (!input) {
                return true;
            } else {
                auto &middle_data = eval_columns_middle[middle.block_id_];
                auto &input_data = eval_columns_input[input.block_id_ - middle_block_cnt];
                return prefer_left_function_.Compare(middle_data, middle.block_offset_, input_data, input.block_offset_);
            }
        };
        // 1. get merged top ids
        auto result_ids = MakeUniqueForOverwrite<VectorBlockRawIndex[]>(result_cnt);
        {
            VectorBlockRawIndex middle_id(middle_result_count, 0, 0), input_id(input_result_count, middle_block_cnt, 0);
            for (u32 total_i = 0; total_i < result_cnt; ++total_i) {
                if (choose_middle(middle_id, input_id)) {
                    result_ids[total_i] = middle_id;
                    ++middle_id;
                } else {
                    result_ids[total_i] = input_id;
                    ++input_id;
                }
            }
        }
        // 2. update middle_data_block_array
        middle_data_block_array.resize(result_block_cnt);
        auto get_block_ptr = [&](u32 block_id) -> DataBlock * {
            if (block_id < middle_block_cnt) {
                return middle_data_block_array[block_id].get();
            } else {
                return input_data_block_array[block_id - middle_block_cnt].get();
            }
        };
        // 2.1. append new blocks
        for (u32 block_id = middle_block_cnt; block_id < result_block_cnt; ++block_id) {
            auto block_to_append = DataBlock::MakeUniquePtr();
            block_to_append->Init(*GetOutputTypes(), DEFAULT_BLOCK_CAPACITY);
            u32 start_id = block_id * DEFAULT_BLOCK_CAPACITY;
            u32 end_id = std::min(result_cnt, start_id + u32(DEFAULT_BLOCK_CAPACITY));
            for (u32 id = start_id; id < end_id; ++id) {
                auto [_, block_idx, block_offset] = result_ids[id];
                block_to_append->AppendWith(get_block_ptr(block_idx), block_offset, 1);
            }
            block_to_append->Finalize();
            middle_data_block_array[block_id] = std::move(block_to_append);
        }
        // 2.2. reuse old blocks (from the end to the beginning)
        // maybe a simple way: create a new data block for "swap"
        auto swap_block = DataBlock::MakeUniquePtr();
        swap_block->Init(*GetOutputTypes(), DEFAULT_BLOCK_CAPACITY);
        for (i32 block_id = middle_block_cnt - 1; block_id >= 0; --block_id) {
            u32 start_id = block_id * DEFAULT_BLOCK_CAPACITY;
            u32 end_id = std::min(result_cnt, start_id + u32(DEFAULT_BLOCK_CAPACITY)); // full
            swap_block->Reset();
            for (u32 id = start_id; id < end_id; ++id) {
                auto [_, block_idx, block_offset] = result_ids[id];
                swap_block->AppendWith(get_block_ptr(block_idx), block_offset, 1);
            }
            swap_block->Finalize();
            std::swap(middle_data_block_array[block_id], swap_block);
        }
        middle_result_count = result_cnt;
    }
    input_data_block_array.clear();
    if (merge_top_op_state->input_complete_) {
        output_data_block_array = std::move(middle_data_block_array);
        PhysicalTop::HandleOutputOffset(middle_result_count, offset_, output_data_block_array);
        operator_state->SetComplete();
        return true;
    } else {
        return false;
    }
}

} // namespace infinity
