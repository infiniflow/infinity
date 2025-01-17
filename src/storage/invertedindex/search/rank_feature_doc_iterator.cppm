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

export module rank_feature_doc_iterator;

import stl;

import posting_iterator;
import index_defines;
import doc_iterator;
import internal_types;
import doc_iterator;
import third_party;

namespace infinity {

export class RankFeatureDocIterator final : public DocIterator {
public:
    RankFeatureDocIterator(UniquePtr<PostingIterator> &&iter, u64 column_id, float boost);

    ~RankFeatureDocIterator() override;

    DocIteratorType GetType() const override { return DocIteratorType::kTermDocIterator; }

    String Name() const override { return "RankFeatureDocIterator"; }

    void UpdateScoreThreshold(float threshold) override {}

    u32 MatchCount() const override { return 0; }

    bool Next(RowID doc_id) override;

    float Score() override;

    void PrintTree(std::ostream &os, const String &prefix, bool is_final) const override;

    const String *term_ptr_ = nullptr;
    const String *column_name_ptr_ = nullptr;

private:
    u64 column_id_;
    float boost_ = 1.0f;
    UniquePtr<PostingIterator> iter_;
};

} // namespace infinity
