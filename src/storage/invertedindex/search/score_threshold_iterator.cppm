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

export module score_threshold_iterator;
import stl;
import doc_iterator;
import internal_types;

namespace infinity {

export class ScoreThresholdIterator final : public DocIterator {
    UniquePtr<DocIterator> query_iterator_{};
    f32 score_threshold_ = {};

public:
    ScoreThresholdIterator(UniquePtr<DocIterator> &&iter, const f32 score_threshold)
        : query_iterator_(std::move(iter)), score_threshold_(score_threshold) {}
    ~ScoreThresholdIterator() override = default;
    DocIteratorType GetType() const override { return DocIteratorType::kScoreThresholdIterator; }
    String Name() const override { return "ScoreThresholdIterator"; };
    bool Next(RowID doc_id) override;
    float BM25Score() override { return query_iterator_->BM25Score(); }
    void UpdateScoreThreshold(const float threshold) override { query_iterator_->UpdateScoreThreshold(threshold); }
    u32 MatchCount() const override { return query_iterator_->MatchCount(); }
    void PrintTree(std::ostream &os, const String &prefix, bool is_final) const override;
};

} // namespace infinity
