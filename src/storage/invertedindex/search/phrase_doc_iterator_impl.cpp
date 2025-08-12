module;

#include <cassert>

module infinity_core:phrase_doc_iterator.impl;

import :phrase_doc_iterator;
import :stl;
import :doc_iterator;
import :posting_iterator;
import :column_length_io;
import :logger;

import std;
import std.compat;
import third_party;

import internal_types;

namespace infinity {

PhraseDocIterator::PhraseDocIterator(Vector<UniquePtr<PostingIterator>> &&iters,
                                     const float weight,
                                     const u32 slop,
                                     const FulltextSimilarity ft_similarity)
    : pos_iters_(std::move(iters)), weight_(weight), slop_(slop), ft_similarity_(ft_similarity) {
    doc_freq_ = 0;
    phrase_freq_ = 0;
    if (pos_iters_.size()) {
        estimate_doc_freq_ = pos_iters_[0]->GetDocFreq();
    } else {
        estimate_doc_freq_ = 0;
    }
    for (SizeT i = 0; i < pos_iters_.size(); ++i) {
        estimate_doc_freq_ = std::min(estimate_doc_freq_, pos_iters_[i]->GetDocFreq());
    }
    estimate_iterate_cost_ = {1, estimate_doc_freq_};
    block_max_bm25_score_cache_part_info_end_ids_.resize(pos_iters_.size(), INVALID_ROWID);
    block_max_bm25_score_cache_part_info_vals_.resize(pos_iters_.size());
}

void PhraseDocIterator::InitBM25Info(UniquePtr<FullTextColumnLengthReader> &&column_length_reader, const float delta, const float k1, const float b) {
    column_length_reader_ = std::move(column_length_reader);
    const u64 total_df = column_length_reader_->GetTotalDF();
    const float avg_column_len = column_length_reader_->GetAvgColumnLength();
    float total_idf = 0.0f;
    for (const auto &iter : pos_iters_) {
        const auto doc_freq = iter->GetDocFreq();
        total_idf += std::log1p((total_df - doc_freq + 0.5F) / (doc_freq + 0.5F));
    }
    bm25_common_score_ = weight_ * total_idf * (k1 + 1.0F);
    bm25_score_upper_bound_ = bm25_common_score_ * (avg_column_len / (avg_column_len + k1 * b) + delta / (k1 + 1.0F));
    f1 = k1 * (1.0F - b);
    f2 = k1 * b / avg_column_len;
    f3 = f2 * std::numeric_limits<u16>::max();
    f4 = delta / (k1 + 1.0F);
    if (SHOULD_LOG_TRACE()) {
        OStringStream oss;
        oss << "PhraseDocIterator: ";
        if (column_name_ptr_ != nullptr) {
            oss << "column: " << *column_name_ptr_ << ",";
        }
        if (terms_ptr_ != nullptr) {
            oss << " phrase:";
            for (auto term : *terms_ptr_)
                oss << " " << term;
            oss << std::endl;
        }
        oss << "total_df: " << total_df << ", avg_column_len: " << avg_column_len << ", bm25_common_score: " << bm25_common_score_
            << ", bm25_score_upper_bound: " << bm25_score_upper_bound_;
        LOG_TRACE(std::move(oss).str());
    }
}

bool PhraseDocIterator::Next(const RowID doc_id) {
    assert(doc_id != INVALID_ROWID);
    if (doc_id_ != INVALID_ROWID && doc_id_ >= doc_id)
        return true;
    assert(pos_iters_.size() > 0);
    RowID target_doc_id = doc_id;
    while (true) {
        for (const auto &it : pos_iters_) {
            target_doc_id = it->SeekDoc(target_doc_id);
            if (target_doc_id == INVALID_ROWID) {
                doc_id_ = INVALID_ROWID;
                return false;
            }
        }
        if (target_doc_id == pos_iters_[0]->DocID()) {
            bool found = GetPhraseMatchData();
            if (found && (threshold_ <= 0.0f || BM25Score() > threshold_)) {
                doc_id_ = target_doc_id;
                UpdateBlockRangeDocID();
                return true;
            }
            ++target_doc_id;
        }
    }
}

void PhraseDocIterator::UpdateBlockRangeDocID() {
    RowID min_doc_id = 0;
    RowID max_doc_id = INVALID_ROWID;
    for (const auto &it : pos_iters_) {
        min_doc_id = std::max(min_doc_id, it->BlockLowestPossibleDocID());
        max_doc_id = std::min(max_doc_id, it->BlockLastDocID());
    }
    block_min_possible_doc_id_ = min_doc_id;
    block_last_doc_id_ = max_doc_id;
}

float PhraseDocIterator::BlockMaxBM25Score() {
    if (const auto last_doc_id = BlockLastDocID(); last_doc_id != block_max_bm25_score_cache_end_id_) {
        block_max_bm25_score_cache_end_id_ = last_doc_id;
        // bm25_common_score_ / (1.0F + k1 * ((1.0F - b) / block_max_tf + b / block_max_percentage / avg_column_len));
        // block_max_bm25_score_cache_ = bm25_common_score_ / (1.0F + f1 / block_max_tf + f3 / block_max_percentage_u16);
        float div_add_min = std::numeric_limits<float>::max();
        for (SizeT i = 0; i < pos_iters_.size(); ++i) {
            const auto *iter = pos_iters_[i].get();
            float current_div_add_min = {};
            if (const auto iter_block_last_doc_id = iter->BlockLastDocID();
                iter_block_last_doc_id == block_max_bm25_score_cache_part_info_end_ids_[i]) {
                current_div_add_min = block_max_bm25_score_cache_part_info_vals_[i];
            } else {
                block_max_bm25_score_cache_part_info_end_ids_[i] = iter_block_last_doc_id;
                const auto [block_max_tf, block_max_percentage_u16] = iter->GetBlockMaxInfo();
                current_div_add_min = f1 / block_max_tf + f3 / block_max_percentage_u16;
                block_max_bm25_score_cache_part_info_vals_[i] = current_div_add_min;
            }
            div_add_min = std::min(div_add_min, current_div_add_min);
        }
        block_max_bm25_score_cache_ = bm25_common_score_ * (1.0F / (1.0F + div_add_min) + f4);
    }
    return block_max_bm25_score_cache_;
}

// Move block cursor to ensure its last_doc_id is no less than given doc_id.
// Returns false and update doc_id_ to INVALID_ROWID if the iterator is exhausted.
// Note that this routine decode skip_list only, and doesn't update doc_id_ when returns true.
// Caller may invoke BlockMaxBM25Score() after this routine.
bool PhraseDocIterator::NextShallow(RowID doc_id) {
    if (threshold_ > BM25ScoreUpperBound()) [[unlikely]] {
        doc_id_ = INVALID_ROWID;
        return false;
    }
    while (true) {
        for (const auto &iter : pos_iters_) {
            if (!iter->SkipTo(doc_id)) {
                doc_id_ = INVALID_ROWID;
                return false;
            }
        }
        UpdateBlockRangeDocID();
        if (threshold_ <= 0.0f || BlockMaxBM25Score() > threshold_) {
            return true;
        }
        doc_id = BlockLastDocID() + 1;
    }
}

float PhraseDocIterator::BM25Score() {
    if (doc_id_ == bm25_score_cache_docid_) [[unlikely]] {
        return bm25_score_cache_;
    }
    calc_score_cnt_++;
    bm25_score_cache_docid_ = doc_id_;
    // bm25_common_score_ * tf / (tf + k1 * (1.0F - b + b * column_len / avg_column_len));
    const auto doc_len = column_length_reader_->GetColumnLength(doc_id_);
    const float p = f1 + f2 * doc_len;
    bm25_score_cache_ = bm25_common_score_ * (tf_ / (tf_ + p) + f4);
    return bm25_score_cache_;
}

void PhraseDocIterator::PrintTree(std::ostream &os, const String &prefix, bool is_final) const {
    os << prefix;
    os << (is_final ? "└──" : "├──");
    os << "PhraseDocIterator";
    os << " (weight: " << weight_ << ")";
    os << " (column: " << *column_name_ptr_ << ")";
    os << " (phrase:";
    for (auto term : *terms_ptr_) {
        os << " " << term;
    }
    os << ")";
    os << " (doc_freq: " << GetDocFreq() << ")";
    os << " (bm25_score_upper_bound: " << BM25ScoreUpperBound() << ")";
    os << '\n';
}

bool PhraseDocIterator::GetExactPhraseMatchData() {
    pos_t beg_position0 = 0;
    pos_t now_position0 = 0;
    Vector<pos_t> begin_positions;
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
            begin_positions.push_back(now_position0);
        }
    }
