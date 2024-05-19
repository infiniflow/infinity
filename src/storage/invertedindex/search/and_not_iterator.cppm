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

export module and_not_iterator;

import stl;
import index_defines;
import multi_query_iterator;
import doc_iterator;
import internal_types;

namespace infinity {
export class AndNotIterator : public MultiQueryDocIterator {
public:
    AndNotIterator(Vector<UniquePtr<DocIterator>> iterators);

    bool IsAndNot() const override { return true; }

    void DoSeek(RowID doc_id) override;

    u32 GetDF() const override;
};
} // namespace infinity