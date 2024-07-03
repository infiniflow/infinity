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
import logger;

namespace infinity {

bool BlockMaxPhraseDocIterator::NextShallow(RowID doc_id){
    assert(doc_id != INVALID_ROWID);
    for (auto &it : pos_iters_) {
        bool ok = it->SkipTo(doc_id);
        if(!ok){
            return false;
        }
    }
    return true;
}

bool BlockMaxPhraseDocIterator::Next(RowID doc_id){
    assert(doc_id != INVALID_ROWID);
    assert(doc_id_ == INVALID_ROWID || doc_id_ < doc_id);
    RowID target_doc_id = doc_id;
    do {
        for (auto &it : pos_iters_) {
            target_doc_id = it->SeekDoc(target_doc_id);
        }
        if (target_doc_id == INVALID_ROWID) {
            doc_id_ = INVALID_ROWID;
            return false;
        }
        if (target_doc_id == pos_iters_[0]->DocID()) {
            doc_id_ = target_doc_id;
            PhraseColumnMatchData phrase_match_data;
            if (GetPhraseMatchData(phrase_match_data, target_doc_id)) {
                current_phrase_freq_ = phrase_match_data.tf_;
                const float score = BM25Score();
                if (score > threshold_) {
                    return true;
                }
            }
            target_doc_id++;
        }
    } while (1);
}

bool BlockMaxPhraseDocIterator::BlockSkipTo(RowID doc_id, float threshold) {
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
        doc_id = max_doc_id;
    }
    doc_id_ = doc_id;
}

bool BlockMaxPhraseDocIterator::GetExactPhraseMatchData(PhraseColumnMatchData &match_data, RowID doc_id) {
    pos_t beg_position0 = 0;
    pos_t now_position0 = 0;
    while (true) {
        pos_iters_[0]->SeekPosition(beg_position0, now_position0);
        if (now_position0 == INVALID_POSITION) {
            break;
        }
        beg_position0 = now_position0 + 1;
        bool found = true;
        for (SizeT i = 1; i < pos_iters_.size(); ++i) {
            auto &iter = pos_iters_[i];
            pos_t beg_position = now_position0 + i;
            pos_t now_position = beg_position;
            iter->SeekPosition(beg_position, now_position);
            if (now_position != beg_position) {
                found = false;
                break;
            }
        }
        if (found) {
            match_data.begin_positions_.push_back(now_position0);
        }
    }
    if (match_data.begin_positions_.empty()) {
        return false;
    }
    doc_freq_++;
    match_data.tf_ = match_data.begin_positions_.size();
    phrase_freq_ += match_data.tf_;
    return true;
}