#ifdef INFINITY_DEBUG
    if (SHOULD_LOG_DEBUG()) {
        std::ostringstream oss;
        oss << "Phrase \"" << terms_ptr_->at(0);
        for (SizeT i = 1; i < terms_ptr_->size(); i++) {
            oss << " " << terms_ptr_->at(i);
        }
        oss << "\"~" << slop_ << " has " << begin_positions.size() << " sulotions:" << std::endl;
        for (SizeT i = 0; i < begin_positions.size(); i++) {
            oss << "solution " << i << ": " << begin_positions[0] << std::endl;
        }
        LOG_DEBUG(oss.str());
    }
#endif
    if (begin_positions.empty()) {
        return false;
    }
    doc_freq_++;
    tf_ = begin_positions.size();
    all_tf_.push_back(tf_);
    phrase_freq_ += tf_;
    return true;
}

bool PhraseDocIterator::GetSloppyPhraseMatchData() {
    Vector<Vector<int>> positions;
    for (auto &pos_iter : pos_iters_) {
        Vector<int> pos_vec;
        pos_t beg_position = 0;
        pos_t position = INVALID_POSITION;
        while (true) {
            pos_iter->SeekPosition(beg_position, position);
            if (position == INVALID_POSITION) {
                break;
            }
            pos_vec.push_back(int(position));
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

    For a solution (term_pos_0, term_pos_1, ..., term_pos_n), it's acceptable if:
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
                auto lower = std::lower_bound(positions[step.pos_i].begin(),
                                              positions[step.pos_i].end(),
                                              prev_step.min_phrase_pos - int(this->slop_) + int(step.pos_i));
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
    tf_ = 0.0f;
    for (auto &solution : solutions) {
        tf_ += 1.0F / (1.0F + solution.matchLength);
    }
#ifdef INFINITY_DEBUG
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
#endif
    if (!solutions.empty()) {
        doc_freq_++;
        all_tf_.push_back(tf_);
        phrase_freq_ += tf_;
        return true;
    }
    return false;
}

} // namespace infinity