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

namespace infinity {

export enum class BMCompressType : i8 {
    kRaw = 0,
    kCompressed = 1,
};

export template <BMCompressType CompressType>
struct BlockData {};

template <>
struct BlockData<BMCompressType::kCompressed> {
public:
    void Calculate(Vector<f32> &upper_bounds, f32 query_score) const;

    void AddBlock(i32 block_id, f32 max_score);

    SizeT GetSizeInBytes() const;
    void WriteAdv(char *&p) const;
    static BlockData<BMCompressType::kCompressed> ReadAdv(char *&p);

private:
    Vector<i32> block_ids_;
    Vector<f32> max_scores_;
};

export template <>
struct BlockData<BMCompressType::kRaw> {
public:
    // template <bool UseSIMD = false>
    void Calculate(Vector<f32> &upper_bounds, f32 query_score) const;

    void AddBlock(i32 block_id, f32 max_score);

    SizeT GetSizeInBytes() const;
    void WriteAdv(char *&p) const;
    static BlockData<BMCompressType::kRaw> ReadAdv(char *&p);

public:
    Vector<f32> max_scores_;
};

export template <BMCompressType CompressType>
struct BlockPostings {
public:
    f32 kth(i32 topk) const { return topk == kth_ ? kth_score_ : 0.0; }

    SizeT GetSizeInBytes() const;
    void WriteAdv(char *&p) const;
    static BlockPostings ReadAdv(char *&p);

public:
    i32 kth_{-1};
    f32 kth_score_;
    BlockData<CompressType> data_;
};

} // namespace infinity