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
import multi_query_iterator;

namespace infinity {

export class AndIterator : public MultiQueryDocIterator {
public:
    AndIterator(Vector<UniquePtr<DocIterator>> iterators);

    virtual ~AndIterator();

    bool IsAnd() const override { return true; }

    void DoSeek(docid_t doc_id) override;

    u32 GetDF() const override;

private:
    Vector<DocIterator *> sorted_iterators_;
};
} // namespace infinity