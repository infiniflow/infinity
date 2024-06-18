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
    f32 kth(i32 topk) const { return topk == kth_ ? kth_score_ : 0.0; }

    template <bool UseSIMD = false>
    void Calculate(Vector<f32> &upper_bounds, f32 query_score) const;

    SizeT GetSizeInBytes() const;
    void WriteAdv(char *&p) const;
    static PostingList ReadAdv(char *&p);

public:
    i32 kth_{-1};
    f32 kth_score_;
    Vector<i32> block_ids_;
    Vector<f32> max_scores_;
};

struct BMIvt {
private:
    BMIvt(Vector<PostingList> postings) : postings_(std::move(postings)) {}

public:
    BMIvt(i32 term_num) : postings_(term_num) {}

    void AddBlock(i32 block_id, const Vector<Vector<Pair<i32, f32>>> &tail_terms);

    void Optimize(i32 topk, Vector<Vector<f32>> ivt_scores);

    const Vector<PostingList> &GetPostings() const { return postings_; }

    i32 term_num() const { return postings_.size(); }

    SizeT GetSizeInBytes() const;
    void WriteAdv(char *&p) const;
    static BMIvt ReadAdv(char *&p);

private:
    Vector<PostingList> postings_;
};

struct TailFwd {
private:
    TailFwd(Vector<Vector<Pair<i32, f32>>> tail_terms) : tail_terms_(std::move(tail_terms)) {}

public:
    TailFwd() = default;

    i8 AddDoc(const SparseVecRef<f32, i32> &doc);

    const Vector<Vector<Pair<i32, f32>>> &GetTailTerms() const { return tail_terms_; }

    Vector<Tuple<i32, Vector<i8>, Vector<f32>>> ToBlockFwd() const;

    Vector<f32> GetScores(const SparseVecRef<f32, i32> &query) const;

    SizeT GetSizeInBytes() const;
    void WriteAdv(char *&p) const;
    static TailFwd ReadAdv(char *&p);

private:
    Vector<Vector<Pair<i32, f32>>> tail_terms_;
};

struct BlockFwd {
private:
    BlockFwd(i8 block_size, Vector<Vector<Tuple<i32, Vector<i8>, Vector<f32>>>> block_terms, TailFwd tail_fwd)
        : block_size_(block_size), block_terms_(std::move(block_terms)), tail_fwd_(std::move(tail_fwd)) {}

public:
    BlockFwd(i8 block_size) : block_size_(block_size) {}

    Optional<TailFwd> AddDoc(const SparseVecRef<f32, i32> &doc);

    Vector<Vector<f32>> GetIvtScores(i32 term_num) const;

    Vector<f32> GetScores(i32 block_id, const SparseVecRef<f32, i32> &query) const;

    Vector<f32> GetScoresTail(const SparseVecRef<f32, i32> &query) const;

    void Prefetch(i32 block_id) const;

    i8 block_size() const { return block_size_; }

    i32 block_num() const { return block_terms_.size(); }

    SizeT GetSizeInBytes() const;
    void WriteAdv(char *&p) const;
    static BlockFwd ReadAdv(char *&p);

private:
    template <bool UseSIMD = false>
    static void Calculate(const Vector<i8> &block_offsets, const Vector<f32> scores, Vector<f32> &res, f32 query_score);

private:
    i8 block_size_;
    Vector<Vector<Tuple<i32, Vector<i8>, Vector<f32>>>> block_terms_;
    TailFwd tail_fwd_;
};

export class BMIndex {
private:
    BMIndex(BMIvt bm_ivt, BlockFwd block_fwd) : bm_ivt_(std::move(bm_ivt)), block_fwd_(std::move(block_fwd)) {}

public:
    BMIndex(i32 term_num, i8 block_size) : bm_ivt_(term_num), block_fwd_(block_size) {}

    template <bool UseLock = false>
    void AddDoc(const SparseVecRef<f32, i32> &doc);

    template <bool UseLock = false>
    void Optimize(i32 topk);

    template <bool UseLock = false, bool CalTail = false>
    Pair<Vector<i32>, Vector<f32>> SearchKnn(const SparseVecRef<f32, i32> &query, i32 topk, f32 alpha, f32 beta) const;

    template <bool UseLock = false>
    void Save(FileHandler &file_handler) const;

    static BMIndex Load(FileHandler &file_handler);

private:
    SizeT GetSizeInBytes() const;

    void WriteAdv(char *&p) const;

    static BMIndex ReadAdv(char *&p);

private:
    mutable std::shared_mutex mtx_;

    BMIvt bm_ivt_;
    BlockFwd block_fwd_;
};

} // namespace infinity
