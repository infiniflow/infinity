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
import serialize;

namespace infinity {

// --------------------------BlockPostings--------------------------

template <BMCompressType CompressType>
SizeT BlockPostings<CompressType>::GetSizeInBytes() const {
    return sizeof(kth_) + sizeof(kth_score_) + data_.GetSizeInBytes();
}

template <BMCompressType CompressType>
void BlockPostings<CompressType>::WriteAdv(char *&p) const {
    WriteBufAdv<i32>(p, kth_);
    WriteBufAdv<f32>(p, kth_score_);
    data_.WriteAdv(p);
}

template <BMCompressType CompressType>
BlockPostings<CompressType> BlockPostings<CompressType>::ReadAdv(char *&p) {
    BlockPostings res;
    res.kth_ = ReadBufAdv<i32>(p);
    res.kth_score_ = ReadBufAdv<f32>(p);
    res.data_ = BlockData<CompressType>::ReadAdv(p);
    return res;
}

template struct BlockPostings<BMCompressType::kCompressed>;
template struct BlockPostings<BMCompressType::kRaw>;

// --------------------------CompressedBlockData--------------------------

SizeT BlockData<BMCompressType::kCompressed>::GetSizeInBytes() const {
    return sizeof(SizeT) + block_ids_.size() * sizeof(i32) + max_scores_.size() * sizeof(f32);
}

void BlockData<BMCompressType::kCompressed>::WriteAdv(char *&p) const {
    SizeT max_score_size = max_scores_.size();
    WriteBufAdv<SizeT>(p, max_score_size);
    WriteBufCharsAdv(p, reinterpret_cast<const char *>(block_ids_.data()), sizeof(i32) * block_ids_.size());
    WriteBufCharsAdv(p, reinterpret_cast<const char *>(max_scores_.data()), sizeof(f32) * max_scores_.size());
}

BlockData<BMCompressType::kCompressed> BlockData<BMCompressType::kCompressed>::ReadAdv(char *&p) {
    BlockData<BMCompressType::kCompressed> res;
    SizeT max_score_size = ReadBufAdv<SizeT>(p);
    res.block_ids_.resize(max_score_size);
    res.max_scores_.resize(max_score_size);
    for (SizeT i = 0; i < max_score_size; ++i) {
        res.block_ids_[i] = ReadBufAdv<i32>(p);
    }
    for (SizeT i = 0; i < max_score_size; ++i) {
        res.max_scores_[i] = ReadBufAdv<f32>(p);
    }
    return res;
}

// // --------------------------RawBlockData--------------------------

SizeT BlockData<BMCompressType::kRaw>::GetSizeInBytes() const { return sizeof(SizeT) + max_scores_.size() * sizeof(f32); }

void BlockData<BMCompressType::kRaw>::WriteAdv(char *&p) const {
    SizeT max_score_size = max_scores_.size();
    WriteBufAdv<SizeT>(p, max_score_size);
    WriteBufCharsAdv(p, reinterpret_cast<const char *>(max_scores_.data()), sizeof(f32) * max_scores_.size());
}

BlockData<BMCompressType::kRaw> BlockData<BMCompressType::kRaw>::ReadAdv(char *&p) {
    BlockData<BMCompressType::kRaw> res;
    SizeT max_score_size = ReadBufAdv<SizeT>(p);
    res.max_scores_.resize(max_score_size);
    for (SizeT i = 0; i < max_score_size; ++i) {
        res.max_scores_[i] = ReadBufAdv<f32>(p);
    }
    return res;
}

} // namespace infinity