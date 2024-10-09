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

export module and_iterator;

import stl;
import index_defines;
import doc_iterator;
import multi_doc_iterator;
import internal_types;

namespace infinity {

export class AndIterator : public MultiDocIterator {
public:
    AndIterator(Vector<UniquePtr<DocIterator>> iterators);

    DocIteratorType GetType() const override { return DocIteratorType::kAndIterator; }

    String Name() const override { return "AndIterator"; }

    /* pure virtual methods implementation */
    bool Next(RowID doc_id) override;

    float BM25Score() override;

    void UpdateScoreThreshold(float threshold) override;

    u32 LeafCount() const override;

    u32 MatchCount() const override;

private:
    // bm25 score cache
    RowID bm25_score_cache_docid_ = INVALID_ROWID;
    float bm25_score_cache_ = 0.0f;
    // for minimum_should_match
    u32 fixed_match_count_ = 0;
    Vector<u32> dyn_match_ids_{};
};
} // namespace infinity