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

export module bm_index;

import stl;
import sparse_util;
import file_system;

namespace infinity {

i32 ceil_div(i32 a, i32 b) { return (a + b - 1) / b; }

struct PostingList {
public:
    f32 kth(i32 topk) const;

    SizeT GetSizeInBytes() const;

    void WriteAdv(char *&p) const;

    static PostingList ReadAdv(char *&p);

public:
    Vector<Pair<i32, f32>> max_scores_;
    Array<f32, 3> kth_score_; // [0] for 10th, [1] for 100th, [2] for 1000th
};

struct BMIvt {
public:
    BMIvt() = default;

    BMIvt(Vector<PostingList> postings) : postings_(std::move(postings)) {}

    void AddBlock(i32 block_id, const Vector<Vector<Pair<i32, f32>>> &tail_terms);

    void Optimize(Vector<Vector<f32>> ivt_scores);

    const Vector<PostingList> &GetPostings() const { return postings_; }

private:
    Vector<PostingList> postings_;
};

export class BMIndex;

struct TailFwd {
public:
    i8 AddDoc(const SparseVecRef<f32, i32> &doc);

    const Vector<Vector<Pair<i32, f32>>> &GetTailTerms() const { return tail_terms_; }

    Vector<Pair<i32, Vector<Pair<i8, f32>>>> ToBlockFwd() const;

private:
    Vector<Vector<Pair<i32, f32>>> tail_terms_;
};

struct BlockFwd {
public:
    BlockFwd(i8 block_size) : block_size_(block_size) {}

    BlockFwd(i8 block_size, Vector<Vector<Pair<i32, Vector<Pair<i8, f32>>>>> block_terms)
        : block_size_(block_size), block_terms_(std::move(block_terms)) {}

    Optional<TailFwd> AddDoc(const SparseVecRef<f32, i32> &doc);

    Vector<Vector<f32>> GetIvtScores(i32 term_num) const;

    Vector<f32> GetScores(i32 block_id, const SparseVecRef<f32, i32> &query) const;

    void Prefetch(i32 block_id) const;

    i8 block_size() const { return block_size_; }

    i32 block_num() const { return block_terms_.size(); }

    SizeT GetSizeInBytes() const;

    void WriteAdv(char *&p) const;

    static BlockFwd ReadAdv(char *&p);

private:
    i8 block_size_;
    Vector<Vector<Pair<i32, Vector<Pair<i8, f32>>>>> block_terms_;
    TailFwd tail_fwd_;
};

export class BMIndexBuilder {
public:
    BMIndexBuilder(i32 term_num, i32 block_size) : term_num_(term_num), block_size_(block_size) {}

    void AddDoc(Vector<Pair<i32, f32>> doc) { fwd_.emplace_back(std::move(doc)); }

    BMIndex Build() &&;

private:
    BlockFwd Fwd2BlockFwd() &&;

private:
    i32 term_num_;
    i32 block_size_;
    Vector<Vector<Pair<i32, f32>>> fwd_;
};

export class BMIndex {
public:
    friend class BMIndexBuilder;

    BMIndex(i32 term_num, i8 block_size) : term_num_(term_num), block_fwd_(block_size) {}

    BMIndex(Vector<PostingList> postings, BlockFwd block_fwd)
        : term_num_(postings.size()), bm_ivt_(std::move(postings)), block_fwd_(std::move(block_fwd)) {}

    void AddDoc(const SparseVecRef<f32, i32> &doc);

    void Optimize();

    Pair<Vector<i32>, Vector<f32>> SearchKnn(const SparseVecRef<f32, i32> &query, i32 topk, f32 alpha, f32 beta) const;

    void Save(FileHandler &file_handler) const;

    static BMIndex Load(FileHandler &file_handler);

private:
    SizeT GetSizeInBytes() const;

    void WriteAdv(char *&p) const;

    static BMIndex ReadAdv(char *&p);

private:
    i32 term_num_;
    BMIvt bm_ivt_;
    BlockFwd block_fwd_;
};

} // namespace infinity
