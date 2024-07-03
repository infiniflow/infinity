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

#include <xmmintrin.h>

module bm_posting;

import stl;
import bmp_simd_func;
import infinity_exception;

namespace infinity {

template <typename DataType>
void BlockData<DataType, BMPCompressType::kCompressed>::Calculate(Vector<DataType> &upper_bounds, DataType query_score) const {
    SizeT block_size = block_ids_.size();
    for (SizeT i = 0; i < block_size; ++i) {
        BMPBlockID block_id = block_ids_[i];
        DataType score = max_scores_[i];
        upper_bounds[block_id] += score * query_score;
    }
}

template <typename DataType>
void BlockData<DataType, BMPCompressType::kCompressed>::AddBlock(BMPBlockID block_id, DataType max_score) {
    block_ids_.push_back(block_id);
    max_scores_.push_back(max_score);
}

template <typename DataType>
void BlockData<DataType, BMPCompressType::kCompressed>::Prefetch() const {
    _mm_prefetch(block_ids_.data(), _MM_HINT_T0);
    _mm_prefetch(max_scores_.data(), _MM_HINT_T0);
}

template struct BlockData<f32, BMPCompressType::kCompressed>;
template struct BlockData<f64, BMPCompressType::kCompressed>;

template <typename DataType>
void BlockData<DataType, BMPCompressType::kRaw>::Calculate(Vector<DataType> &upper_bounds, DataType query_score) const {
    for (BMPBlockID block_id = 0; block_id < (BMPBlockID)max_scores_.size(); ++block_id) {
        if (max_scores_[block_id] > 0.0) {
            upper_bounds[block_id] += max_scores_[block_id] * query_score;
        }
    }
}

template <typename DataType>
void BlockData<DataType, BMPCompressType::kRaw>::AddBlock(BMPBlockID block_id, DataType max_score) {
    if (block_id >= (BMPBlockID)max_scores_.size()) {
        max_scores_.resize(block_id + 1, 0.0);
    }
    max_scores_[block_id] = max_score;
}

template <typename DataType>
void BlockData<DataType, BMPCompressType::kRaw>::Prefetch() const {
    _mm_prefetch(max_scores_.data(), _MM_HINT_T0);
}

template struct BlockData<f32, BMPCompressType::kRaw>;
template struct BlockData<f64, BMPCompressType::kRaw>;

} // namespace infinity