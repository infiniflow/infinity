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

export module bmp_blockterms;

import stl;
import bmp_util;
import infinity_exception;
import serialize;

namespace infinity {

export template <typename DataType, typename IdxType>
class BlockTerms;

export template <typename DataType, typename IdxType>
class BlockTermsIter {
public:
    BlockTermsIter(const char *ptr, const char *ptr_end) : ptr_(ptr), ptr_end_(ptr_end) {}

    bool HasNext() const { return ptr_ < ptr_end_; }

    Tuple<IdxType, SizeT, const BMPBlockOffset *, const DataType *> Value() {
        const char *p = ptr_;
        auto block_size = ReadBufAdv<SizeT>(p);
        auto term_id = ReadBufAdv<IdxType>(p);

        auto *block_offsets = reinterpret_cast<const BMPBlockOffset *>(p);
        auto *values = reinterpret_cast<const DataType *>(block_offsets + block_size);
        return {term_id, block_size, block_offsets, values};
    }

    void Next() {
        auto block_size = ReadBuf<SizeT>(ptr_);
        ptr_ += sizeof(block_size) + sizeof(IdxType) + block_size * sizeof(BMPBlockOffset) + block_size * sizeof(DataType);
    }

private:
    const char *ptr_;
    const char *ptr_end_;
};

export template <typename DataType, typename IdxType>
class BlockTerms {
    using IterT = BlockTermsIter<DataType, IdxType>;

public:
    BlockTerms(const Vector<Tuple<IdxType, Vector<BMPBlockOffset>, Vector<DataType>>> &block_terms)
        : data_len_(GetBufferSize(block_terms)), data_(MakeUniqueForOverwrite<char[]>(data_len_)) {
        char *ptr = data_.get();
        for (const auto &[term_id, block_offsets, values] : block_terms) {
            SizeT block_size = block_offsets.size();
            WriteBufAdv(ptr, block_size);
            WriteBufAdv(ptr, term_id);
            WriteBufVecAdv(ptr, block_offsets.data(), block_size);
            WriteBufVecAdv(ptr, values.data(), block_size);
        }
    }

private:
    static SizeT GetBufferSize(const Vector<Tuple<IdxType, Vector<BMPBlockOffset>, Vector<DataType>>> &block_terms) {
        SizeT size = 0;
        for (const auto &[term_id, block_offsets, values] : block_terms) {
            SizeT block_size = block_offsets.size();
            if (block_size != values.size()) {
                UnrecoverableError("Block offsets and values size mismatch");
            }
            size += sizeof(block_size);
            size += sizeof(term_id);
            size += block_size * sizeof(BMPBlockOffset);
            size += block_size * sizeof(DataType);
        }
        return size;
    }

    BlockTerms(SizeT data_len, UniquePtr<char[]> data) : data_len_(data_len), data_(std::move(data)) {}

public:
    IterT Iter() const { return IterT(data_.get(), data_.get() + data_len_); }

    SizeT GetSizeInBytes() const { return sizeof(data_len_) + data_len_; }

    void WriteAdv(char *&p) const {
        WriteBufAdv(p, data_len_);
        WriteBufVecAdv(p, data_.get(), data_len_);
    }

    static BlockTerms<DataType, IdxType> ReadAdv(const char *&p) {
        SizeT data_len = ReadBufAdv<SizeT>(p);
        UniquePtr<char[]> data = MakeUniqueForOverwrite<char[]>(data_len);
        std::memcpy(data.get(), p, data_len);
        p += data_len;
        return BlockTerms<DataType, IdxType>(data_len, std::move(data));
    }

    void Prefetch() const {
        const char *ptr = data_.get();
        _mm_prefetch(ptr, _MM_HINT_T0);
    }

private:
    SizeT data_len_{};
    UniquePtr<char[]> data_{};
};

} // namespace infinity