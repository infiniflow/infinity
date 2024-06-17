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

export module early_terminate_iterator;
import stl;
import index_defines;
import internal_types;

namespace infinity {

export enum class EarlyTermAlgo {
    kBMW,
    kBMM,
    kNaive,
    kCompare
};

// usage: get sequence of results by calling NextWithThreshold() or BlockNextWithThreshold()
// do not use Next() directly
export class EarlyTerminateIterator {
protected:
    RowID doc_id_ = INVALID_ROWID;
    float threshold_ = 0.0f;
    u32 doc_freq_ = 0;
    float bm25_score_upper_bound_ = 0.0f;

public:
    virtual ~EarlyTerminateIterator() = default;

    inline RowID DocID() const { return doc_id_; }

    inline u32 DocFreq() const { return doc_freq_; }

    inline float BM25ScoreUpperBound() const { return bm25_score_upper_bound_; }

    inline float Threshold() const { return threshold_; }

    Pair<RowID, float> BlockNextWithThreshold(float threshold);

    virtual void UpdateScoreThreshold(float threshold){
        if (threshold > threshold_)
            threshold_ = threshold;
    };

    // Move block cursor to ensure its last_doc_id is no less than given doc_id.
    // Returns false and update doc_id_ to INVALID_ROWID if the iterator is exhausted.
    // Note that this routine decode skip_list only, and doesn't update doc_id_ when returns true.
    // Caller may invoke BlockMaxBM25Score() after this routine.
    virtual bool NextShallow(RowID doc_id) = 0;

    // Update doc_id_ to one no less than given doc_id and its BM25 score is larger than current threshold.
    // Returns false and update doc_id_ to INVALID_ROWID if the iterator is exhausted.
    // Note that this routine decode skip_list and doc_list, and doesn't decode tf_list when returns true.
    // Caller may invoke BlockMaxBM25Score() and BM25Score() after this routine.
    virtual bool Next(RowID doc_id) = 0;

    // Update doc_id_ to one larger than previous one and its BM25 score is larger than current threshold.
    // Note that this routine decode skip_list and doc_list, and doesn't decode tf_list.
    // Caller may invoke BlockMaxBM25Score() and BM25Score() after this routine.
    bool Next();

    // only decode skiplist, need to call Seek() or SeekInBlockRange() to decode doc_id, tf, etc.
    virtual bool BlockSkipTo(RowID doc_id, float threshold) = 0;

    // following functions are available only after BlockSkipTo() is called

    // not always exist
    virtual RowID BlockMinPossibleDocID() const = 0;

    virtual RowID BlockLastDocID() const = 0;

    virtual float BlockMaxBM25Score() = 0;

    // if seek failed in current block, return false, doc_id_ may be unchanged or changed
    // if seek succeed in current block, return true, doc_id_ is updated
    virtual Pair<bool, RowID> SeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond) = 0;

    virtual Tuple<bool, float, RowID> SeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond, float threshold) = 0;

    virtual float BM25Score() = 0;

    virtual Pair<bool, RowID> PeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond) = 0;

    // return true: inner doc_id_ is updated
    // return false: may not find the next valid inner doc_id_
    virtual bool NotPartCheckExist(RowID doc_id) = 0;

    // print the query tree, for debugging
    virtual void PrintTree(std::ostream &os, const String &prefix = "", bool is_final = true) const = 0;
};

export void MultiQueryEarlyTerminateIteratorCommonPrintTree(const EarlyTerminateIterator *this_iter,
                                                            std::string_view iter_type_name,
                                                            const Vector<UniquePtr<EarlyTerminateIterator>> &children,
                                                            std::ostream &os,
                                                            const String &prefix,
                                                            bool is_final);

} // namespace infinity