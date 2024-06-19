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

export template <typename DataType, BMPCompressType CompressType>
struct BlockData {};

template <typename DataType>
struct BlockData<DataType, BMPCompressType::kCompressed> {
public:
    void Calculate(Vector<DataType> &upper_bounds, DataType query_score) const;

    void AddBlock(BMPBlockID block_id, DataType max_score);

    SizeT GetSizeInBytes() const;
    void WriteAdv(char *&p) const;
    static BlockData<DataType, BMPCompressType::kCompressed> ReadAdv(char *&p);

private:
    Vector<BMPBlockID> block_ids_;
    Vector<DataType> max_scores_;
};

export template <typename DataType>
struct BlockData<DataType, BMPCompressType::kRaw> {
public:
    // template <bool UseSIMD = false>
    void Calculate(Vector<DataType> &upper_bounds, DataType query_score) const;

    void AddBlock(BMPBlockID block_id, DataType max_score);

    SizeT GetSizeInBytes() const;
    void WriteAdv(char *&p) const;
    static BlockData<DataType, BMPCompressType::kRaw> ReadAdv(char *&p);

public:
    Vector<DataType> max_scores_;
};

export template <typename DataType, BMPCompressType CompressType>
struct BlockPostings {
public:
    DataType kth(i32 topk) const { return topk == kth_ ? kth_score_ : 0.0; }

    SizeT GetSizeInBytes() const;
    void WriteAdv(char *&p) const;
    static BlockPostings ReadAdv(char *&p);

public:
    i32 kth_{-1};
    DataType kth_score_;
    BlockData<DataType, CompressType> data_;
};

} // namespace infinity