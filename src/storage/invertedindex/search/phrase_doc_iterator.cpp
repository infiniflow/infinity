module;

#include <cassert>
#include <iostream>

module phrase_doc_iterator;

import stl;
import doc_iterator;
import internal_types;
import third_party;
import posting_iterator;
import column_length_io;
import logger;

namespace infinity {

void PhraseDocIterator::InitBM25Info(UniquePtr<FullTextColumnLengthReader> &&column_length_reader) {
    // BM25 parameters
    constexpr float k1 = 1.2F;
    constexpr float b = 0.75F;

    column_length_reader_ = std::move(column_length_reader);
    u64 total_df = column_length_reader_->GetTotalDF();
    float avg_column_len = column_length_reader_->GetAvgColumnLength();
    float smooth_idf = std::log(1.0F + (total_df - estimate_doc_freq_ + 0.5F) / (estimate_doc_freq_ + 0.5F));
    bm25_common_score_ = weight_ * smooth_idf * (k1 + 1.0F);
    bm25_score_upper_bound_ = bm25_common_score_ / (1.0F + k1 * b / avg_column_len);
    if (SHOULD_LOG_TRACE()) {
        OStringStream oss;
        oss << "TermDocIterator: ";
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

bool PhraseDocIterator::Next(RowID doc_id) {
    assert(doc_id != INVALID_ROWID);
    if (doc_id_ != INVALID_ROWID && doc_id_ >= doc_id)
        return true;
    assert(pos_iters_.size() > 0);
    RowID target_doc_id = doc_id;
    do {
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
                return true;
            }
            target_doc_id++;
        }
    } while (1);
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
    bm25_score_cache_ = bm25_common_score_ * tf_ / (tf_ + p);
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
    os << " (doc_freq: " << GetDF() << ")";
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
        doc_freq_++;
        all_tf_.push_back(tf_);
        phrase_freq_ += tf_;
        return true;
    }
    return false;
}

} // namespace infinity