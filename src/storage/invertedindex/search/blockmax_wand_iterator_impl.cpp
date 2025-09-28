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

module infinity_core:blockmax_wand_iterator.impl;

import :blockmax_wand_iterator;
import :index_defines;
import :blockmax_leaf_iterator;
import :multi_doc_iterator;
import :logger;
import :infinity_exception;

import third_party;
import internal_types;

namespace infinity {

BlockMaxWandIterator::~BlockMaxWandIterator() {
    if (SHOULD_LOG_TRACE()) {
        std::string msg = "BlockMaxWandIterator pivot_history: ";
        size_t num_history = pivot_history_.size();
        for (size_t i = 0; i < num_history; i++) {
            auto &p = pivot_history_[i];
            u32 pivot = std::get<0>(p);
            u64 row_id = std::get<1>(p);
            float score = std::get<2>(p);
            // oss << " (" << pivot << ", " << row_id << ", " << score << ")";
            msg += fmt::format(" ({}, {}, {:6f})", pivot, row_id, score);
        }
        msg += fmt::format("\nnext_sort_cnt_ {}, next_it0_docid_mismatch_cnt_ {}, next_sum_score_low_cnt_ {}, next_sum_score_bm_low_cnt_ {}",
                           next_sort_cnt_,
                           next_it0_docid_mismatch_cnt_,
                           next_sum_score_low_cnt_,
                           next_sum_score_bm_low_cnt_);
        LOG_TRACE(msg);
    }
}

BlockMaxWandIterator::BlockMaxWandIterator(std::vector<std::unique_ptr<DocIterator>> &&iterators)
    : MultiDocIterator(std::move(iterators)), pivot_(sorted_iterators_.size()) {
    bm25_score_upper_bound_ = 0.0f;
    estimate_iterate_cost_ = {};
    size_t num_iterators = children_.size();
    for (size_t i = 0; i < num_iterators; i++) {
        BlockMaxLeafIterator *tdi = dynamic_cast<BlockMaxLeafIterator *>(children_[i].get());
        if (tdi == nullptr) {
            UnrecoverableError("BMW only supports BlockMaxLeafIterator");
        }
        bm25_score_upper_bound_ += tdi->BM25ScoreUpperBound();
        estimate_iterate_cost_ += tdi->GetEstimateIterateCost();
        sorted_iterators_.push_back(tdi);
    }
    next_sum_score_bm_low_cnt_dist_.resize(100, 0);
    backup_iterators_.reserve(sorted_iterators_.size());

    // Initialize optimization structures for many keywords
    score_ub_prefix_sums_.reserve(num_iterators + 1);
    iterator_indices_.reserve(num_iterators);

    // Pre-populate iterator indices for cache-friendly access
    for (size_t i = 0; i < num_iterators; i++) {
        iterator_indices_.push_back(i);
    }
    indices_valid_ = true;

    UpdateScoreUpperBoundPrefixSums();
}

// Highly optimized pivot calculation with multiple strategies and improved safety
size_t BlockMaxWandIterator::FindPivotOptimized(float threshold) {
    const size_t num_iterators = sorted_iterators_.size();

    // Safety check for empty iterator list
    if (num_iterators == 0) {
        return 0;
    }

    // For very large sets, try fast estimation first
    if (num_iterators > FAST_PIVOT_THRESHOLD) {
        size_t estimated_pivot;
        if (TryFastPivotEstimation(threshold, estimated_pivot)) {
            // Double-check the estimated pivot for safety
            if (estimated_pivot < num_iterators) {
                return estimated_pivot;
            }
        }
    }

    // Ensure prefix sums are valid
    if (!prefix_sums_valid_) {
        UpdateScoreUpperBoundPrefixSums();
    }

    // Binary search for pivot using prefix sums - more cache-friendly
    size_t left = 0, right = num_iterators;
    while (left < right) {
        size_t mid = left + ((right - left) >> 1); // Avoid overflow
        if (score_ub_prefix_sums_[mid + 1] > threshold) {
            right = mid;
        } else {
            left = mid + 1;
        }
    }

    // Safety check - ensure we don't return an out-of-bounds pivot
    return std::min(left, num_iterators);
}

void BlockMaxWandIterator::UpdateScoreUpperBoundPrefixSums() {
    const size_t num_iterators = sorted_iterators_.size();

    // Resize only if necessary to avoid memory allocations
    if (score_ub_prefix_sums_.size() != num_iterators + 1) {
        score_ub_prefix_sums_.resize(num_iterators + 1);
    }

    score_ub_prefix_sums_[0] = 0.0f;

    // Vectorized accumulation for better performance
    for (size_t i = 0; i < num_iterators; i++) {
        score_ub_prefix_sums_[i + 1] = score_ub_prefix_sums_[i] + sorted_iterators_[i]->BM25ScoreUpperBound();
    }
    prefix_sums_valid_ = true;
}

bool BlockMaxWandIterator::ShouldSkipSort() const {
    const size_t num_iterators = sorted_iterators_.size();

    // For small keyword sets, always sort for optimal performance
    if (num_iterators <= SORT_SKIP_THRESHOLD) {
        return false;
    }

    // For large keyword sets, use adaptive lazy sorting strategy
    // Skip more aggressively if we've had many consecutive skips without issues
    u32 adaptive_interval = LAZY_SORT_INTERVAL;
    if (consecutive_skips_ > 10) {
        adaptive_interval = std::min(adaptive_interval + 2, 8u);
    }

    return iterations_since_sort_ < adaptive_interval;
}

// Optimized partial sort that only sorts what we need
void BlockMaxWandIterator::OptimizedPartialSort(size_t limit) {
    const size_t num_iterators = sorted_iterators_.size();
    if (limit >= num_iterators) {
        std::sort(sorted_iterators_.begin(), sorted_iterators_.end(), [](const auto &a, const auto &b) { return a->DocID() < b->DocID(); });
    } else {
        std::partial_sort(sorted_iterators_.begin(), sorted_iterators_.begin() + limit, sorted_iterators_.end(), [](const auto &a, const auto &b) {
            return a->DocID() < b->DocID();
        });
    }
}

// Aggressively optimized pivot estimation with safety guarantees
bool BlockMaxWandIterator::TryFastPivotEstimation(float threshold, size_t &estimated_pivot) {
    const size_t num_iterators = sorted_iterators_.size();
    if (num_iterators <= FAST_PIVOT_THRESHOLD) {
        return false;
    }

    estimated_pivot = num_iterators;

    // Level 1: Top 5 high-score terms check (hot path)
    constexpr size_t TOP_K = 5;
    float sum = 0.0f;
    for (size_t i = 0; i < std::min(TOP_K, num_iterators); ++i) {
        sum += sorted_iterators_[i]->BM25ScoreUpperBound();
        if (sum > threshold) {
            estimated_pivot = i;
            return true;
        }
    }

    // Level 2: Strided sampling with early termination
    const size_t stride = std::max<size_t>(1, std::min(num_iterators / 20, (num_iterators - TOP_K) / 10));
    float prev_sum = sum;
    size_t prev_i = TOP_K;

    for (size_t i = TOP_K; i < num_iterators; i += stride) {
        prev_sum = sum;
        prev_i = i;
        sum += sorted_iterators_[i]->BM25ScoreUpperBound();

        if (sum > threshold) {
            if (prev_i >= i || prev_i >= num_iterators) {
                estimated_pivot = i;
                return true;
            }

            // Linear search backward to find exact pivot
            for (size_t j = prev_i; j < i && j < num_iterators; ++j) {
                prev_sum += sorted_iterators_[j]->BM25ScoreUpperBound();
                if (prev_sum > threshold) {
                    estimated_pivot = j;
                    return true;
                }
            }
            estimated_pivot = i;
            return true;
        }

        float max_remaining_score = 0.0f;
        size_t remaining_count = num_iterators - i - 1;
        size_t sample_size = std::min(remaining_count, size_t(10));

        if (sample_size > 0) {
            float max_score = 0.0f;
            for (size_t s = 0; s < sample_size; ++s) {
                size_t idx = i + 1 + (s * remaining_count / sample_size);
                if (idx < num_iterators) {
                    max_score = std::max(max_score, sorted_iterators_[idx]->BM25ScoreUpperBound());
                }
            }
            max_remaining_score = remaining_count * max_score;
        }

        if (sum + max_remaining_score <= threshold) {
            break;
        }
    }

    return false;
}

void BlockMaxWandIterator::UpdateScoreThreshold(const float threshold) {
    if (threshold > threshold_) {
        threshold_ = threshold;
        const float base_threshold = threshold - BM25ScoreUpperBound();
        size_t num_iterators = sorted_iterators_.size();
        for (size_t i = 0; i < num_iterators; i++) {
            auto &it = sorted_iterators_[i];
            float new_threshold = base_threshold + it->BM25ScoreUpperBound();
            it->UpdateScoreThreshold(new_threshold);
        }
        // Invalidate prefix sums when threshold changes significantly
        prefix_sums_valid_ = false;
    }
}

bool BlockMaxWandIterator::Next(RowID doc_id) {
    assert(doc_id != INVALID_ROWID);
    bm25_score_cached_ = false;
    size_t num_iterators = sorted_iterators_.size();
    if (doc_id_ == INVALID_ROWID) {
        // Initialize children once.
        for (size_t i = 0; i < num_iterators; i++) {
            sorted_iterators_[i]->Next(0);
        }
    } else if (doc_id_ >= doc_id) {
        return true;
    } else {
        assert(pivot_ < num_iterators);
        assert(doc_id_ < doc_id);
        // Move all pointers from lists[0] to lists[p] by calling Next(list, d + 1)
        for (size_t i = 0; i <= pivot_ && sorted_iterators_[i]->DocID() < doc_id; i++) {
            sorted_iterators_[i]->Next(doc_id);
        }
    }
    while (1) {
        // Enhanced sorting strategy for many keywords
        bool should_sort = !ShouldSkipSort();
        if (should_sort) {
            next_sort_cnt_++;
            consecutive_skips_ = 0; // Reset consecutive skip counter

            if (num_iterators > SORT_SKIP_THRESHOLD) {
                // For large keyword sets, use intelligent partial sort
                size_t sort_limit = num_iterators / PARTIAL_SORT_FACTOR + 5;
                sort_limit = std::min(sort_limit, num_iterators);
                OptimizedPartialSort(sort_limit);
            } else {
                // For smaller keyword sets, use full sort
                std::sort(sorted_iterators_.begin(), sorted_iterators_.end(), [](const auto &a, const auto &b) { return a->DocID() < b->DocID(); });
            }
            iterations_since_sort_ = 0;
            prefix_sums_valid_ = false; // Invalidate prefix sums after sorting
        } else {
            iterations_since_sort_++;
            consecutive_skips_++;
        }

        // remove exhausted lists - optimized for batch removal
        size_t write_pos = num_iterators;
        for (int i = int(num_iterators) - 1; i >= 0 && sorted_iterators_[i]->DocID() == INVALID_ROWID; i--) {
            if (SHOULD_LOG_TRACE()) {
                std::ostringstream oss;
                sorted_iterators_[i]->PrintTree(oss, "Exhaused: ", true);
                LOG_TRACE(oss.str());
            }
            bm25_score_upper_bound_ -= sorted_iterators_[i]->BM25ScoreUpperBound();
            write_pos = i;
        }

        if (write_pos < num_iterators) {
            sorted_iterators_.erase(sorted_iterators_.begin() + write_pos, sorted_iterators_.end());
            num_iterators = sorted_iterators_.size();
            prefix_sums_valid_ = false; // Invalidate prefix sums when iterators are removed
            indices_valid_ = false;     // Invalidate indices cache
        }
        if (bm25_score_upper_bound_ <= threshold_) [[unlikely]] {
            doc_id_ = INVALID_ROWID;
            return false;
        }

        // Enhanced pivot calculation with multiple optimization strategies
        size_t pivot;
        if (num_iterators > SORT_SKIP_THRESHOLD && should_sort) {
            pivot = FindPivotOptimized(threshold_);
        } else {
            // Critical: BMW algorithm requires sorted order for correctness
            if (!should_sort) {
                // We must ensure sorted order for correct pivot calculation
                // Use a more efficient sort for this critical path
                if (num_iterators > FAST_PIVOT_THRESHOLD) {
                    // For very large sets, sort only what we likely need
                    size_t estimated_limit = std::min(num_iterators, num_iterators / 4 + 10);
                    OptimizedPartialSort(estimated_limit);
                } else {
                    std::sort(sorted_iterators_.begin(), sorted_iterators_.end(), [](const auto &a, const auto &b) {
                        return a->DocID() < b->DocID();
                    });
                }
                iterations_since_sort_ = 0;
                prefix_sums_valid_ = false;
            }

            if (!prefix_sums_valid_) {
                UpdateScoreUpperBoundPrefixSums();
            }

            // Use optimized pivot calculation even for smaller sets
            if (prefix_sums_valid_ || num_iterators > 30) {
                pivot = FindPivotOptimized(threshold_);
            } else {
                // Linear search for very small sets (most cache-friendly)
                pivot = num_iterators;
                float sum_score_ub = 0.0f;
                for (size_t i = 0; i < num_iterators; i++) {
                    sum_score_ub += sorted_iterators_[i]->BM25ScoreUpperBound();
                    if (sum_score_ub > threshold_) {
                        pivot = i;
                        break;
                    }
                }
            }
        }

        if (pivot >= num_iterators) [[unlikely]] {
            doc_id_ = INVALID_ROWID;
            return false;
        }
        RowID d = sorted_iterators_[pivot]->DocID();
        while (pivot + 1 < num_iterators && sorted_iterators_[pivot + 1]->DocID() == d) {
            pivot++;
        }

        // NextShallow iterators 0..pivot to d, sum the blockmax score, and purge exhausted lists
        float sum_score_bm = 0.0f;
        bool found_exhausted_it = false;
        size_t exhausted_idx = 0;

        for (size_t i = 0; i <= pivot; i++) {
            bool ok = sorted_iterators_[i]->NextShallow(d);
            if (ok) [[likely]] {
                sum_score_bm += sorted_iterators_[i]->BlockMaxBM25Score();
            } else {
                if (SHOULD_LOG_TRACE()) {
                    std::ostringstream oss;
                    sorted_iterators_[i]->PrintTree(oss, "Exhausted: ", true);
                    LOG_TRACE(oss.str());
                }
                exhausted_idx = i;
                found_exhausted_it = true;
                break;
            }
        }

        if (found_exhausted_it) [[unlikely]] {
            // Remove exhausted iterator and update bounds
            bm25_score_upper_bound_ -= sorted_iterators_[exhausted_idx]->BM25ScoreUpperBound();
            sorted_iterators_.erase(sorted_iterators_.begin() + exhausted_idx);
            num_iterators = sorted_iterators_.size();
            prefix_sums_valid_ = false;
            indices_valid_ = false;
            continue;
        }

        if (sum_score_bm > threshold_) {
            if (sorted_iterators_[0]->DocID() == d) {
                // EvaluatePartial(d , p);
                float sum_score = 0.0f;
                for (size_t i = 0; i <= pivot; i++) {
                    sum_score += sorted_iterators_[i]->BM25Score();
                }
                if (sum_score > threshold_) {
                    pivot_ = pivot;
                    doc_id_ = d;
                    bm25_score_cache_ = sum_score;
                    bm25_score_cached_ = true;
                    pivot_history_.emplace_back(pivot_, doc_id_.ToUint64(), sum_score);
                    return true;
                } else {
                    next_sum_score_low_cnt_++;
                    for (size_t i = 0; i <= pivot; i++) {
                        sorted_iterators_[i]->Next(d + 1);
                    }
                }
            } else {
                next_it0_docid_mismatch_cnt_++;
                for (size_t i = 0; i < pivot && sorted_iterators_[i]->DocID() < d; i++) {
                    sorted_iterators_[i]->Next(d);
                }
            }
        } else {
            next_sum_score_bm_low_cnt_++;
            // d′ = GetNewCandidate();
            // Choose one list from the lists before and including lists[p] with the largest IDF, move it by calling Next(list, d′)
            RowID up_to = INVALID_ROWID;
            if (pivot + 1 < num_iterators)
                up_to = sorted_iterators_[pivot + 1]->DocID();
            RowID shallowed_did = INVALID_ROWID;
            std::partial_sort(sorted_iterators_.begin(),
                              sorted_iterators_.begin() + 1,
                              sorted_iterators_.begin() + pivot + 1,
                              [](const auto &a, const auto &b) { return a->BlockLastDocID() < b->BlockLastDocID(); });
            // sum_score_bm_0 is the sum of block_max of [1..pivot]
            float sum_score_bm_0 = sum_score_bm - sorted_iterators_[0]->BlockMaxBM25Score();
            RowID block_last_doc_id_0 = sorted_iterators_[0]->BlockLastDocID();
            size_t cnt_next_shallow = 0;
            while (block_last_doc_id_0 < up_to) {
                shallowed_did = block_last_doc_id_0 + 1;
                cnt_next_shallow++;
                bool ok = sorted_iterators_[0]->NextShallow(block_last_doc_id_0 + 1);
                if (!ok)
                    break;
                block_last_doc_id_0 = sorted_iterators_[0]->BlockLastDocID();
                if (block_last_doc_id_0 == INVALID_ROWID)
                    break;
                sum_score_bm = sum_score_bm_0 + sorted_iterators_[0]->BlockMaxBM25Score();
                if (sum_score_bm > threshold_)
                    break;
                std::partial_sort(sorted_iterators_.begin(),
                                  sorted_iterators_.begin() + 1,
                                  sorted_iterators_.begin() + pivot + 1,
                                  [](const auto &a, const auto &b) { return a->BlockLastDocID() < b->BlockLastDocID(); });
                // sum_score_bm_0 is the sum of block_max of [1..pivot]
                sum_score_bm_0 = sum_score_bm - sorted_iterators_[0]->BlockMaxBM25Score();
                block_last_doc_id_0 = sorted_iterators_[0]->BlockLastDocID();
            }
            if (cnt_next_shallow >= 100) [[unlikely]] {
                next_sum_score_bm_low_cnt_dist_[99]++;
            } else {
                next_sum_score_bm_low_cnt_dist_[cnt_next_shallow]++;
            }
            if (shallowed_did != INVALID_ROWID) {
                for (size_t i = 0; i <= pivot; i++) {
                    if (sorted_iterators_[i]->DocID() < shallowed_did)
                        sorted_iterators_[i]->Next(shallowed_did);
                }
            } else {
                for (size_t i = 0; i <= pivot; i++) {
                    sorted_iterators_[i]->Next(up_to);
                }
            }
        }
    }

    UnrecoverableError("Should not reach here!");
    return false;
}

float BlockMaxWandIterator::BM25Score() {
    if (bm25_score_cached_) [[unlikely]] {
        return bm25_score_cache_;
    }
    size_t num_iterators = sorted_iterators_.size();
    if (doc_id_ == INVALID_ROWID || pivot_ >= num_iterators) [[unlikely]] {
        return 0.0f;
    }
    float sum_score = 0.0f;
    RowID d = sorted_iterators_[pivot_]->DocID();
    for (size_t i = 0; i < num_iterators && sorted_iterators_[i]->DocID() == d; i++) {
        sum_score += sorted_iterators_[i]->BM25Score();
    }
    bm25_score_cache_ = sum_score;
    bm25_score_cached_ = true;
    return sum_score;
}

u32 BlockMaxWandIterator::MatchCount() const {
    u32 count = 0;
    if (const auto current_doc_id = DocID(); current_doc_id != INVALID_ROWID) {
        for (u32 i = 0; i < sorted_iterators_.size() && sorted_iterators_[i]->DocID() == current_doc_id; ++i) {
            count += sorted_iterators_[i]->MatchCount();
        }
    }
    return count;
}

} // namespace infinity
