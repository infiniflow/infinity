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

template <typename DataType>
void BlockData<DataType, BMCompressType::kCompressed>::Calculate(Vector<DataType> &upper_bounds, DataType query_score) const {
    // MultiF32StoreI32(block_ids_.data(), max_scores_.data(), upper_bounds.data(), query_score, block_ids_.size());
    for (SizeT i = 0; i < block_ids_.size(); ++i) {
        BMBlockID block_id = block_ids_[i];
        DataType score = max_scores_[i];
        upper_bounds[block_id] += score * query_score;
    }
}

template <typename DataType>
void BlockData<DataType, BMCompressType::kCompressed>::AddBlock(BMBlockID block_id, DataType max_score) {
    block_ids_.push_back(block_id);
    max_scores_.push_back(max_score);
}

template struct BlockData<f32, BMCompressType::kCompressed>;
template struct BlockData<f64, BMCompressType::kCompressed>;

template <typename DataType>
void BlockData<DataType, BMCompressType::kRaw>::Calculate(Vector<DataType> &upper_bounds, DataType query_score) const {
    for (BMBlockID block_id = 0; block_id < (BMBlockID)max_scores_.size(); ++block_id) {
        if (max_scores_[block_id] > 0.0) {
            upper_bounds[block_id] += max_scores_[block_id] * query_score;
        }
    }
}

template <typename DataType>
void BlockData<DataType, BMCompressType::kRaw>::AddBlock(BMBlockID block_id, DataType max_score) {
    if (block_id >= (BMBlockID)max_scores_.size()) {
        max_scores_.resize(block_id + 1, 0.0);
    }
    max_scores_[block_id] = max_score;
}

template struct BlockData<f32, BMCompressType::kRaw>;
template struct BlockData<f64, BMCompressType::kRaw>;

} // namespace infinity