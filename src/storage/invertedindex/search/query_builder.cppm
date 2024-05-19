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
import column_index_reader;
import match_data;
import table_entry;
import internal_types;
import default_values;
import base_table_ref;

namespace infinity {

class Txn;
struct QueryNode;
export struct FullTextQueryContext {
    UniquePtr<QueryNode> query_tree_;
    UniquePtr<QueryNode> optimized_query_tree_;
};

class EarlyTerminateIterator;

export class QueryBuilder {
public:
    QueryBuilder(Txn *txn, SharedPtr<BaseTableRef> &base_table_ref);

    ~QueryBuilder();

    const Map<String, String> &GetColumn2Analyzer() { return index_reader_.GetColumn2Analyzer(); }

    UniquePtr<DocIterator> CreateSearch(FullTextQueryContext &context);

    UniquePtr<EarlyTerminateIterator> CreateEarlyTerminateSearch(FullTextQueryContext &context);

    inline float Score(RowID doc_id) { return scorer_.Score(doc_id); }

private:
    TableEntry *table_entry_{nullptr};
    IndexReader index_reader_;
    Scorer scorer_;
};
} // namespace infinity