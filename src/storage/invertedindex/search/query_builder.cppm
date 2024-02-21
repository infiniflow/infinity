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

export module query_builder;

import stl;
import third_party;
import doc_iterator;
import term_queries;
import column_index_reader;

namespace infinity {

namespace detail {
template <class T>
struct Hash {
    inline SizeT operator()(const T &val) const { return val; }
};
} // namespace detail

class QueryNode;
export struct QueryContext {
    UniquePtr<QueryNode> query_tree_;
};

export class QueryBuilder {
public:
    QueryBuilder();

    ~QueryBuilder();

    UniquePtr<DocIterator> CreateSearch(QueryContext &context);

private:
    FlatHashMap<u64, UniquePtr<ColumnIndexReader>, detail::Hash<u64>> column_index_readers_;
};
} // namespace infinity