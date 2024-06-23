module;

#include <iostream>
#include <cassert>

module phrase_doc_iterator;

import stl;
import doc_iterator;
import internal_types;
import third_party;
import posting_iterator;
import match_data;
import logger;

namespace infinity {
    void PhraseDocIterator::DoSeek(RowID doc_id) {
        assert(pos_iters_.size() > 0);

        bool need_loop = true;
        while (need_loop) {
            if (doc_id == INVALID_ROWID) {
                break;
            }
            doc_ids_[0] = pos_iters_[0]->SeekDoc(doc_id);
            RowID max_doc_id = doc_ids_[0];
            need_loop = false;
            for (SizeT i = 1; i < pos_iters_.size(); ++i) {
                auto &iter = pos_iters_[i];
                doc_ids_[i] = iter->SeekDoc(doc_id);
                if (max_doc_id != doc_ids_[i]) {
                    max_doc_id = std::max(max_doc_id, doc_ids_[i]);
                    need_loop = true;
                    break;
                }
            }
            doc_id = max_doc_id;
        }
        doc_id_ = doc_id;
        for (SizeT i = 0; i < pos_iters_.size(); ++i) {
            pos_iters_[i]->SeekDoc(doc_id_);
        }
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

    bool PhraseDocIterator::CheckBeginPosition(pos_t position) {
        pos_t now_position = position;
        for (SizeT i = 1; i < pos_iters_.size(); ++i) {
            auto &iter = pos_iters_[i];
            pos_t next_position = 0;
            iter->SeekPosition(now_position, next_position);
            if (next_position != now_position + 1) {
                return false;
            }
            now_position = next_position;
        }
        return true;
    }

    bool PhraseDocIterator::GetPhraseMatchData(PhraseColumnMatchData &match_data, RowID doc_id) {
        if (doc_id != doc_id_) {
            return false;
        }
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
            doc_freq_++;
            phrase_freq_ += match_data.tf_;
            return true;
        }
        return false;
    }

    const Vector<u32>& PhraseDocIterator::GetAllDF() const {
        return all_df_;
    }

    u32 PhraseDocIterator::GetDF() const {
        return doc_freq_;
    }
}