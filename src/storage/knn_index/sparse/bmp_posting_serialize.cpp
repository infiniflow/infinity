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

// --------------------------CompressedBlockData--------------------------

template <typename DataType>
SizeT BlockData<DataType, BMPCompressType::kCompressed>::GetSizeInBytes() const {
    return sizeof(SizeT) + block_ids_.size() * sizeof(BMPBlockID) + max_scores_.size() * sizeof(DataType);
}

template <typename DataType>
void BlockData<DataType, BMPCompressType::kCompressed>::WriteAdv(char *&p) const {
    SizeT max_score_size = max_scores_.size();
    WriteBufAdv<SizeT>(p, max_score_size);
    WriteBufCharsAdv(p, reinterpret_cast<const char *>(block_ids_.data()), sizeof(BMPBlockID) * block_ids_.size());
    WriteBufCharsAdv(p, reinterpret_cast<const char *>(max_scores_.data()), sizeof(DataType) * max_scores_.size());
}

template <typename DataType>
BlockData<DataType, BMPCompressType::kCompressed> BlockData<DataType, BMPCompressType::kCompressed>::ReadAdv(char *&p) {
    BlockData<DataType, BMPCompressType::kCompressed> res;
    SizeT max_score_size = ReadBufAdv<SizeT>(p);
    res.block_ids_.resize(max_score_size);
    res.max_scores_.resize(max_score_size);
    for (SizeT i = 0; i < max_score_size; ++i) {
        res.block_ids_[i] = ReadBufAdv<BMPBlockID>(p);
    }
    for (SizeT i = 0; i < max_score_size; ++i) {
        res.max_scores_[i] = ReadBufAdv<DataType>(p);
    }
    return res;
}

template struct BlockData<f32, BMPCompressType::kCompressed>;
template struct BlockData<f64, BMPCompressType::kCompressed>;

// --------------------------RawBlockData--------------------------

template <typename DataType>
SizeT BlockData<DataType, BMPCompressType::kRaw>::GetSizeInBytes() const {
    return sizeof(SizeT) + max_scores_.size() * sizeof(DataType);
}

template <typename DataType>
void BlockData<DataType, BMPCompressType::kRaw>::WriteAdv(char *&p) const {
    SizeT max_score_size = max_scores_.size();
    WriteBufAdv<SizeT>(p, max_score_size);
    WriteBufCharsAdv(p, reinterpret_cast<const char *>(max_scores_.data()), sizeof(DataType) * max_scores_.size());
}

template <typename DataType>
BlockData<DataType, BMPCompressType::kRaw> BlockData<DataType, BMPCompressType::kRaw>::ReadAdv(char *&p) {
    BlockData<DataType, BMPCompressType::kRaw> res;
    SizeT max_score_size = ReadBufAdv<SizeT>(p);
    res.max_scores_.resize(max_score_size);
    for (SizeT i = 0; i < max_score_size; ++i) {
        res.max_scores_[i] = ReadBufAdv<DataType>(p);
    }
    return res;
}

template struct BlockData<f32, BMPCompressType::kRaw>;
template struct BlockData<f64, BMPCompressType::kRaw>;

// --------------------------BlockPostings--------------------------

template <typename DataType, BMPCompressType CompressType>
SizeT BlockPostings<DataType, CompressType>::GetSizeInBytes() const {
    return sizeof(kth_) + sizeof(kth_score_) + data_.GetSizeInBytes();
}

template <typename DataType, BMPCompressType CompressType>
void BlockPostings<DataType, CompressType>::WriteAdv(char *&p) const {
    WriteBufAdv<i32>(p, kth_);
    WriteBufAdv<DataType>(p, kth_score_);
    data_.WriteAdv(p);
}

template <typename DataType, BMPCompressType CompressType>
BlockPostings<DataType, CompressType> BlockPostings<DataType, CompressType>::ReadAdv(char *&p) {
    BlockPostings res;
    res.kth_ = ReadBufAdv<i32>(p);
    res.kth_score_ = ReadBufAdv<DataType>(p);
    res.data_ = BlockData<DataType, CompressType>::ReadAdv(p);
    return res;
}

template struct BlockPostings<f32, BMPCompressType::kCompressed>;
template struct BlockPostings<f32, BMPCompressType::kRaw>;
template struct BlockPostings<f64, BMPCompressType::kCompressed>;
template struct BlockPostings<f64, BMPCompressType::kRaw>;

} // namespace infinity