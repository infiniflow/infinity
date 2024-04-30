module;

#include <iostream>
#include <cassert>
#include <cmath>

module blockmax_phrase_doc_iterator;

import stl;
import index_defines;
import internal_types;
import early_terminate_iterator;
import phrase_doc_iterator;
import blockmax_term_doc_iterator;
import column_length_io;
import third_party;

namespace infinity {

bool BlockMaxPhraseDocIterator::BlockSkipTo(RowID doc_id, float threshold) {
//    for (auto &iter : term_doc_iters_) {
//        if (!iter->BlockSkipTo(doc_id, threshold)) {
//            return false;
//        }
//    }
//    return true;
    fmt::print("Phrase BlockSkipTo doc_id = {}, threshold = {}, BM25ScoreUpperBound = {}\n", doc_id.ToUint64(), threshold, BM25ScoreUpperBound());
    if (threshold > BM25ScoreUpperBound()) [[unlikely]] {
        return false;
    }
    while (true) {
        for (u32 i = 0; i < pos_iters_.size(); ++i) {
            const auto &iter = pos_iters_[i];
            if (!iter->SkipTo(doc_id)) {
                doc_id_ = INVALID_ROWID;
                return false;
            }
            term_doc_id_[i] = doc_id;
        }
        if (BlockMaxBM25Score() >= threshold) {
            return true;
        }
        doc_id = BlockLastDocID() + 1;
    }
}

RowID BlockMaxPhraseDocIterator::BlockMinPossibleDocID() const { return pos_iters_[0]->BlockLowestPossibleDocID(); }

RowID BlockMaxPhraseDocIterator::BlockLastDocID() const { return TermBlockLastDocID(0); }


void BlockMaxPhraseDocIterator::SeekDoc(RowID doc_id, RowID seek_end) {
    assert(pos_iters_.size() > 0);
    bool need_loop = true;
    fmt::print("SeekDoc, doc_id = {}, seek_end = {}\n", doc_id.ToUint64(), seek_end.ToUint64());
    while (need_loop) {
        if (doc_id == INVALID_ROWID || doc_id > seek_end) {
            break;
        }
        RowID max_doc_id = pos_iters_[0]->SeekDoc(doc_id);
        term_doc_id_[0] = max_doc_id;
        need_loop = false;
        for (SizeT i = 1; i < pos_iters_.size(); ++i) {
            auto& iter = pos_iters_[i];
            auto tmp_doc_id = iter->SeekDoc(doc_id);
            term_doc_id_[i] = tmp_doc_id;
            if (max_doc_id != tmp_doc_id) {
                max_doc_id = std::max(max_doc_id, tmp_doc_id);
                need_loop = true;
                break;
            }
        }
        fmt::print("loop, max_doc_id = {}\n", max_doc_id.ToUint64());
        doc_id = max_doc_id;
    }
    doc_id_ = doc_id;
}

bool BlockMaxPhraseDocIterator::CheckBeginPosition(pos_t position) {
    pos_t now_position = position;
    for (SizeT i = 1; i < pos_iters_.size(); ++i) {
        auto& iter = pos_iters_[i];
        pos_t next_position = 0;
        fmt::print("iter:{}, has position: {}\n", i, iter->HasPosition());
        fmt::print("iter:{}, seek position = {}\n", i, now_position);
        iter->SeekPosition(now_position, next_position);
        fmt::print("iter:{}, last position = {}, find postion = {}\n", i, now_position, next_position);
        if (next_position != now_position + 1) {
            return false;
        }
        now_position = next_position;
    }
    return true;
}

bool BlockMaxPhraseDocIterator::GetPhraseMatchData(PhraseColumnMatchData &match_data, RowID doc_id) {
    if (doc_id != doc_id_) {
        return false;
    }
    auto& iter = pos_iters_[0];
    pos_t beg_position = 0;
    match_data.tf_ = 0;
    while (true) {
        pos_t position = INVALID_POSITION;
        fmt::print("first iter, doc_id = {}, has position: {}\n", doc_id.ToUint64(), iter->HasPosition());
        iter->SeekPosition(beg_position, position);
        fmt::print("first iter, doc_id = {}, begin position = {}, find position = {}\n", doc_id.ToUint64(), beg_position, position);
        if (position == INVALID_POSITION) {
            break;
        }
        if (CheckBeginPosition(position)) {
            match_data.begin_positions_.push_back(position);
            match_data.tf_ += 1;
        }
        beg_position = position + 1;
    }
    match_data.doc_id_ = doc_id_;
    if (match_data.begin_positions_.size()) {
        for (SizeT i = 0; i < pos_iters_.size(); ++i) {
            auto& iter_temp = pos_iters_[i];
            match_data.all_tf_.emplace_back(iter_temp->GetCurrentTF());
            match_data.all_doc_payload_.emplace_back(iter_temp->GetCurrentDocPayload());
        }
        if (all_doc_ids_.count(doc_id_) == 0) {
            all_doc_ids_.insert(doc_id_);
            doc_freq_++;
            phrase_freq_ += match_data.begin_positions_.size();
        }
        fmt::print("query res: doc_id = {}, phrase_freq = {}\n", doc_id_.ToUint64(), match_data.begin_positions_.size());
        return true;
    }
    return false;
}

Tuple<bool, float, RowID> BlockMaxPhraseDocIterator::SeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond, float threshold) {
    fmt::print("SeekInBlockRange, doc_id = {}, threshold = {}, block_max_bm25score = {}\n", doc_id.ToUint64(), threshold, BlockMaxBM25Score());
    if (threshold > BlockMaxBM25Score()) [[unlikely]] {
        return {false, 0.0F, INVALID_ROWID};
    }
    const RowID block_last = BlockLastDocID();
    const RowID seek_end = std::min(doc_id_no_beyond, block_last);
    fmt::print("doc_id = {}, seek_end = {}\n", doc_id.ToUint64(), seek_end.ToUint64());
    while (doc_id <= seek_end) {
        SeekDoc(doc_id, seek_end);
        doc_id = doc_id_;
        fmt::print("doc_id: {}, block_last: {}\n", doc_id.ToUint64(), block_last.ToUint64());

        // assert((doc_id <= block_last));
        if (doc_id > seek_end) {
            return {false, 0.0F, INVALID_ROWID};
        }
        PhraseColumnMatchData phrase_match_data;
        if (GetPhraseMatchData(phrase_match_data, doc_id)) {
            current_phrase_freq_ = phrase_match_data.tf_;
            fmt::print("doc_id = {}, current_phrase_freq_: {}\n", doc_id.ToUint64(), current_phrase_freq_);
            if (const float score = BM25Score(); score >= threshold) {
                return {true, score, doc_id};
            }
        }
        ++doc_id;
    }
    return {false, 0.0F, INVALID_ROWID};
//    Vector<Tuple<bool, float, RowID>> all_res;
//    for (SizeT i = 0; i < pos_iters_.size(); ++i) {
//        auto res = TermSeekInBlockRange(i, doc_id, doc_id_no_beyond, threshold);
//        if (!std::get<0>(res)) {
//            return {false, 0.0F, INVALID_ROWID};
//        }
//        all_res.push_back(res);
//    }
//    if (all_res.empty()) {
//        return {false, 0.0F, INVALID_ROWID};
//    }
//    return all_res[0];
}

