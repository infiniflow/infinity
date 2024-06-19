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

module bm_index;

import infinity_exception;
import third_party;
import serialize;

namespace infinity {

// --------------------------BMIvt--------------------------

template <typename DataType, BMCompressType CompressType>
SizeT BMIvt<DataType, CompressType>::GetSizeInBytes() const {
    SizeT size = sizeof(SizeT);
    for (const auto &posting : postings_) {
        size += posting.GetSizeInBytes();
    }
    return size;
}

template <typename DataType, BMCompressType CompressType>
void BMIvt<DataType, CompressType>::WriteAdv(char *&p) const {
    SizeT posting_size = postings_.size();
    WriteBufAdv<SizeT>(p, posting_size);
    for (const auto &posting : postings_) {
        posting.WriteAdv(p);
    }
}

template <typename DataType, BMCompressType CompressType>
BMIvt<DataType, CompressType> BMIvt<DataType, CompressType>::ReadAdv(char *&p) {
    SizeT posting_size = ReadBufAdv<SizeT>(p);
    Vector<BlockPostings<DataType, CompressType>> postings(posting_size);
    for (SizeT i = 0; i < posting_size; ++i) {
        postings[i] = BlockPostings<DataType, CompressType>::ReadAdv(p);
    }
    return BMIvt(std::move(postings));
}

template class BMIvt<f32, BMCompressType::kCompressed>;
template class BMIvt<f32, BMCompressType::kRaw>;
template class BMIvt<f64, BMCompressType::kCompressed>;
template class BMIvt<f64, BMCompressType::kRaw>;

// --------------------------TailFwd--------------------------

template <typename DataType, typename IdxType>
SizeT TailFwd<DataType, IdxType>::GetSizeInBytes() const {
    SizeT size = sizeof(SizeT);
    for (const auto &tail_terms : tail_terms_) {
        size += sizeof(SizeT) + tail_terms.size() * (sizeof(IdxType) + sizeof(DataType));
    }
    return size;
}

template <typename DataType, typename IdxType>
void TailFwd<DataType, IdxType>::WriteAdv(char *&p) const {
    SizeT tail_num = tail_terms_.size();
    WriteBufAdv<SizeT>(p, tail_num);
    for (const auto &tail_terms : tail_terms_) {
        SizeT term_num = tail_terms.size();
        WriteBufAdv<SizeT>(p, term_num);
        for (const auto &[term_id, score] : tail_terms) {
            WriteBufAdv<IdxType>(p, term_id);
            WriteBufAdv<DataType>(p, score);
        }
    }
}

template <typename DataType, typename IdxType>
TailFwd<DataType, IdxType> TailFwd<DataType, IdxType>::ReadAdv(char *&p) {
    SizeT tail_num = ReadBufAdv<SizeT>(p);
    Vector<Vector<Pair<IdxType, DataType>>> tail_terms(tail_num);
    for (SizeT i = 0; i < tail_num; ++i) {
        SizeT term_num = ReadBufAdv<SizeT>(p);
        tail_terms[i].resize(term_num);
        for (SizeT j = 0; j < term_num; ++j) {
            tail_terms[i][j].first = ReadBufAdv<IdxType>(p);
            tail_terms[i][j].second = ReadBufAdv<DataType>(p);
        }
    }
    return TailFwd(std::move(tail_terms));
}

template class TailFwd<f32, i32>;
template class TailFwd<f32, i16>;
template class TailFwd<f64, i32>;
template class TailFwd<f64, i16>;

// --------------------------BlockFwd--------------------------

template <typename DataType, typename IdxType>
SizeT BlockFwd<DataType, IdxType>::GetSizeInBytes() const {
    SizeT size = sizeof(block_size_) + sizeof(SizeT);
    for (const auto &block_terms : block_terms_) {
        size += sizeof(SizeT);
        for (const auto &[term_id, block_offsets, scores] : block_terms) {
            size += sizeof(IdxType) + sizeof(SizeT) + block_offsets.size() * sizeof(BMBlockOffset) + scores.size() * sizeof(DataType);
        }
    }
    size += tail_fwd_.GetSizeInBytes();
    return size;
}

template <typename DataType, typename IdxType>
void BlockFwd<DataType, IdxType>::WriteAdv(char *&p) const {
    WriteBufAdv<SizeT>(p, block_size_);
    SizeT block_num = block_terms_.size();
    WriteBufAdv<SizeT>(p, block_num);
    for (const auto &block_terms : block_terms_) {
        SizeT term_num = block_terms.size();
        WriteBufAdv<SizeT>(p, term_num);
        for (const auto &[term_id, block_offsets, scores] : block_terms) {
            WriteBufAdv<IdxType>(p, term_id);
            SizeT inner_num = block_offsets.size();
            WriteBufAdv<SizeT>(p, inner_num);
            WriteBufCharsAdv(p, reinterpret_cast<const char *>(block_offsets.data()), sizeof(BMBlockOffset) * block_offsets.size());
            WriteBufCharsAdv(p, reinterpret_cast<const char *>(scores.data()), sizeof(DataType) * scores.size());
        }
    }
    tail_fwd_.WriteAdv(p);
}

template <typename DataType, typename IdxType>
BlockFwd<DataType, IdxType> BlockFwd<DataType, IdxType>::ReadAdv(char *&p) {
    SizeT block_size = ReadBufAdv<SizeT>(p);
    SizeT block_num = ReadBufAdv<SizeT>(p);
    Vector<Vector<Tuple<IdxType, Vector<BMBlockOffset>, Vector<DataType>>>> block_terms(block_num);
    for (SizeT i = 0; i < block_num; ++i) {
        SizeT term_num = ReadBufAdv<SizeT>(p);
        block_terms[i].resize(term_num);
        for (SizeT j = 0; j < term_num; ++j) {
            IdxType term_id = ReadBufAdv<IdxType>(p);
            SizeT inner_num = ReadBufAdv<SizeT>(p);
            Vector<BMBlockOffset> block_offsets(inner_num);
            Vector<DataType> scores(inner_num);
            for (SizeT k = 0; k < inner_num; ++k) {
                block_offsets[k] = ReadBufAdv<BMBlockOffset>(p);
            }
            for (SizeT k = 0; k < inner_num; ++k) {
                scores[k] = ReadBufAdv<DataType>(p);
            }
            block_terms[i][j] = {term_id, std::move(block_offsets), std::move(scores)};
        }
    }
    auto tail_fwd = TailFwd<DataType, IdxType>::ReadAdv(p);
    return BlockFwd(block_size, std::move(block_terms), std::move(tail_fwd));
}

template class BlockFwd<f32, i32>;
template class BlockFwd<f32, i16>;
template class BlockFwd<f64, i32>;
template class BlockFwd<f64, i16>;

// --------------------------BMIndex--------------------------

template <typename DataType, typename IdxType, BMCompressType CompressType>
void BMIndex<DataType, IdxType, CompressType>::Save(FileHandler &file_handler) const {
    SizeT size = GetSizeInBytes();
    auto buffer = MakeUnique<char[]>(sizeof(size) + size);
    char *p = buffer.get();
    WriteBufAdv<SizeT>(p, size);
    WriteAdv(p);
    if (SizeT write_n = p - buffer.get(); write_n != sizeof(size) + size) {
        UnrecoverableError(fmt::format("BMIndex::Save: write_n != sizeof(size) + size: {} != {}", write_n, sizeof(size) + size));
    }
    file_handler.Write(buffer.get(), sizeof(size) + size);
}

template <typename DataType, typename IdxType, BMCompressType CompressType>
BMIndex<DataType, IdxType, CompressType> BMIndex<DataType, IdxType, CompressType>::Load(FileHandler &file_handler) {
    SizeT size;
    file_handler.Read(&size, sizeof(size));
    auto buffer = MakeUnique<char[]>(size);
    file_handler.Read(buffer.get(), size);
    char *p = buffer.get();
    return ReadAdv(p);
}

template <typename DataType, typename IdxType, BMCompressType CompressType>
SizeT BMIndex<DataType, IdxType, CompressType>::GetSizeInBytes() const {
    SizeT size = 0;
    size += bm_ivt_.GetSizeInBytes();
    size += block_fwd_.GetSizeInBytes();
    return size;
}

template <typename DataType, typename IdxType, BMCompressType CompressType>
void BMIndex<DataType, IdxType, CompressType>::WriteAdv(char *&p) const {
    bm_ivt_.WriteAdv(p);
    block_fwd_.WriteAdv(p);
}

template <typename DataType, typename IdxType, BMCompressType CompressType>
BMIndex<DataType, IdxType, CompressType> BMIndex<DataType, IdxType, CompressType>::ReadAdv(char *&p) {
    auto postings = BMIvt<DataType, CompressType>::ReadAdv(p);
    auto block_fwd = BlockFwd<DataType, IdxType>::ReadAdv(p);
    return BMIndex(std::move(postings), std::move(block_fwd));
}

template class BMIndex<f32, i32, BMCompressType::kCompressed>;
template class BMIndex<f32, i32, BMCompressType::kRaw>;
template class BMIndex<f32, i16, BMCompressType::kCompressed>;
template class BMIndex<f32, i16, BMCompressType::kRaw>;

template class BMIndex<f64, i32, BMCompressType::kCompressed>;
template class BMIndex<f64, i32, BMCompressType::kRaw>;
template class BMIndex<f64, i16, BMCompressType::kCompressed>;
template class BMIndex<f64, i16, BMCompressType::kRaw>;

} // namespace infinity