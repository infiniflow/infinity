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
module blockmax_term_doc_iterator;

import stl;
import index_defines;
import internal_types;
import memory_pool;
import segment_posting;
import posting_iterator;
import column_length_io;
import infinity_exception;

namespace infinity {
BlockMaxTermDocIterator::BlockMaxTermDocIterator(optionflag_t flag, MemoryPool *session_pool) : iter_(flag, session_pool) {}

bool BlockMaxTermDocIterator::InitPostingIterator(SharedPtr<Vector<SegmentPosting>> seg_postings, const u32 state_pool_size) {
    if (iter_.Init(std::move(seg_postings), state_pool_size)) {
        doc_freq_ = iter_.GetDocFreq();
        return true;
    }
    UnrecoverableError("Unexpected case: Init PostingIterator failed");
    return false;
}

bool BlockMaxTermDocIterator::BlockSkipTo(RowID doc_id, float threshold) {
    if (threshold > BM25ScoreUpperBound()) [[unlikely]] {
        return false;
    }
    while (true) {
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
    float smooth_idf = std::log(1.0F + (total_df - doc_freq_ + 0.5F) / (doc_freq_ + 0.5F));
    bm25_common_score_ = weight_ * smooth_idf * (k1 + 1.0F);
    bm25_score_upper_bound_ = bm25_common_score_ / (1.0F + k1 * b / avg_column_len_);
}

// weight included
float BlockMaxTermDocIterator::BlockMaxBM25Score() {
    if (auto last_doc_id = BlockLastDocID(); last_doc_id == block_max_bm25_score_cache_end_id_) {
        return block_max_bm25_score_cache_;
    } else {
        block_max_bm25_score_cache_end_id_ = last_doc_id;
        // bm25_common_score_ / (1.0F + k1 * ((1.0F - b) / block_max_tf + b / block_max_percentage / avg_column_len));
        auto [block_max_tf, block_max_percentage_u16] = GetBlockMaxInfo();
        return block_max_bm25_score_cache_ =
                   bm25_common_score_ /
                   (1.0F + k1 * ((1.0F - b) / block_max_tf + b * std::numeric_limits<u16>::max() / (block_max_percentage_u16 * avg_column_len_)));
    }
}

// weight included
float BlockMaxTermDocIterator::BM25Score() {
    // bm25_common_score_ * tf / (tf + k1 * (1.0F - b + b * column_len / avg_column_len));
    auto tf = iter_.GetCurrentTF();
    auto doc_len = column_length_reader_->GetColumnLength(doc_id_);
    return bm25_common_score_ * tf / (tf + k1 * (1.0F - b + b * doc_len / avg_column_len_));
}

Tuple<bool, float, RowID> BlockMaxTermDocIterator::SeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond, float threshold) {
    if (threshold > BlockMaxBM25Score()) [[unlikely]] {
        return {false, 0.0F, INVALID_ROWID};
    }
    RowID seek_end = std::min(doc_id_no_beyond, BlockLastDocID());
    while (true) {
        doc_id = iter_.SeekDoc(doc_id);
        // always update inner doc_id_
        doc_id_ = doc_id;
        if (doc_id > seek_end) {
            return {false, 0.0F, INVALID_ROWID};
        }
        if (const float score = BM25Score(); score >= threshold) {
            return {true, score, doc_id};
        }
        ++doc_id;
    }
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
    Pair<bool, RowID> result = iter_.PeekInBlockRange(doc_id, seek_end);
    // update cache
    peek_doc_id_range_start_ = doc_id;
    peek_doc_id_range_end_ = seek_end;
    peek_doc_id_val_ = result.second;
    return result;
}

bool BlockMaxTermDocIterator::Seek(RowID doc_id) {
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
    os << '\n';
}

} // namespace infinity