Pair<bool, RowID> BlockMaxPhraseDocIterator::PeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond) {
    Vector<Pair<bool, RowID>> all_res;
    for (SizeT i = 0; i < pos_iters_.size(); ++i) {
        auto res = TermPeekInBlockRange(i, doc_id, doc_id_no_beyond);
        if (!std::get<0>(res)) {
            return {false, INVALID_ROWID};
        }
        all_res.push_back(res);
    }
    if (all_res.empty()) {
        return {false, INVALID_ROWID};
    }
    return all_res[0];
}

bool BlockMaxPhraseDocIterator::NotPartCheckExist(RowID doc_id) {
    for (SizeT i = 0; i < pos_iters_.size(); ++i) {
        if (!TermNotPartCheckExist(i, doc_id)) {
            return false;
        }
    }
    return true;
}

void BlockMaxPhraseDocIterator::PrintTree(std::ostream &os, const String &prefix, bool is_final) const {
    os << prefix;
    os << (is_final ? "└──" : "├──");
    os << "BlockMaxPhraseDocIterator";
    os << " (weight: " << weight_ << ")";
    os << " (column: " << *column_name_ptr_ << ")";
    os << " (phrase: ";
    for (auto term : *terms_ptr_) {
        os << term << " ";
    }
    os << ")";
    os << " (doc_freq: " << DocFreq() << ")";
    os << " (bm25_score_upper_bound: " << BM25ScoreUpperBound() << ")";
    os << '\n';
}