bool BlockMaxPhraseDocIterator::GetSloppyPhraseMatchData(PhraseColumnMatchData &match_data, RowID doc_id) {
    Vector<Vector<pos_t>> positions;
    for (auto &pos_iter : pos_iters_) {
        Vector<pos_t> pos_vec;
        pos_t beg_position = 0;
        pos_t position = INVALID_POSITION;
        while (true) {
            pos_iter->SeekPosition(beg_position, position);
            if (position == INVALID_POSITION) {
                break;
            }
            pos_vec.push_back(position);
            beg_position = position + 1;
        }
        assert(!pos_vec.empty());
        positions.push_back(std::move(pos_vec));
    }
    /*
    Refers to: https://lucene.apache.org/core/9_11_0/core/org/apache/lucene/search/SloppyPhraseMatcher.html

    pos_i : term i's position in phrase
    term_pos_i : term i's current position in document
    phrase_pos_i: term_pos_i - pos_i

    For a solution (term_pos_0, term_pos_1, ..., term_pos_n), it's acceptable iff:
    for any i, j (0<=i<=n, 0<=j<=n), |phrase_pos_i - phrase_pos_j| <= slop

    For an acceptable solution, its matchLength is:
    max(|phrase_pos_i - phrase_pos_j|) for all i, j (0<=i<=n, 0<=j<=n)

    For an acceptable solution, its sloppyWeight is:
    1f / (1f + matchLength)

    For a phrase, its freq is sum of every acceptable solution's sloppyWeight.
    */
    struct StepState {
        SizeT pos_i;
        SizeT term_pos_idx; // index to positions[pos_i]
        int min_phrase_pos;
        int max_phrase_pos;
    };
    struct Solution {
        Vector<StepState> steps;
        u32 matchLength;
    };
    Vector<StepState> steps = {{0, 0, int(positions[0][0]), int(positions[0][0])}};
    Vector<Solution> solutions;
    auto CalibrateLastStep = [this, &positions, &steps] {
        StepState &step = steps.back();
        if (step.term_pos_idx >= positions[step.pos_i].size())
            return false;
        int phrase_pos_i = int(positions[step.pos_i][step.term_pos_idx]) - int(step.pos_i);
        if (step.pos_i == 0) {
            step.min_phrase_pos = phrase_pos_i;
            step.max_phrase_pos = phrase_pos_i;
        } else {
            StepState &prev_step = steps[steps.size() - 2];
            step.min_phrase_pos = std::min(phrase_pos_i, prev_step.min_phrase_pos);
            step.max_phrase_pos = std::max(phrase_pos_i, prev_step.max_phrase_pos);
            if (step.max_phrase_pos - step.min_phrase_pos > int(this->slop_)) {
                return false;
            }
        }
        return true;
    };

    while (!steps.empty()) {
        bool ok = CalibrateLastStep();
        if (ok) {
            if (steps.size() == positions.size()) {
                // got an acceptable solution
                StepState &step = steps.back();
                solutions.emplace_back(steps, u32(step.max_phrase_pos - step.min_phrase_pos));
                // goto next possible step
                steps.back().term_pos_idx++;
            } else {
                // advance a step
                assert(!steps.empty());
                steps.emplace_back(steps.size(), 0, 0, 0);
                StepState &step = steps.back();
                StepState &prev_step = steps[steps.size() - 2];
                // bisect to locate the fist step.term_pos_idx which satisfies:
                // int(positions[step.pos_i][step.term_pos_idx]) - int(step.pos_i) >= prev_step.min_phrase_pos
                auto lower =
                    std::lower_bound(positions[step.pos_i].begin(), positions[step.pos_i].end(), prev_step.min_phrase_pos - this->slop_ + step.pos_i);
                step.term_pos_idx = std::distance(positions[step.pos_i].begin(), lower);
            }
        } else {
            // goto next possible step
            steps.pop_back();
            if (steps.empty())
                break;
            if (steps.size() != positions.size()) {
                assert(steps.back().term_pos_idx < positions[steps.back().pos_i].size());
                steps.back().term_pos_idx++;
            }
        }
    }
    match_data.tf_ = 0.0F;
    for (auto &solution : solutions) {
        match_data.tf_ += 1.0F / (1.0F + solution.matchLength);
    }
    if (SHOULD_LOG_DEBUG()) {
        std::ostringstream oss;
        oss << "Phrase \"" << terms_ptr_->at(0);
        for (SizeT i = 1; i < terms_ptr_->size(); i++) {
            oss << " " << terms_ptr_->at(i);
        }
        oss << "\"~" << slop_ << " has " << solutions.size() << " sulotions:" << std::endl;
        for (SizeT i = 0; i < solutions.size(); i++) {
            oss << "solution " << i << ": ";
            Solution &solution = solutions[i];
            for (SizeT j = 0; j < solution.steps.size(); j++) {
                oss << "(" << solution.steps[j].pos_i << "," << solution.steps[j].term_pos_idx << "," << solution.steps[j].min_phrase_pos << ","
                    << solution.steps[j].max_phrase_pos << ") ";
            }
            oss << std::endl;
        }
        LOG_DEBUG(oss.str());
    }
    if (!solutions.empty()) {
        phrase_freq_ += match_data.tf_;
        current_phrase_freq_ = match_data.tf_;
        return true;
    }
    return false;
}

Tuple<bool, float, RowID> BlockMaxPhraseDocIterator::SeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond, float threshold) {
    if (threshold > BlockMaxBM25Score()) [[unlikely]] {
        return {false, 0.0F, INVALID_ROWID};
    }
    const RowID block_last = BlockLastDocID();
    const RowID seek_end = std::min(doc_id_no_beyond, block_last);
    while (doc_id <= seek_end) {
        SeekDoc(doc_id, seek_end);
        doc_id = doc_id_;
        // assert((doc_id <= block_last));
        if (doc_id > seek_end) {
            return {false, 0.0F, INVALID_ROWID};
        }
        PhraseColumnMatchData phrase_match_data;
        if (GetPhraseMatchData(phrase_match_data, doc_id)) {
            current_phrase_freq_ = phrase_match_data.tf_;
            if (const float score = BM25Score(); score >= threshold) {
                return {true, score, doc_id};
            }
        }
        ++doc_id;
    }
    return {false, 0.0F, INVALID_ROWID};
}

Pair<bool, RowID> BlockMaxPhraseDocIterator::SeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond) {
    const RowID block_last = BlockLastDocID();
    const RowID seek_end = std::min(doc_id_no_beyond, block_last);
    if (doc_id > seek_end) {
        return {false, INVALID_ROWID};
    }
    SeekDoc(doc_id, seek_end);
    doc_id = doc_id_;
    if (doc_id > seek_end) {
        return {false, INVALID_ROWID};
    }
    PhraseColumnMatchData phrase_match_data;
    if (GetPhraseMatchData(phrase_match_data, doc_id)) {
        current_phrase_freq_ = phrase_match_data.tf_;
        return {true, doc_id};
    }
    return {false, INVALID_ROWID};
}

Pair<bool, RowID> BlockMaxPhraseDocIterator::PeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond) {
    return TermPeekInBlockRange(0, doc_id, doc_id_no_beyond);
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
    os << " (phrase:";
    for (auto term : *terms_ptr_) {
        os << " " << term;
    }
    os << ")";
    os << " (doc_freq: " << DocFreq() << ")";
    os << " (bm25_score_upper_bound: " << BM25ScoreUpperBound() << ")";
    os << " (threshold: " << threshold_ << ")";
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
    auto tf = current_phrase_freq_;
    auto doc_len = column_length_reader_->GetColumnLength(doc_id_);
    return bm25_common_score_ * tf / (tf + k1 * (1.0F - b + b * doc_len / avg_column_len_));
}

float BlockMaxPhraseDocIterator::BlockMaxBM25Score() {
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
