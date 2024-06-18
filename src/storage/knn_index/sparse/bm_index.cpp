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
import knn_result_handler;
import serialize;

namespace infinity {

f32 PostingList::kth(i32 topk) const {
    switch (topk) {
        case 10: {
            return kth_score_[0];
        }
        case 100: {
            return kth_score_[1];
        }
        case 1000: {
            return kth_score_[2];
        }
        default: {
            return 0.0;
        }
    }
}

SizeT PostingList::GetSizeInBytes() const {
    SizeT size = sizeof(SizeT) + max_scores_.size() * (sizeof(i32) + sizeof(f32));
    size += 3 * sizeof(f32);
    return size;
}

void PostingList::WriteAdv(char *&p) const {
    SizeT max_score_size = max_scores_.size();
    WriteBufAdv<SizeT>(p, max_score_size);
    for (const auto &[block_id, score] : max_scores_) {
        WriteBufAdv<i32>(p, block_id);
        WriteBufAdv<f32>(p, score);
    }
    for (i32 i = 0; i < 3; ++i) {
        WriteBufAdv<f32>(p, kth_score_[i]);
    }
}

PostingList PostingList::ReadAdv(char *&p) {
    PostingList res;
    SizeT max_score_size = ReadBufAdv<SizeT>(p);
    res.max_scores_.resize(max_score_size);
    for (SizeT i = 0; i < max_score_size; ++i) {
        res.max_scores_[i].first = ReadBufAdv<i32>(p);
        res.max_scores_[i].second = ReadBufAdv<f32>(p);
    }
    for (i32 i = 0; i < 3; ++i) {
        res.kth_score_[i] = ReadBufAdv<f32>(p);
    }
    return res;
}

void BMIvt::AddBlock(i32 block_id, const Vector<Vector<Pair<i32, f32>>> &tail_terms) {
    HashMap<i32, f32> max_scores;
    for (const auto &terms : tail_terms) {
        for (const auto &[term_id, score] : terms) {
            max_scores[term_id] = std::max(max_scores[term_id], score);
        }
    }
    for (const auto &[term_id, score] : max_scores) {
        postings_[term_id].max_scores_.emplace_back(block_id, score);
    }
}

void BMIvt::Optimize(Vector<Vector<f32>> ivt_scores) {
    for (i32 term_id = 0; term_id < (i32)ivt_scores.size(); ++term_id) {
        auto &posting = postings_[term_id];
        auto &term_scores = ivt_scores[term_id];
        std::sort(term_scores.begin(), term_scores.end(), std::greater<>());
        for (i32 i = 0; i < 3; ++i) {
            i32 k = std::pow(10, i + 1);
            if (k < (i32)term_scores.size()) {
                posting.kth_score_[i] = term_scores[k];
            }
        }
    }
}

i8 TailFwd::AddDoc(const SparseVecRef<f32, i32> &doc) {
    Vector<Pair<i32, f32>> doc_terms;
    for (i32 i = 0; i < doc.nnz_; ++i) {
        doc_terms.emplace_back(doc.indices_[i], doc.data_[i]);
    }
    tail_terms_.emplace_back(std::move(doc_terms));
    return tail_terms_.size();
}

Vector<Pair<i32, Vector<Pair<i8, f32>>>> TailFwd::ToBlockFwd() const {
    Vector<Tuple<i32, i8, f32>> term_pairs;
    i8 block_size = tail_terms_.size();
    for (i8 block_offset = 0; block_offset < block_size; ++block_offset) {
        for (const auto &[term_id, score] : tail_terms_[block_offset]) {
            term_pairs.emplace_back(term_id, block_offset, score);
        }
    }
    std::sort(term_pairs.begin(), term_pairs.end(), [](const auto &a, const auto &b) { return std::get<0>(a) < std::get<0>(b); });

    Vector<Pair<i32, Vector<Pair<i8, f32>>>> res;
    i32 last_term_id = -1;
    Vector<Pair<i8, f32>> inners;
    for (const auto &[term_id, block_offset, score] : term_pairs) {
        if (term_id != last_term_id) {
            if (last_term_id != -1) {
                Vector<Pair<i8, f32>> inners1;
                std::swap(inners1, inners);
                res.emplace_back(last_term_id, std::move(inners1));
            }
            last_term_id = term_id;
        }
        inners.emplace_back(block_offset, score);
    }
    if (!inners.empty()) {
        res.emplace_back(last_term_id, std::move(inners));
    }
    return res;
}

Optional<TailFwd> BlockFwd::AddDoc(const SparseVecRef<f32, i32> &doc) {
    i8 tail_size = tail_fwd_.AddDoc(doc);
    if (tail_size < block_size_) {
        return None;
    }
    TailFwd tail_fwd1;
    std::swap(tail_fwd1, tail_fwd_);

    Vector<Pair<i32, Vector<Pair<i8, f32>>>> block_terms = tail_fwd1.ToBlockFwd();
    block_terms_.emplace_back(std::move(block_terms));
    return tail_fwd1;
}

Vector<Vector<f32>> BlockFwd::GetIvtScores(i32 term_num) const {
    Vector<Vector<f32>> res(term_num);
    for (const auto &block_terms : block_terms_) {
        for (const auto &[term_id, inners] : block_terms) {
            for (const auto &[block_offset, block_score] : inners) {
                res[term_id].emplace_back(block_score);
            }
        }
    }
    return res;
}

Vector<f32> BlockFwd::GetScores(i32 block_id, const SparseVecRef<f32, i32> &query) const {
    const auto &block_terms = block_terms_[block_id];
    Vector<f32> res(block_size_, 0.0);
    SizeT j = 0;
    for (i32 i = 0; i < query.nnz_; ++i) {
        i32 query_term = query.indices_[i];
        f32 query_score = query.data_[i];
        while (j < block_terms.size() && block_terms[j].first < query_term) {
            ++j;
        }
        if (j == block_terms.size()) {
            break;
        }
        if (block_terms[j].first == query_term) {
            const auto &inners = block_terms[j].second;
            for (const auto &[block_offset, block_score] : inners) {
                res[block_offset] += query_score * block_score;
            }
        }
    }
    return res;
}

void BlockFwd::Prefetch(i32 block_id) const {
    const auto *ptr = reinterpret_cast<const char *>(block_terms_[block_id].data());
    _mm_prefetch(ptr, _MM_HINT_T0);
}

SizeT BlockFwd::GetSizeInBytes() const {
    SizeT size = sizeof(block_size_) + sizeof(SizeT);
    for (const auto &block_terms : block_terms_) {
        size += sizeof(SizeT);
        for (const auto &[term_id, inners] : block_terms) {
            size += sizeof(i32) + sizeof(SizeT) + inners.size() * (sizeof(i8) + sizeof(f32));
        }
    }
    return size;
}

void BlockFwd::WriteAdv(char *&p) const {
    WriteBufAdv<i32>(p, block_size_);
    SizeT block_num = block_terms_.size();
    WriteBufAdv<SizeT>(p, block_num);
    for (const auto &block_terms : block_terms_) {
        SizeT term_num = block_terms.size();
        WriteBufAdv<SizeT>(p, term_num);
        for (const auto &[term_id, inners] : block_terms) {
            WriteBufAdv<i32>(p, term_id);
            SizeT inner_num = inners.size();
            WriteBufAdv<SizeT>(p, inner_num);
            for (const auto &[block_offset, block_score] : inners) {
                WriteBufAdv<i8>(p, block_offset);
                WriteBufAdv<f32>(p, block_score);
            }
        }
    }
}

BlockFwd BlockFwd::ReadAdv(char *&p) {
    i32 block_size = ReadBufAdv<i32>(p);
    SizeT block_num = ReadBufAdv<SizeT>(p);
    Vector<Vector<Pair<i32, Vector<Pair<i8, f32>>>>> block_terms(block_num);
    for (SizeT i = 0; i < block_num; ++i) {
        SizeT term_num = ReadBufAdv<SizeT>(p);
        block_terms[i].resize(term_num);
        for (SizeT j = 0; j < term_num; ++j) {
            i32 term_id = ReadBufAdv<i32>(p);
            SizeT inner_num = ReadBufAdv<SizeT>(p);
            block_terms[i][j].first = term_id;
            block_terms[i][j].second.resize(inner_num);
            for (SizeT k = 0; k < inner_num; ++k) {
                block_terms[i][j].second[k].first = ReadBufAdv<i8>(p);
                block_terms[i][j].second[k].second = ReadBufAdv<f32>(p);
            }
        }
    }
    return BlockFwd(block_size, std::move(block_terms));
}

BMIndex BMIndexBuilder::Build() && {
    i32 doc_num = fwd_.size();
    i32 block_size = block_size_;
    i32 block_num = ceil_div(doc_num, block_size_);

    Vector<Vector<Pair<i32, f32>>> ivt(term_num_);
    for (u32 doc_id = 0; doc_id < fwd_.size(); ++doc_id) {
        const auto &terms = fwd_[doc_id];
        for (const auto &[term_id, score] : terms) {
            ivt[term_id].emplace_back(doc_id, score);
        }
    }
    BlockFwd block_fwd = std::move(*this).Fwd2BlockFwd();

    Vector<PostingList> postings(term_num_);
    for (i32 term_id = 0; term_id < term_num_; ++term_id) {
        auto &posting = ivt[term_id];
        auto &bm = postings[term_id];
        Vector<f32> max_score(block_num, 0.0);
        for (const auto &[doc_id, score] : posting) {
            i32 block_id = doc_id / block_size;
            max_score[block_id] = std::max(max_score[block_id], score);
        }
        for (i32 i = 0; i < block_num; ++i) {
            if (max_score[i] > 0) {
                bm.max_scores_.emplace_back(i, max_score[i]);
            }
        }

        std::sort(posting.begin(), posting.end(), [&](const auto &a, const auto &b) { return a.second > b.second; });
        for (i32 i = 0; i < 3; ++i) {
            i32 k = std::pow(10, i + 1);
            if (k < (i32)posting.size()) {
                bm.kth_score_[i] = posting[k].second;
            }
        }
    }
    return BMIndex(std::move(postings), std::move(block_fwd));
}

BlockFwd BMIndexBuilder::Fwd2BlockFwd() && {
    i32 doc_num = fwd_.size();
    i32 block_num = ceil_div(doc_num, block_size_);
    Vector<Vector<Pair<i32, Vector<Pair<i8, f32>>>>> res;
    res.reserve(block_num);
    for (i32 off = 0; off < doc_num; off += block_size_) {
        Vector<Tuple<i32, i32, f32>> term_pairs;
        for (i32 doc_id = off; doc_id < std::min(off + block_size_, doc_num); ++doc_id) {
            Vector<Pair<i32, f32>> terms = std::move(fwd_[doc_id]);
            for (const auto &[term_id, score] : terms) {
                term_pairs.emplace_back(term_id, doc_id, score);
            }
        }
        std::sort(term_pairs.begin(), term_pairs.end(), [](const auto &a, const auto &b) { return std::get<0>(a) < std::get<0>(b); });

        Vector<Pair<i32, Vector<Pair<i8, f32>>>> block_terms;
        i32 last_term_id = -1;
        Vector<Pair<i8, f32>> inners;
        for (const auto &[term_id, doc_id, score] : term_pairs) {
            if (term_id != last_term_id) {
                if (last_term_id != -1) {
                    Vector<Pair<i8, f32>> inners1;
                    std::swap(inners1, inners);
                    block_terms.emplace_back(last_term_id, std::move(inners1));
                }
                last_term_id = term_id;
            }
            i8 block_offset = doc_id % block_size_;
            inners.emplace_back(block_offset, score);
        }
        if (!inners.empty()) {
            block_terms.emplace_back(last_term_id, std::move(inners));
        }
        res.push_back(std::move(block_terms));
    }
    return BlockFwd(block_size_, std::move(res));
}

void BMIndex::AddDoc(const SparseVecRef<f32, i32> &doc) {
    Optional<TailFwd> tail_fwd = block_fwd_.AddDoc(doc);
    if (!tail_fwd.has_value()) {
        return;
    }
    i32 block_id = block_fwd_.block_num() - 1;
    const auto &tail_terms = tail_fwd->GetTailTerms();
    bm_ivt_.AddBlock(block_id, tail_terms);
}

void BMIndex::Optimize() {
    Vector<Vector<f32>> ivt_scores = block_fwd_.GetIvtScores(term_num_);
    bm_ivt_.Optimize(std::move(ivt_scores));
}

Pair<Vector<i32>, Vector<f32>> BMIndex::SearchKnn(const SparseVecRef<f32, i32> &query, i32 topk, f32 alpha, f32 beta) const {
    SparseVecEle<f32, i32> keeped_query;
    if (beta < 1.0) {
        i32 terms_to_keep = std::ceil(query.nnz_ * beta);
        Vector<i32> query_term_idxes(query.nnz_);
        std::iota(query_term_idxes.begin(), query_term_idxes.end(), 0);
        std::partial_sort(query_term_idxes.begin(), query_term_idxes.begin() + terms_to_keep, query_term_idxes.end(), [&](i32 a, i32 b) {
            return query.data_[a] > query.data_[b];
        });
        query_term_idxes.resize(terms_to_keep);
        std::sort(query_term_idxes.begin(), query_term_idxes.end(), [&](i32 a, i32 b) { return query.indices_[a] < query.indices_[b]; });

        keeped_query.Init(terms_to_keep);
        i32 i = 0;
        for (i32 query_term_idx : query_term_idxes) {
            keeped_query.indices_[i] = query.indices_[query_term_idx];
            keeped_query.data_[i] = query.data_[query_term_idx];
            ++i;
        }
    }
    const SparseVecRef<f32, i32> &query_ref =
        beta < 1.0 ? SparseVecRef<f32, i32>(keeped_query.nnz_, keeped_query.indices_.get(), keeped_query.data_.get()) : query;

    const auto &postings = bm_ivt_.GetPostings();
    f32 threshold = 0.0;
    i32 block_num = block_fwd_.block_num();
    Vector<f32> upper_bounds(block_num, 0.0);
    for (i32 i = 0; i < query_ref.nnz_; ++i) {
        i32 query_term = query_ref.indices_[i];
        f32 query_score = query_ref.data_[i];
        const auto &posting = postings[query_term];
        threshold = std::max(threshold, query_score * posting.kth(topk));
        for (const auto &[block_id, score] : posting.max_scores_) {
            upper_bounds[block_id] += score * query_score;
        }
    }

    Vector<Pair<f32, i32>> block_scores;
    for (i32 block_id = 0; block_id < block_num; ++block_id) {
        if (upper_bounds[block_id] >= threshold) {
            block_scores.emplace_back(upper_bounds[block_id], block_id);
        }
    }
    std::sort(block_scores.begin(), block_scores.end(), [](const auto &a, const auto &b) { return a.first > b.first; });

    Vector<i32> result(topk);
    Vector<f32> result_score(topk);
    HeapResultHandler<CompareMin<f32, i32>> result_handler(1 /*query_n*/, topk, result_score.data(), result.data());

    SizeT block_scores_num = block_scores.size();
    f32 prev_ub_score = block_scores[0].first;
    i32 prev_block_id = block_scores[0].second;
    for (SizeT i = 1; i < block_scores_num; ++i) {
        const auto &[next_ub_score, next_block_id] = block_scores[i];
        block_fwd_.Prefetch(next_block_id);
        i32 off = prev_block_id * block_fwd_.block_size();
        Vector<f32> scores = block_fwd_.GetScores(prev_block_id, query_ref);
        for (SizeT block_off = 0; block_off < scores.size(); ++block_off) {
            i32 doc_id = off + block_off;
            f32 score = scores[block_off];
            result_handler.AddResult(0 /*query_id*/, score, doc_id);
        }
        if (prev_ub_score * alpha < result_handler.GetDistance0(0 /*query_id*/)) {
            break;
        }
        prev_ub_score = next_ub_score;
        prev_block_id = next_block_id;
    }
    result_handler.End(0 /*query_id*/);
    return {result, result_score};
}

void BMIndex::Save(FileHandler &file_handler) const {
    SizeT size = GetSizeInBytes();
    auto buffer = MakeUnique<char[]>(sizeof(size) + size);
    char *p = buffer.get();
    WriteBufAdv<SizeT>(p, size);
    WriteAdv(p);
    file_handler.Write(buffer.get(), sizeof(size) + size);
}

BMIndex BMIndex::Load(FileHandler &file_handler) {
    SizeT size;
    file_handler.Read(&size, sizeof(size));
    auto buffer = MakeUnique<char[]>(size);
    file_handler.Read(buffer.get(), size);
    char *p = buffer.get();
    return ReadAdv(p);
}

SizeT BMIndex::GetSizeInBytes() const {
    SizeT size = 0;
    size += sizeof(SizeT);
    const auto &postings = bm_ivt_.GetPostings();
    for (const auto &posting : postings) {
        size += posting.GetSizeInBytes();
    }
    size += block_fwd_.GetSizeInBytes();
    return size;
}

void BMIndex::WriteAdv(char *&p) const {
    const auto &postings = bm_ivt_.GetPostings();
    SizeT posting_size = postings.size();
    WriteBufAdv<SizeT>(p, posting_size);
    for (const auto &posting : postings) {
        posting.WriteAdv(p);
    }
    block_fwd_.WriteAdv(p);
}

BMIndex BMIndex::ReadAdv(char *&p) {
    SizeT posting_size = ReadBufAdv<SizeT>(p);
    Vector<PostingList> postings(posting_size);
    for (SizeT i = 0; i < posting_size; ++i) {
        postings[i] = PostingList::ReadAdv(p);
    }
    BlockFwd block_fwd = BlockFwd::ReadAdv(p);
    return BMIndex(std::move(postings), std::move(block_fwd));
}

} // namespace infinity
