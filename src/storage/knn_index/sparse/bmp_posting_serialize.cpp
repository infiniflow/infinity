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

module bm_posting;

import stl;
import serialize;

namespace infinity {

// --------------------------CompressedBlockData--------------------------

template <typename DataType>
SizeT BlockData<DataType, BMPCompressType::kCompressed, BMPOwnMem::kTrue>::GetSizeInBytes() const {
    return sizeof(SizeT) + block_ids_.size() * sizeof(BMPBlockID) + max_scores_.size() * sizeof(DataType);
}

template <typename DataType>
void BlockData<DataType, BMPCompressType::kCompressed, BMPOwnMem::kTrue>::WriteAdv(char *&p) const {
    SizeT max_score_size = max_scores_.size();
    WriteBufAdv<SizeT>(p, max_score_size);
    WriteBufVecAdv(p, block_ids_.data(), block_ids_.size());
    WriteBufVecAdv(p, max_scores_.data(), max_scores_.size());
}

template <typename DataType>
BlockData<DataType, BMPCompressType::kCompressed, BMPOwnMem::kTrue>
BlockData<DataType, BMPCompressType::kCompressed, BMPOwnMem::kTrue>::ReadAdv(const char *&p) {
    BlockData<DataType, BMPCompressType::kCompressed, BMPOwnMem::kTrue> res;
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

template <typename DataType>
void BlockData<DataType, BMPCompressType::kCompressed, BMPOwnMem::kTrue>::GetSizeToPtr(
    const char *&p,
    const Vector<const BlockData<DataType, BMPCompressType::kCompressed, BMPOwnMem::kTrue> *> &block_data_list) {
    GetSizeInBytesVecAligned<SizeT>(p, block_data_list.size() + 1);
    SizeT num_sum = 0;
    for (const auto *block_data : block_data_list) {
        num_sum += block_data->block_ids_.size();
    }
    GetSizeInBytesVecAligned<BMPBlockID>(p, num_sum);
    GetSizeInBytesVecAligned<DataType>(p, num_sum);
}

template <typename DataType>
void BlockData<DataType, BMPCompressType::kCompressed, BMPOwnMem::kTrue>::WriteToPtr(
    char *&p,
    const Vector<const BlockData<DataType, BMPCompressType::kCompressed, BMPOwnMem::kTrue> *> &block_data_list) {
    SizeT num_sum = 0;
    Vector<SizeT> block_data_prefix_sum;
    block_data_prefix_sum.push_back(0);
    for (const auto *block_data : block_data_list) {
        num_sum += block_data->block_ids_.size();
        block_data_prefix_sum.push_back(num_sum);
    }
    WriteBufVecAdvAligned<SizeT>(p, block_data_prefix_sum.data(), block_data_prefix_sum.size());
    for (const auto *block_data : block_data_list) {
        WriteBufVecAdvAligned<BMPBlockID>(p, block_data->block_ids_.data(), block_data->block_ids_.size());
    }
    for (const auto *block_data : block_data_list) {
        WriteBufVecAdvAligned<DataType>(p, block_data->max_scores_.data(), block_data->max_scores_.size());
    }
}

template struct BlockData<f32, BMPCompressType::kCompressed, BMPOwnMem::kTrue>;
template struct BlockData<f64, BMPCompressType::kCompressed, BMPOwnMem::kTrue>;

// --------------------------RawBlockData--------------------------

template <typename DataType>
SizeT BlockData<DataType, BMPCompressType::kRaw, BMPOwnMem::kTrue>::GetSizeInBytes() const {
    return sizeof(SizeT) + max_scores_.size() * sizeof(DataType);
}

template <typename DataType>
void BlockData<DataType, BMPCompressType::kRaw, BMPOwnMem::kTrue>::WriteAdv(char *&p) const {
    SizeT max_score_size = max_scores_.size();
    WriteBufAdv<SizeT>(p, max_score_size);
    WriteBufVecAdv(p, max_scores_.data(), max_scores_.size());
}

template <typename DataType>
BlockData<DataType, BMPCompressType::kRaw, BMPOwnMem::kTrue> BlockData<DataType, BMPCompressType::kRaw, BMPOwnMem::kTrue>::ReadAdv(const char *&p) {
    BlockData<DataType, BMPCompressType::kRaw, BMPOwnMem::kTrue> res;
    SizeT max_score_size = ReadBufAdv<SizeT>(p);
    res.max_scores_.resize(max_score_size);
    for (SizeT i = 0; i < max_score_size; ++i) {
        res.max_scores_[i] = ReadBufAdv<DataType>(p);
    }
    return res;
}

template <typename DataType>
void BlockData<DataType, BMPCompressType::kRaw, BMPOwnMem::kTrue>::GetSizeToPtr(
    const char *&p,
    const Vector<const BlockData<DataType, BMPCompressType::kRaw, BMPOwnMem::kTrue> *> &block_data_list) {
    GetSizeInBytesVecAligned<SizeT>(p, block_data_list.size());
    SizeT num_sum = 0;
    for (const auto *block_data : block_data_list) {
        num_sum += block_data->max_scores_.size();
    }
    GetSizeInBytesVecAligned<DataType>(p, num_sum);
}

template <typename DataType>
void BlockData<DataType, BMPCompressType::kRaw, BMPOwnMem::kTrue>::WriteToPtr(
    char *&p,
    const Vector<const BlockData<DataType, BMPCompressType::kRaw, BMPOwnMem::kTrue> *> &block_data_list) {
    SizeT num_sum = 0;
    Vector<SizeT> block_data_prefix_sum;
    block_data_prefix_sum.push_back(0);
    for (const auto *block_data : block_data_list) {
        num_sum += block_data->max_scores_.size();
        block_data_prefix_sum.push_back(num_sum);
    }
    for (const auto *block_data : block_data_list) {
        WriteBufVecAdvAligned<DataType>(p, block_data->max_scores_.data(), block_data->max_scores_.size());
    }
}

template struct BlockData<f32, BMPCompressType::kRaw, BMPOwnMem::kTrue>;
template struct BlockData<f64, BMPCompressType::kRaw, BMPOwnMem::kTrue>;

// --------------------------BlockPostings--------------------------

template <typename DataType, BMPCompressType CompressType>
SizeT BlockPostings<DataType, CompressType, BMPOwnMem::kTrue>::GetSizeInBytes() const {
    return sizeof(kth_) + sizeof(kth_score_) + data_.GetSizeInBytes();
}

template <typename DataType, BMPCompressType CompressType>
void BlockPostings<DataType, CompressType, BMPOwnMem::kTrue>::WriteAdv(char *&p) const {
    WriteBufAdv<i32>(p, kth_);
    WriteBufAdv<DataType>(p, kth_score_);
    data_.WriteAdv(p);
}

template <typename DataType, BMPCompressType CompressType>
BlockPostings<DataType, CompressType, BMPOwnMem::kTrue> BlockPostings<DataType, CompressType, BMPOwnMem::kTrue>::ReadAdv(const char *&p) {
    BlockPostings res;
    res.kth_ = ReadBufAdv<i32>(p);
    res.kth_score_ = ReadBufAdv<DataType>(p);
    res.data_ = BlockData<DataType, CompressType, BMPOwnMem::kTrue>::ReadAdv(p);
    return res;
}

template <typename DataType, BMPCompressType CompressType>
void BlockPostings<DataType, CompressType, BMPOwnMem::kTrue>::GetSizeToPtr(
    const char *&p,
    const Vector<BlockPostings<DataType, CompressType, BMPOwnMem::kTrue>> &postings) {
    GetSizeInBytesAligned<SizeT>(p);
    GetSizeInBytesVecAligned<i32>(p, postings.size());
    GetSizeInBytesVecAligned<DataType>(p, postings.size());
    Vector<const BlockData<DataType, CompressType, BMPOwnMem::kTrue> *> block_data_list;
    for (const auto &posting : postings) {
        block_data_list.push_back(&posting.data_);
    }
    BlockData<DataType, CompressType, BMPOwnMem::kTrue>::GetSizeToPtr(p, block_data_list);
}

template <typename DataType, BMPCompressType CompressType>
void BlockPostings<DataType, CompressType, BMPOwnMem::kTrue>::WriteToPtr(
    char *&p,
    const Vector<BlockPostings<DataType, CompressType, BMPOwnMem::kTrue>> &postings) {
    WriteBufAdvAligned<SizeT>(p, postings.size());
    Vector<i32> kths;
    Vector<DataType> kth_scores;
    for (const auto &posting : postings) {
        kths.push_back(posting.kth_);
        kth_scores.push_back(posting.kth_score_);
    }
    WriteBufVecAdvAligned<i32>(p, kths.data(), kths.size());
    WriteBufVecAdvAligned<DataType>(p, kth_scores.data(), kth_scores.size());
    Vector<const BlockData<DataType, CompressType, BMPOwnMem::kTrue> *> block_data_list;
    for (const auto &posting : postings) {
        block_data_list.push_back(&posting.data_);
    }
    BlockData<DataType, CompressType, BMPOwnMem::kTrue>::WriteToPtr(p, block_data_list);
}

template struct BlockPostings<f32, BMPCompressType::kCompressed, BMPOwnMem::kTrue>;
template struct BlockPostings<f32, BMPCompressType::kRaw, BMPOwnMem::kTrue>;
template struct BlockPostings<f64, BMPCompressType::kCompressed, BMPOwnMem::kTrue>;
template struct BlockPostings<f64, BMPCompressType::kRaw, BMPOwnMem::kTrue>;

} // namespace infinity