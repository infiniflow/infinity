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

export module infinity_core:and_not_iterator;

import :stl;
import :index_defines;
import :multi_doc_iterator;
import :doc_iterator;
import internal_types;

namespace infinity {
export class AndNotIterator : public MultiDocIterator {
public:
    AndNotIterator(Vector<UniquePtr<DocIterator>> iterators);

    DocIteratorType GetType() const override { return DocIteratorType::kAndNotIterator; }

    String Name() const override { return "AndNotIterator"; }

    /* pure virtual methods implementation */
    bool Next(RowID doc_id) override;

    float Score() override;

    void UpdateScoreThreshold(float threshold) override;

    u32 MatchCount() const override;
};
} // namespace infinity