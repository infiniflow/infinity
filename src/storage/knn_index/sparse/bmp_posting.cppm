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

export module bm_posting;

import stl;
import bmp_util;

namespace infinity {

export template <typename DataType, BMPCompressType CompressType, BMPOwnMem OwnMem>
struct BlockData {};

template <typename DataType>
struct BlockData<DataType, BMPCompressType::kCompressed, BMPOwnMem::kTrue> {
public:
    void Calculate(Vector<DataType> &upper_bounds, DataType query_score) const;

    void AddBlock(BMPBlockID block_id, DataType max_score, SizeT &mem_usage);

    void Prefetch() const;

    SizeT GetSizeInBytes() const;
    void WriteAdv(char *&p) const;
    static BlockData ReadAdv(const char *&p);

    static void GetSizeToPtr(const char *&p,
                             const Vector<const BlockData<DataType, BMPCompressType::kCompressed, BMPOwnMem::kTrue> *> &block_data_list);
    static void WriteToPtr(char *&p, const Vector<const BlockData<DataType, BMPCompressType::kCompressed, BMPOwnMem::kTrue> *> &block_data_list);

private:
    Vector<BMPBlockID> block_ids_;
    Vector<DataType> max_scores_;
};

template <typename DataType>
struct BlockData<DataType, BMPCompressType::kCompressed, BMPOwnMem::kFalse> {
public:
    BlockData(SizeT block_size, const BMPBlockID *block_ids, const DataType *max_scores)
        : block_size_(block_size), block_ids_(block_ids), max_scores_(max_scores) {}

    void Calculate(Vector<DataType> &upper_bounds, DataType query_score) const;

    void Prefetch() const;

private:
    SizeT block_size_;
    const BMPBlockID *block_ids_;
    const DataType *max_scores_;
};

export template <typename DataType>
struct BlockData<DataType, BMPCompressType::kRaw, BMPOwnMem::kTrue> {
public:
    // template <bool UseSIMD = false>
    void Calculate(Vector<DataType> &upper_bounds, DataType query_score) const;

    void AddBlock(BMPBlockID block_id, DataType max_score, SizeT &mem_usage);

    void Prefetch() const;

    SizeT GetSizeInBytes() const;
    void WriteAdv(char *&p) const;
    static BlockData ReadAdv(const char *&p);

    static void GetSizeToPtr(const char *&p, const Vector<const BlockData<DataType, BMPCompressType::kRaw, BMPOwnMem::kTrue> *> &block_data_list);
    static void WriteToPtr(char *&p, const Vector<const BlockData<DataType, BMPCompressType::kRaw, BMPOwnMem::kTrue> *> &block_data_list);

public:
    Vector<DataType> max_scores_;
};

export template <typename DataType>
struct BlockData<DataType, BMPCompressType::kRaw, BMPOwnMem::kFalse> {
public:
    BlockData(SizeT block_size, const DataType *max_scores) : block_size_(block_size), max_scores_(max_scores) {}

    void Calculate(Vector<DataType> &upper_bounds, DataType query_score) const;

    void Prefetch() const;

private:
    SizeT block_size_;
    const DataType *max_scores_;
};

export template <typename DataType, BMPCompressType CompressType, BMPOwnMem OwnMem>
struct BlockPostings {};

export template <typename DataType, BMPCompressType CompressType>
struct BlockPostings<DataType, CompressType, BMPOwnMem::kTrue> {
public:
    DataType kth(i32 topk) const { return topk == kth_ ? kth_score_ : 0.0; }

    void Prefetch() const { data_.Prefetch(); }

    SizeT GetSizeInBytes() const;
    void WriteAdv(char *&p) const;
    static BlockPostings ReadAdv(const char *&p);

    static void GetSizeToPtr(const char *&p, const Vector<BlockPostings<DataType, CompressType, BMPOwnMem::kTrue>> &postings);
    static void WriteToPtr(char *&p, const Vector<BlockPostings<DataType, CompressType, BMPOwnMem::kTrue>> &postings);

public:
    i32 kth_{-1};
    DataType kth_score_;
    BlockData<DataType, CompressType, BMPOwnMem::kTrue> data_;
};

} // namespace infinity