// BM25 parameters
constexpr float k1 = 1.2F;
constexpr float b = 0.75F;

void BlockMaxPhraseDocIterator::InitBM25Info(u64 total_df, float avg_column_len, FullTextColumnLengthReader *column_length_reader) {
    avg_column_len_ = avg_column_len;

    column_length_reader_ = column_length_reader;
    float smooth_idf = std::log(1.0F + (total_df - estimate_doc_freq_ + 0.5F) / (estimate_doc_freq_ + 0.5F));
    bm25_common_score_ = weight_ * smooth_idf * (k1 + 1.0F);
    bm25_score_upper_bound_ = bm25_common_score_ / (1.0F + k1 * b / avg_column_len);

    for (SizeT i = 0; i < pos_iters_.size(); ++i) {
        TermInitBM25Info(i, total_df, avg_column_len, column_length_reader);
    }
}

float BlockMaxPhraseDocIterator::BM25Score() {
    //    float bm25_score = 0;
    //    for (SizeT i = 0; i < pos_iters_.size(); ++i) {
    //        bm25_score += TermBM25Score(i, phrase_freq_);
    //    }
    //    return bm25_score;
    auto tf = current_phrase_freq_;
    auto doc_len = column_length_reader_->GetColumnLength(doc_id_);
    return bm25_common_score_ * tf / (tf + k1 * (1.0F - b + b * doc_len / avg_column_len_));
}

float BlockMaxPhraseDocIterator::BlockMaxBM25Score() {
//    float bm25_score = 0;
//    for (SizeT i = 0; i < pos_iters_.size(); ++i) {
//        bm25_score += TermBlockMaxBM25Score(i);
//    }
//    return bm25_score;

    if (auto last_doc_id = BlockLastDocID(); last_doc_id != block_max_bm25_score_cache_end_id_) {
        block_max_bm25_score_cache_end_id_ = last_doc_id;
        // bm25_common_score_ / (1.0F + k1 * ((1.0F - b) / block_max_tf + b / block_max_percentage / avg_column_len));
        auto [block_max_tf, block_max_percentage_u16] = GetBlockMaxInfo();
        block_max_bm25_score_cache_ =
                   bm25_common_score_ /
                   (1.0F + k1 * ((1.0F - b) / block_max_tf + b * std::numeric_limits<u16>::max() / (block_max_percentage_u16 * avg_column_len_)));
    }

    for (SizeT i = 0; i < pos_iters_.size(); ++i) {
        TermBlockMaxBM25Score(i);
    }

    return block_max_bm25_score_cache_;
}

// for term block operator
float BlockMaxPhraseDocIterator::TermBlockMaxBM25Score(u32 term_id) {
    if (auto last_doc_id = TermBlockLastDocID(term_id); last_doc_id == term_block_max_bm25_score_cache_end_id_[term_id]) {
        return term_block_max_bm25_score_cache_[term_id];
    } else {
        term_block_max_bm25_score_cache_end_id_[term_id] = last_doc_id;
        // bm25_common_score_ / (1.0F + k1 * ((1.0F - b) / block_max_tf + b / block_max_percentage / avg_column_len));
        auto [block_max_tf, block_max_percentage_u16] = TermGetBlockMaxInfo(term_id);
        return term_block_max_bm25_score_cache_[term_id] =
                   term_bm25_common_score_[term_id] /
                   (1.0F + k1 * ((1.0F - b) / block_max_tf + b * std::numeric_limits<u16>::max() / (block_max_percentage_u16 * avg_column_len_)));
    }
}

