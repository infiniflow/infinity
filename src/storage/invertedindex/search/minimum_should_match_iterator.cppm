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

export module minimum_should_match_iterator;
import stl;
import doc_iterator;
import multi_doc_iterator;
import internal_types;

namespace infinity {

export class MinimumShouldMatchIterator final : public MultiDocIterator {
public:
    MinimumShouldMatchIterator(Vector<UniquePtr<DocIterator>> &&iterators, u32 minimum_should_match);

    ~MinimumShouldMatchIterator() override;

    DocIteratorType GetType() const override { return DocIteratorType::kMinimumShouldMatchIterator; }

    String Name() const override { return "MinimumShouldMatchIterator"; }

    void UpdateScoreThreshold(float threshold) override;

    bool Next(RowID doc_id) override;

    float Score() override;

    u32 MatchCount() const override;

private:
    RowID HeadHeapTop() const;
    void PushToHeadHeap(u32 idx);
    u32 PopFromHeadHeap();
    Pair<bool, u32> PushToTailHeap(u32 idx);
    u32 PopFromTailHeap();

    const u32 minimum_should_match_ = 0;
    Vector<u32> lead_{};
    Vector<u32> head_heap_{};
    Vector<u32> tail_heap_{};
    u32 tail_size_ = 0;

    // score cache
    RowID score_cache_docid_ = {};
    float score_cache_ = {};
};

export template <std::derived_from<MultiDocIterator> T>
class MinimumShouldMatchWrapper final : public T {
    u32 minimum_should_match_ = 0;

public:
    MinimumShouldMatchWrapper(Vector<UniquePtr<DocIterator>> &&iterators, const u32 minimum_should_match)
        : T(std::move(iterators)), minimum_should_match_(minimum_should_match) {}
    ~MinimumShouldMatchWrapper() override = default;
    bool Next(RowID doc_id) override {
        for (; T::Next(doc_id); doc_id = this->doc_id_ + 1) {
            if (this->MatchCount() >= minimum_should_match_) {
                return true;
            }
        }
        return false;
    }
};

} // namespace infinity
