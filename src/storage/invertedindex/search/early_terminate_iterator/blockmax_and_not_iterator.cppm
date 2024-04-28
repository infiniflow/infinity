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

export module blockmax_and_not_iterator;
import stl;
import index_defines;
import early_terminate_iterator;
import internal_types;

namespace infinity {

export class BlockMaxAndNotIterator final : public EarlyTerminateIterator {
public:
    explicit BlockMaxAndNotIterator(Vector<UniquePtr<EarlyTerminateIterator>> iterators) : inner_iterators_(std::move(iterators)) {
        doc_freq_ = inner_iterators_[0]->DocFreq();
        bm25_score_upper_bound_ = inner_iterators_[0]->BM25ScoreUpperBound();
    }

    void UpdateScoreThreshold(float threshold) override { return inner_iterators_[0]->UpdateScoreThreshold(threshold); }

    bool BlockSkipTo(RowID doc_id, float threshold) override { return inner_iterators_[0]->BlockSkipTo(doc_id, threshold); }

    // following functions are available only after BlockSkipTo() is called

    RowID BlockMinPossibleDocID() const override { return inner_iterators_[0]->BlockMinPossibleDocID(); }

    RowID BlockLastDocID() const override { return inner_iterators_[0]->BlockLastDocID(); }

    float BlockMaxBM25Score() override { return inner_iterators_[0]->BlockMaxBM25Score(); }

    Pair<bool, RowID> SeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond) override;
    Tuple<bool, float, RowID> SeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond, float threshold) override;
    float BM25Score() override { return inner_iterators_[0]->BM25Score(); }

    Pair<bool, RowID> PeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond) override;

    bool NotPartCheckExist(RowID doc_id) override;

    void PrintTree(std::ostream &os, const String &prefix, bool is_final) const override {
        return MultiQueryEarlyTerminateIteratorCommonPrintTree(this, "BlockMaxAndNotIterator", inner_iterators_, os, prefix, is_final);
    }

private:
    Vector<UniquePtr<EarlyTerminateIterator>> inner_iterators_; // first: iter, others: "not" part
};

} // namespace infinity