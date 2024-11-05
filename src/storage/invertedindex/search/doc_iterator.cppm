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

export module doc_iterator;

import stl;

import index_defines;
import internal_types;

namespace infinity {

export enum class EarlyTermAlgo { kNaive, kBMM, kBMW, kCompare };

export enum class DocIteratorType : u8 {
    kTermDocIterator,
    kPhraseIterator,
    kAndIterator,
    kAndNotIterator,
    kOrIterator,
    kMinimumShouldMatchIterator,
    kBMMIterator,
    kBMWIterator,
    kFilterIterator,
    kScoreThresholdIterator,
    kKeywordIterator,
};

export struct DocIteratorEstimateIterateCost {
    u32 priority_ = 0;
    u32 estimate_cost_ = 0;
    friend auto operator<=>(const DocIteratorEstimateIterateCost &, const DocIteratorEstimateIterateCost &) = default;
    auto &operator+=(DocIteratorEstimateIterateCost rhs) {
        if (priority_ < rhs.priority_) {
            std::swap(*this, rhs);
        }
        estimate_cost_ += (rhs.estimate_cost_ >> 3 * (priority_ - rhs.priority_));
        return *this;
    }
};

export class DocIterator {
public:
    DocIterator() {}

    virtual ~DocIterator();

    RowID DocID() const { return doc_id_; }

    inline DocIteratorEstimateIterateCost GetEstimateIterateCost() const { return estimate_iterate_cost_; }

    // Update doc_id_ to one larger than previous one.
    // If has_blockmax is true, it ensures its BM25 score be larger than current threshold.
    // Note that this routine decode skip_list and doc_list, and doesn't decode tf_list.
    // Caller may invoke BlockMaxBM25Score() and BM25Score() after this routine.
    inline bool Next() {
        RowID target_doc_id = (doc_id_ == INVALID_ROWID) ? 0 : (doc_id_ + 1);
        return Next(target_doc_id);
    }

    inline float BM25ScoreUpperBound() const { return bm25_score_upper_bound_; }

    inline float Threshold() const { return threshold_; }

    /* virtual methods */

    virtual DocIteratorType GetType() const = 0;
    virtual String Name() const = 0;

    // Update doc_id_ to one no less than given doc_id.
    // If has_blockmax is true, it ensures its BM25 score be larger than current threshold.
    virtual bool Next(RowID doc_id) = 0;

    virtual float Score() = 0;

    virtual void UpdateScoreThreshold(float threshold) = 0;

    // for minimum_should_match parameter
    virtual u32 MatchCount() const = 0;

    // print the query tree, for debugging
    virtual void PrintTree(std::ostream &os, const String &prefix = "", bool is_final = true) const = 0;

protected:
    RowID doc_id_{INVALID_ROWID};
    float threshold_ = 0.0f;
    float bm25_score_upper_bound_ = std::numeric_limits<float>::max();
    DocIteratorEstimateIterateCost estimate_iterate_cost_ = {};
};
} // namespace infinity
