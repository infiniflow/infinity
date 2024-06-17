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

    f32 threshold = 0.0;
    i32 block_num = block_fwd_.block_num();
    Vector<f32> upper_bounds(block_num, 0.0);
    for (i32 i = 0; i < query_ref.nnz_; ++i) {
        i32 query_term = query_ref.indices_[i];
        f32 query_score = query_ref.data_[i];
        const auto &posting = postings_[query_term];
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
    for (const auto &[ub_score, block_id] : block_scores) {
        i32 off = block_id * block_fwd_.block_size();
        Vector<f32> scores = block_fwd_.GetScores(block_id, query_ref);
        for (SizeT block_off = 0; block_off < scores.size(); ++block_off) {
            i32 doc_id = off + block_off;
            f32 score = scores[block_off];
            result_handler.AddResult(0 /*query_id*/, score, doc_id);
        }
        if (ub_score * alpha < result_handler.GetDistance0(0 /*query_id*/)) {
            break;
        }
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
    for (const auto &posting : postings_) {
        size += posting.GetSizeInBytes();
    }
    size += block_fwd_.GetSizeInBytes();
    return size;
}

void BMIndex::WriteAdv(char *&p) const {
    SizeT posting_size = postings_.size();
    WriteBufAdv<SizeT>(p, posting_size);
    for (const auto &posting : postings_) {
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
