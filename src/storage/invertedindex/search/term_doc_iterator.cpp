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

#include <cassert>
#include <iostream>
module term_doc_iterator;

import stl;
import column_length_io;
import logger;

namespace infinity {

TermDocIterator::TermDocIterator(UniquePtr<PostingIterator> &&iter, const u64 column_id, const float weight, const FulltextSimilarity ft_similarity)
    : column_id_(column_id), iter_(std::move(iter)), weight_(weight), ft_similarity_(ft_similarity) {
    doc_freq_ = iter_->GetDocFreq();
    term_freq_ = 0;
    estimate_iterate_cost_ = {0, doc_freq_};
}

TermDocIterator::~TermDocIterator() {
    if (SHOULD_LOG_TRACE()) {
        OStringStream oss;
        oss << "TermDocIterator Debug Info:\n    column name: " << *column_name_ptr_ << ", term: " << *term_ptr_
            << "\n    access_bm_score_cnt: " << access_bm_score_cnt_ << ", calc_bm_score_cnt: " << calc_bm_score_cnt_
            << "\n    access_score_cnt: " << access_score_cnt_ << ", calc_score_cnt: " << calc_score_cnt_ << ", seek_cnt: " << seek_cnt_
            << "\n    block_skip_cnt: " << block_skip_cnt_ << " block_skip_cnt_inner: " << block_skip_cnt_inner_;
        if (duplicate_calc_score_cnt_) {
            oss << "!!! duplicate_calc_score_cnt: " << duplicate_calc_score_cnt_ << '\n';
        }
        LOG_TRACE(std::move(oss).str());
    }
}

void TermDocIterator::InitBM25Info(UniquePtr<FullTextColumnLengthReader> &&column_length_reader) {
    // BM25 parameters
    constexpr float k1 = 1.2F;
    constexpr float b = 0.75F;

    column_length_reader_ = std::move(column_length_reader);
    avg_column_len_ = column_length_reader_->GetAvgColumnLength();
    const float smooth_idf = std::log(1.0F + (column_length_reader_->GetTotalDF() - doc_freq_ + 0.5F) / (doc_freq_ + 0.5F));
    bm25_common_score_ = weight_ * smooth_idf * (k1 + 1.0F);
    bm25_score_upper_bound_ = bm25_common_score_ / (1.0F + k1 * b / avg_column_len_);
    f1 = k1 * (1.0F - b);
    f2 = k1 * b / avg_column_len_;
    f3 = f2 * std::numeric_limits<u16>::max();
    if (SHOULD_LOG_TRACE()) {
        OStringStream oss;
        oss << "TermDocIterator: ";
        if (column_name_ptr_ != nullptr && term_ptr_ != nullptr) {
            oss << "column: " << *column_name_ptr_ << ", term: " << *term_ptr_ << ",";
        }
        oss << "bm25_common_score: " << bm25_common_score_ << ", bm25_score_upper_bound: " << bm25_score_upper_bound_
            << ", avg_column_len: " << avg_column_len_ << ", f1: " << f1 << ", f2: " << f2 << '\n';
        LOG_TRACE(std::move(oss).str());
    }
}

float TermDocIterator::BlockMaxBM25Score() {
    ++access_bm_score_cnt_;
    if (const auto last_doc_id = BlockLastDocID(); last_doc_id == block_max_bm25_score_cache_end_id_) [[likely]] {
        return block_max_bm25_score_cache_;
    } else {
        ++calc_bm_score_cnt_;
        block_max_bm25_score_cache_end_id_ = last_doc_id;
        // bm25_common_score_ / (1.0F + k1 * ((1.0F - b) / block_max_tf + b / block_max_percentage / avg_column_len));
        const auto [block_max_tf, block_max_percentage_u16] = iter_->GetBlockMaxInfo();
        return block_max_bm25_score_cache_ = bm25_common_score_ / (1.0F + f1 / block_max_tf + f3 / block_max_percentage_u16);
    }
}

bool TermDocIterator::Next(RowID doc_id) {
    assert(doc_id != INVALID_ROWID);
    if (doc_id_ != INVALID_ROWID && doc_id_ >= doc_id)
        return true;
    while (true) {
        ++seek_cnt_;
        doc_id_ = iter_->SeekDoc(doc_id);
        if (doc_id_ == INVALID_ROWID)
            return false;

        if (threshold_ <= 0.0f || BlockMaxBM25Score() > threshold_) {
            return true;
        }
        doc_id = BlockLastDocID() + 1;
    }
}

bool TermDocIterator::NextShallow(RowID doc_id) {
    ++block_skip_cnt_;
    if (threshold_ > BM25ScoreUpperBound()) [[unlikely]] {
        doc_id_ = INVALID_ROWID;
        return false;
    }
    while (true) {
        ++block_skip_cnt_inner_;
        if (!iter_->SkipTo(doc_id)) {
            doc_id_ = INVALID_ROWID;
            return false;
        }
        if (threshold_ <= 0.0f || BlockMaxBM25Score() > threshold_) {
            return true;
        }
        doc_id = BlockLastDocID() + 1;
    }
}

float TermDocIterator::BM25Score() {
    ++access_score_cnt_;
    if (doc_id_ == bm25_score_cache_docid_) [[unlikely]] {
        return bm25_score_cache_;
    }
    calc_score_cnt_++;
    bm25_score_cache_docid_ = doc_id_;
    // bm25_common_score_ * tf / (tf + k1 * (1.0F - b + b * column_len / avg_column_len));
    const auto tf = iter_->GetCurrentTF();
    const auto doc_len = column_length_reader_->GetColumnLength(doc_id_);
    const float p = f1 + f2 * doc_len;
    bm25_score_cache_ = bm25_common_score_ * tf / (tf + p);
    term_freq_ += tf;
    return bm25_score_cache_;
}

void TermDocIterator::PrintTree(std::ostream &os, const String &prefix, bool is_final) const {
    os << prefix;
    os << (is_final ? "└──" : "├──");
    os << "TermDocIterator";
    os << " (weight: " << weight_ << ")";
    os << " (column: " << *column_name_ptr_ << ")";
    os << " (term: " << *term_ptr_ << ")";
    os << " (doc_freq: " << GetDocFreq() << ")";
    os << " (bm25_score_upper_bound: " << BM25ScoreUpperBound() << ")";
    os << " (threshold: " << Threshold() << ")";
    os << " (bm25_score_cache_docid_: " << bm25_score_cache_docid_.ToUint64() << ")";
    os << " (bm25_score_cache_: " << bm25_score_cache_ << ")";
    os << '\n';
}

} // namespace infinity
