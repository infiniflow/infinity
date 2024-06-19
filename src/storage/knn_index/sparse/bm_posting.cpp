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

module bm_posting;

import stl;
import bm_simd_func;
import infinity_exception;

namespace infinity {

void BlockData<BMCompressType::kCompressed>::Calculate(Vector<f32> &upper_bounds, f32 query_score) const {
    // MultiF32StoreI32(block_ids_.data(), max_scores_.data(), upper_bounds.data(), query_score, block_ids_.size());
    for (SizeT i = 0; i < block_ids_.size(); ++i) {
        i32 block_id = block_ids_[i];
        f32 score = max_scores_[i];
        upper_bounds[block_id] += score * query_score;
    }
}

void BlockData<BMCompressType::kCompressed>::AddBlock(i32 block_id, f32 max_score) {
    block_ids_.push_back(block_id);
    max_scores_.push_back(max_score);
}

void BlockData<BMCompressType::kRaw>::Calculate(Vector<f32> &upper_bounds, f32 query_score) const {
    for (i32 block_id = 0; block_id < (i32)max_scores_.size(); ++block_id) {
        if (max_scores_[block_id] > 0.0) {
            upper_bounds[block_id] += max_scores_[block_id] * query_score;
        }
    }
}

void BlockData<BMCompressType::kRaw>::AddBlock(i32 block_id, f32 max_score) {
    if (block_id >= (i32)max_scores_.size()) {
        max_scores_.resize(block_id + 1, 0.0);
    }
    max_scores_[block_id] = max_score;
}

} // namespace infinity