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
#include <cmath>
#include <iostream>
#include <tuple>
#include <string>
module blockmax_term_doc_iterator;

import stl;
import index_defines;
import internal_types;

import segment_posting;
import posting_iterator;
import column_length_io;
import infinity_exception;
import logger;

namespace infinity {

BlockMaxTermDocIterator::~BlockMaxTermDocIterator() {
    OStringStream oss;
    oss << "BlockMaxTermDocIterator Debug Info:\n    column name: " << *column_name_ptr_ << " term: " << *term_ptr_
        << "\n    access_bm_score_cnt: " << access_bm_score_cnt_ << " calc_bm_score_cnt: " << calc_bm_score_cnt_
        << "\n    access_score_cnt: " << access_score_cnt_ << " calc_score_cnt: " << calc_score_cnt_
        << " seek_cnt: " << seek_cnt_ << " peek_cnt: " << peek_cnt_
        << " block_skip_cnt: " << block_skip_cnt_ << " block_skip_cnt_inner: " << block_skip_cnt_inner_ << "\n";
    if (duplicate_calc_score_cnt_) {
        oss << "!!! duplicate_calc_score_cnt: " << duplicate_calc_score_cnt_ << '\n';
    }
    LOG_TRACE(std::move(oss).str());
}

BlockMaxTermDocIterator::BlockMaxTermDocIterator(optionflag_t flag) : iter_(flag) {}

bool BlockMaxTermDocIterator::InitPostingIterator(SharedPtr<Vector<SegmentPosting>> seg_postings, const u32 state_pool_size) {
    if (iter_.Init(std::move(seg_postings), state_pool_size)) {
        doc_freq_ = iter_.GetDocFreq();
        return true;
    }

    String error_message = "Unexpected case: Init PostingIterator failed";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return false;
}

bool BlockMaxTermDocIterator::NextShallow(RowID doc_id){
    ++block_skip_cnt_;
    if (threshold_ > BM25ScoreUpperBound()) [[unlikely]] {
        doc_id_ = INVALID_ROWID;
        return false;
    }
    while (true) {
        ++block_skip_cnt_inner_;
        if (!iter_.SkipTo(doc_id)) {
            doc_id_ = INVALID_ROWID;
            return false;
        }
        if (BlockMaxBM25Score() > threshold_) {
            return true;
        }
        doc_id = BlockLastDocID() + 1;
    }
}

bool BlockMaxTermDocIterator::Next(RowID doc_id){
    assert(doc_id != INVALID_ROWID);
    assert(last_target_doc_id_ == INVALID_ROWID || doc_id > doc_id_);
    last_target_doc_id_ = doc_id;
    while (true) {
        ++seek_cnt_;
        doc_id_ = iter_.SeekDoc(doc_id);
        if (doc_id_ == INVALID_ROWID)
            return false;
        if (BlockMaxBM25Score() > threshold_) {
            return true;
        }
        doc_id = BlockLastDocID() + 1;
    }
}


bool BlockMaxTermDocIterator::BlockSkipTo(RowID doc_id, float threshold) {
    ++block_skip_cnt_;
    if (threshold > BM25ScoreUpperBound()) [[unlikely]] {
        return false;
    }
    while (true) {
        ++block_skip_cnt_inner_;
        if (!iter_.SkipTo(doc_id)) {
            doc_id_ = INVALID_ROWID;
            return false;
        }
        if (BlockMaxBM25Score() >= threshold) {
            return true;
        }
        doc_id = BlockLastDocID() + 1;
    }
}

// BM25 parameters
constexpr float k1 = 1.2F;
constexpr float b = 0.75F;

void BlockMaxTermDocIterator::InitBM25Info(u64 total_df, float avg_column_len, FullTextColumnLengthReader *column_length_reader) {
    avg_column_len_ = avg_column_len;
    column_length_reader_ = column_length_reader;
    const float smooth_idf = std::log(1.0F + (total_df - doc_freq_ + 0.5F) / (doc_freq_ + 0.5F));
    bm25_common_score_ = weight_ * smooth_idf * (k1 + 1.0F);
    bm25_score_upper_bound_ = bm25_common_score_ / (1.0F + k1 * b / avg_column_len_);
    f1 = k1 * (1.0F - b);
    f2 = k1 * b / avg_column_len_;
    f3 = f2 * std::numeric_limits<u16>::max();
    OStringStream oss;
    oss << "BlockMaxTermDocIterator: " << *column_name_ptr_ << " " << *term_ptr_ << " bm25_common_score: " << bm25_common_score_
        << " bm25_score_upper_bound: " << bm25_score_upper_bound_ << " avg_column_len: " << avg_column_len_ << " f1: " << f1 << " f2: " << f2 << '\n';
    LOG_TRACE(std::move(oss).str());
}

// weight included
float BlockMaxTermDocIterator::BlockMaxBM25Score() {
    ++access_bm_score_cnt_;
    if (const auto last_doc_id = BlockLastDocID(); last_doc_id == block_max_bm25_score_cache_end_id_)[[likely]] {
        return block_max_bm25_score_cache_;
    } else {
        ++calc_bm_score_cnt_;
        block_max_bm25_score_cache_end_id_ = last_doc_id;
        // bm25_common_score_ / (1.0F + k1 * ((1.0F - b) / block_max_tf + b / block_max_percentage / avg_column_len));
        const auto [block_max_tf, block_max_percentage_u16] = GetBlockMaxInfo();
        return block_max_bm25_score_cache_ = bm25_common_score_ / (1.0F + f1 / block_max_tf + f3 / block_max_percentage_u16);
    }
}

Pair<tf_t, u32> BlockMaxTermDocIterator::GetScoreData() { return {iter_.GetCurrentTF(), column_length_reader_->GetColumnLength(doc_id_)}; }

// weight included
float BlockMaxTermDocIterator::BM25Score() {
    ++access_score_cnt_;
    if (doc_id_ == prev_calc_score_doc_id_)[[unlikely]] {
        return bm25_score_cache_;
    }
    calc_score_cnt_++;
    prev_calc_score_doc_id_ = doc_id_;
    // bm25_common_score_ * tf / (tf + k1 * (1.0F - b + b * column_len / avg_column_len));
    const auto [tf, doc_len] = GetScoreData();
    const float p = f1 + f2 * doc_len;
    bm25_score_cache_ = bm25_common_score_ * tf / (tf + p);
    return bm25_score_cache_;
}

Pair<bool, RowID> BlockMaxTermDocIterator::SeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond) {
    const RowID block_last = BlockLastDocID();
    const RowID seek_end = std::min(doc_id_no_beyond, block_last);
    if (doc_id > seek_end) {
        return {false, INVALID_ROWID};
    }
    ++seek_cnt_;
    doc_id = iter_.SeekDoc(doc_id);
    // always update inner doc_id_
    doc_id_ = doc_id;
    assert((doc_id <= block_last));
    if (doc_id > seek_end) {
        return {false, INVALID_ROWID};
    }
    return {true, doc_id};
}

