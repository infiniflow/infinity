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
#include <xmmintrin.h>

module bmp_alg;

import infinity_exception;
import third_party;
import serialize;

namespace infinity {

// --------------------------BMPIvt--------------------------

template <typename DataType, BMPCompressType CompressType>
SizeT BMPIvt<DataType, CompressType>::GetSizeInBytes() const {
    SizeT size = sizeof(SizeT);
    for (const auto &posting : postings_) {
        size += posting.GetSizeInBytes();
    }
    return size;
}

template <typename DataType, BMPCompressType CompressType>
void BMPIvt<DataType, CompressType>::WriteAdv(char *&p) const {
    SizeT posting_size = postings_.size();
    WriteBufAdv<SizeT>(p, posting_size);
    for (const auto &posting : postings_) {
        posting.WriteAdv(p);
    }
}

template <typename DataType, BMPCompressType CompressType>
BMPIvt<DataType, CompressType> BMPIvt<DataType, CompressType>::ReadAdv(const char *&p) {
    SizeT posting_size = ReadBufAdv<SizeT>(p);
    Vector<BlockPostings<DataType, CompressType>> postings(posting_size);
    for (SizeT i = 0; i < posting_size; ++i) {
        postings[i] = BlockPostings<DataType, CompressType>::ReadAdv(p);
    }
    return BMPIvt(std::move(postings));
}

template class BMPIvt<f32, BMPCompressType::kCompressed>;
template class BMPIvt<f32, BMPCompressType::kRaw>;
template class BMPIvt<f64, BMPCompressType::kCompressed>;
template class BMPIvt<f64, BMPCompressType::kRaw>;

// --------------------------TailFwd--------------------------

template <typename DataType, typename IdxType>
SizeT TailFwd<DataType, IdxType>::GetSizeInBytes() const {
    SizeT size = sizeof(SizeT);
    for (const auto &tail_terms : tail_terms_) {
        const auto [indices, data] = tail_terms;
        size += sizeof(SizeT) + indices.size() * sizeof(IdxType) + data.size() * sizeof(DataType);
    }
    return size;
}

template <typename DataType, typename IdxType>
void TailFwd<DataType, IdxType>::WriteAdv(char *&p) const {
    SizeT tail_num = tail_terms_.size();
    WriteBufAdv<SizeT>(p, tail_num);
    for (const auto &tail_terms : tail_terms_) {
        const auto &[indices, data] = tail_terms;
        SizeT term_num = indices.size();
        WriteBufAdv<SizeT>(p, term_num);
        WriteBufVecAdv(p, indices.data(), indices.size());
        WriteBufVecAdv(p, data.data(), data.size());
    }
}

template <typename DataType, typename IdxType>
TailFwd<DataType, IdxType> TailFwd<DataType, IdxType>::ReadAdv(const char *&p) {
    SizeT tail_num = ReadBufAdv<SizeT>(p);
    Vector<Pair<Vector<IdxType>, Vector<DataType>>> tail_terms(tail_num);
    for (SizeT i = 0; i < tail_num; ++i) {
        SizeT term_num = ReadBufAdv<SizeT>(p);
        auto &[indices, data] = tail_terms[i];
        indices.resize(term_num);
        data.resize(term_num);
        for (SizeT j = 0; j < term_num; ++j) {
            indices[j] = ReadBufAdv<IdxType>(p);
        }
        for (SizeT j = 0; j < term_num; ++j) {
            data[j] = ReadBufAdv<DataType>(p);
        }
    }
    return TailFwd(std::move(tail_terms));
}

template class TailFwd<f32, i32>;
template class TailFwd<f32, i16>;
template class TailFwd<f32, i8>;
template class TailFwd<f64, i32>;
template class TailFwd<f64, i16>;
template class TailFwd<f64, i8>;

// --------------------------BlockFwd--------------------------

template <typename DataType, typename IdxType>
SizeT BlockFwd<DataType, IdxType>::GetSizeInBytes() const {
    SizeT size = sizeof(block_size_) + sizeof(SizeT);
    for (const auto &block_terms : block_terms_list_) {
        size += block_terms.GetSizeInBytes();
    }
    size += tail_fwd_.GetSizeInBytes();
    return size;
}

template <typename DataType, typename IdxType>
void BlockFwd<DataType, IdxType>::WriteAdv(char *&p) const {
    WriteBufAdv<SizeT>(p, block_size_);
    SizeT block_num = block_terms_list_.size();
    WriteBufAdv<SizeT>(p, block_num);
    for (const auto &block_terms : block_terms_list_) {
        block_terms.WriteAdv(p);
    }
    tail_fwd_.WriteAdv(p);
}

template <typename DataType, typename IdxType>
BlockFwd<DataType, IdxType> BlockFwd<DataType, IdxType>::ReadAdv(const char *&p) {
    SizeT block_size = ReadBufAdv<SizeT>(p);
    SizeT block_num = ReadBufAdv<SizeT>(p);
    Vector<BlockTerms<DataType, IdxType>> block_terms_list;
    block_terms_list.reserve(block_num);
    for (SizeT i = 0; i < block_num; ++i) {
        auto block_terms = BlockTerms<DataType, IdxType>::ReadAdv(p);
        block_terms_list.push_back(std::move(block_terms));
    }
    auto tail_fwd = TailFwd<DataType, IdxType>::ReadAdv(p);
    return BlockFwd(block_size, std::move(block_terms_list), std::move(tail_fwd));
}

template class BlockFwd<f32, i32>;
template class BlockFwd<f32, i16>;
template class BlockFwd<f32, i8>;
template class BlockFwd<f64, i32>;
template class BlockFwd<f64, i16>;
template class BlockFwd<f64, i8>;

// --------------------------BMPAlg--------------------------

template <typename DataType, typename IdxType, BMPCompressType CompressType>
void BMPAlg<DataType, IdxType, CompressType>::Save(LocalFileHandle &file_handle) const {
    auto size = GetSizeInBytes();
    auto buffer = MakeUnique<char[]>(sizeof(size) + size);
    char *p = buffer.get();
    WriteBufAdv<SizeT>(p, size);
    WriteAdv(p);
    if (SizeT write_n = p - buffer.get(); write_n != sizeof(size) + size) {
        UnrecoverableError(fmt::format("BMPAlg::Save: write_n != sizeof(size) + size: {} != {}", write_n, sizeof(size) + size));
    }
    file_handle.Append(buffer.get(), sizeof(size) + size);
}

template <typename DataType, typename IdxType, BMPCompressType CompressType>
BMPAlg<DataType, IdxType, CompressType> BMPAlg<DataType, IdxType, CompressType>::Load(LocalFileHandle &file_handle) {
    SizeT size;
    file_handle.Read(&size, sizeof(size));
    auto buffer = MakeUnique<char[]>(size);
    file_handle.Read(buffer.get(), size);
    const char *p = buffer.get();
    return ReadAdv(p);
}

template <typename DataType, typename IdxType, BMPCompressType CompressType>
SizeT BMPAlg<DataType, IdxType, CompressType>::GetSizeInBytes() const {
    std::shared_lock lock(mtx_);

    SizeT size = 0;
    size += bm_ivt_.GetSizeInBytes();
    size += block_fwd_.GetSizeInBytes();
    size += sizeof(SizeT);
    size += doc_ids_.size() * sizeof(BMPDocID);
    return size;
}

template <typename DataType, typename IdxType, BMPCompressType CompressType>
void BMPAlg<DataType, IdxType, CompressType>::WriteAdv(char *&p) const {
    std::shared_lock lock(mtx_);

    bm_ivt_.WriteAdv(p);
    block_fwd_.WriteAdv(p);
    SizeT doc_num = doc_ids_.size();
    WriteBufAdv<SizeT>(p, doc_num);
    WriteBufVecAdv(p, doc_ids_.data(), doc_ids_.size());
}

template <typename DataType, typename IdxType, BMPCompressType CompressType>
BMPAlg<DataType, IdxType, CompressType> BMPAlg<DataType, IdxType, CompressType>::ReadAdv(const char *&p) {
    auto postings = BMPIvt<DataType, CompressType>::ReadAdv(p);
    auto block_fwd = BlockFwd<DataType, IdxType>::ReadAdv(p);
    SizeT doc_num = ReadBufAdv<SizeT>(p);
    Vector<BMPDocID> doc_ids(doc_num);
    for (SizeT i = 0; i < doc_num; ++i) {
        doc_ids[i] = ReadBufAdv<BMPDocID>(p);
    }
    return BMPAlg(std::move(postings), std::move(block_fwd), std::move(doc_ids));
}

template class BMPAlg<f32, i32, BMPCompressType::kCompressed>;
template class BMPAlg<f32, i32, BMPCompressType::kRaw>;
template class BMPAlg<f32, i16, BMPCompressType::kCompressed>;
template class BMPAlg<f32, i16, BMPCompressType::kRaw>;
template class BMPAlg<f32, i8, BMPCompressType::kCompressed>;
template class BMPAlg<f32, i8, BMPCompressType::kRaw>;

template class BMPAlg<f64, i32, BMPCompressType::kCompressed>;
template class BMPAlg<f64, i32, BMPCompressType::kRaw>;
template class BMPAlg<f64, i16, BMPCompressType::kCompressed>;
template class BMPAlg<f64, i16, BMPCompressType::kRaw>;
template class BMPAlg<f64, i8, BMPCompressType::kCompressed>;
template class BMPAlg<f64, i8, BMPCompressType::kRaw>;

} // namespace infinity