Pair<bool, RowID> BlockMaxPhraseDocIterator::TermPeekInBlockRange(u32 term_id, RowID doc_id, RowID doc_id_no_beyond) {
    const RowID seek_end = std::min(doc_id_no_beyond, TermBlockLastDocID(term_id));
    if (doc_id > seek_end) {
        return {false, INVALID_ROWID};
    }
    // check cache
    if (term_peek_doc_id_range_start_[term_id] <= doc_id) {
        if (const RowID peek_cache = term_peek_doc_id_val_[term_id]; peek_cache >= doc_id) {
            if (peek_cache <= seek_end) {
                return {true, peek_cache};
            }
            if (term_peek_doc_id_range_end_[term_id] >= seek_end) {
                return {false, INVALID_ROWID};
            }
        }
    }
    // need to decode
    Pair<bool, RowID> result = pos_iters_[term_id]->PeekInBlockRange(doc_id, seek_end);
    // update cache
    term_peek_doc_id_range_start_[term_id] = doc_id;
    term_peek_doc_id_range_end_[term_id] = seek_end;
    term_peek_doc_id_val_[term_id] = result.second;
    return result;
}

Tuple<bool, float, RowID> BlockMaxPhraseDocIterator::TermSeekInBlockRange(u32 term_id, RowID doc_id, RowID doc_id_no_beyond, float threshold) {
    if (threshold > TermBlockMaxBM25Score(term_id)) [[unlikely]] {
        return {false, 0.0F, INVALID_ROWID};
    }
    const RowID block_last = TermBlockLastDocID(term_id);
    const RowID seek_end = std::min(doc_id_no_beyond, block_last);
    while (doc_id <= seek_end) {
        doc_id = pos_iters_[term_id]->SeekDoc(doc_id);
        term_doc_id_[term_id] = doc_id;
        assert((doc_id <= block_last));
        if (doc_id > seek_end) {
            return {false, 0.0F, INVALID_ROWID};
        }
        if (const float score = TermBM25Score(term_id); score >= threshold) {
            return {true, score, doc_id};
        }
        ++doc_id;
    }
    return {false, 0.0F, INVALID_ROWID};
}

float BlockMaxPhraseDocIterator::TermBM25Score(u32 term_id) {
    // bm25_common_score_ * tf / (tf + k1 * (1.0F - b + b * column_len / avg_column_len));
    auto tf = pos_iters_[term_id]->GetCurrentTF();
    auto doc_len = term_column_length_reader_[term_id]->GetColumnLength(term_doc_id_[term_id]);
    return term_bm25_common_score_[term_id] * tf / (tf + k1 * (1.0F - b + b * doc_len / avg_column_len_));
}

float BlockMaxPhraseDocIterator::TermBM25Score(infinity::u32 term_id, infinity::tf_t phrase_freq) {
    // bm25_common_score_ * tf / (tf + k1 * (1.0F - b + b * column_len / avg_column_len));
    auto tf = phrase_freq;
    auto doc_len = term_column_length_reader_[term_id]->GetColumnLength(term_doc_id_[term_id]);
    return term_bm25_common_score_[term_id] * tf / (tf + k1 * (1.0F - b + b * doc_len / avg_column_len_));
}

void BlockMaxPhraseDocIterator::TermInitBM25Info(u32 term_id, u64 total_df, float avg_column_len, FullTextColumnLengthReader *column_length_reader) {
    term_column_length_reader_[term_id] = column_length_reader;
    float smooth_idf = std::log(1.0F + (total_df - estimate_doc_freq_ + 0.5F) / (estimate_doc_freq_ + 0.5F));
    term_bm25_common_score_[term_id] = weight_ * smooth_idf * (k1 + 1.0F);
    term_bm25_score_upper_bound_[term_id] = term_bm25_common_score_[term_id] / (1.0F + k1 * b / avg_column_len);
    bm25_score_upper_bound_ = term_bm25_score_upper_bound_[0];
}

bool BlockMaxPhraseDocIterator::TermNotPartCheckExist(u32 term_id, RowID doc_id) {
    const RowID seek_result = pos_iters_[term_id]->SeekDoc(doc_id);
    term_doc_id_[term_id] = seek_result;
    return seek_result == doc_id;
}

RowID BlockMaxPhraseDocIterator::TermBlockLastDocID(u32 term_id) const {
    return pos_iters_[term_id]->BlockLastDocID();
}

} // namespace infinity