Tuple<bool, float, RowID> BlockMaxTermDocIterator::SeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond, float threshold) {
    if (threshold > BlockMaxBM25Score()) [[unlikely]] {
        return {false, 0.0F, INVALID_ROWID};
    }
    const RowID block_last = BlockLastDocID();
    const RowID seek_end = std::min(doc_id_no_beyond, block_last);
    while (doc_id <= seek_end) {
        ++seek_cnt_;
        doc_id = iter_.SeekDoc(doc_id);
        // always update inner doc_id_
        doc_id_ = doc_id;
        assert((doc_id <= block_last));
        if (doc_id > seek_end) {
            return {false, 0.0F, INVALID_ROWID};
        }
        if (const float score = BM25Score(); score >= threshold) {
            return {true, score, doc_id};
        }
        ++doc_id;
    }
    return {false, 0.0F, INVALID_ROWID};
}

Pair<bool, RowID> BlockMaxTermDocIterator::PeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond) {
    const RowID seek_end = std::min(doc_id_no_beyond, BlockLastDocID());
    if (doc_id > seek_end) {
        return {false, INVALID_ROWID};
    }
    // check cache
    if (peek_doc_id_range_start_ <= doc_id) {
        if (const RowID peek_cache = peek_doc_id_val_; peek_cache >= doc_id) {
            if (peek_cache <= seek_end) {
                return {true, peek_cache};
            }
            if (peek_doc_id_range_end_ >= seek_end) {
                return {false, INVALID_ROWID};
            }
        }
    }
    // need to decode
    ++peek_cnt_;
    Pair<bool, RowID> result = iter_.PeekInBlockRange(doc_id, seek_end);
    // update cache
    peek_doc_id_range_start_ = doc_id;
    peek_doc_id_range_end_ = seek_end;
    peek_doc_id_val_ = result.second;
    return result;
}

bool BlockMaxTermDocIterator::NotPartCheckExist(RowID doc_id) {
    const RowID seek_result = iter_.SeekDoc(doc_id);
    doc_id_ = seek_result;
    return seek_result == doc_id;
}

void BlockMaxTermDocIterator::PrintTree(std::ostream &os, const String &prefix, bool is_final) const {
    os << prefix;
    os << (is_final ? "└──" : "├──");
    os << "BlockMaxTermDocIterator";
    os << " (weight: " << weight_ << ")";
    os << " (column: " << *column_name_ptr_ << ")";
    os << " (term: " << *term_ptr_ << ")";
    os << " (doc_freq: " << DocFreq() << ")";
    os << " (bm25_score_upper_bound: " << BM25ScoreUpperBound() << ")";
    os << " (threshold: " << threshold_ << ")";
    os << '\n';
}

} // namespace infinity