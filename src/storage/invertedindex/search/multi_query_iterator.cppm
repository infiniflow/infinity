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

export module multi_query_iterator;

import stl;
import memory_pool;
import posting_iterator;
import index_defines;
import segment;
import index_config;
import doc_iterator;
namespace infinity {
export class MultiQueryIterator : public DocIterator {
public:
    MultiQueryIterator();

    virtual ~MultiQueryIterator() = default;

    virtual bool IsAnd() const { return false; }

    virtual bool IsAndNot() const { return false; }

    virtual bool IsOr() const { return false; }

protected:
    Vector<UniquePtr<DocIterator>> children_;
};
} // namespace infinity