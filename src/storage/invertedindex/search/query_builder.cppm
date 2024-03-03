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
import doc_iterator;
import term_queries;
import column_index_reader;
import indexer;
import match_data;
import index_config;

namespace infinity {

class QueryNode;
export struct QueryContext {
    UniquePtr<QueryNode> query_tree_;
};

export class QueryBuilder {
public:
    QueryBuilder(Indexer *indexer);

    ~QueryBuilder();

    UniquePtr<DocIterator> CreateSearch(QueryContext &context);

private:
    Indexer *indexer_{nullptr};
    Vector<u64> column_ids_;
    IndexReader index_reader_;
    UniquePtr<Scorer> scorer_;
};
} // namespace infinity