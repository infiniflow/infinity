// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

export module rank_features_doc_iterator;

import stl;
import index_defines;
import doc_iterator;
import multi_doc_iterator;
import internal_types;

namespace infinity {

export class RankFeaturesDocIterator : public MultiDocIterator {
public:
    explicit RankFeaturesDocIterator(Vector<UniquePtr<DocIterator>> &&iterators);

    ~RankFeaturesDocIterator() override;

    DocIteratorType GetType() const override { return DocIteratorType::kRankFeaturesDocIterator; }

    String Name() const override { return "RankFeaturesDocIterator"; }

    void UpdateScoreThreshold(float threshold) override {}

    bool Next(RowID doc_id) override;

    float Score() override { return 1.0f; }

    u32 MatchCount() const override;

    void DecodeFrom(RowID buffer_start_doc_id);

private:
    RowID buffer_start_doc_id_ = INVALID_ROWID;
    u32 memset_bytes_ = 0;
    void *aligned_buffer_ = nullptr;
    u32 *match_cnt_ptr_ = nullptr;
    u16 *payload_ptr_ = nullptr;
};

} // namespace infinity
