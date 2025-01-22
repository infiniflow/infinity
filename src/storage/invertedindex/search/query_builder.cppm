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
import table_entry;
import internal_types;
import default_values;
import base_table_ref;
import parse_fulltext_options;
import query_node;

namespace infinity {

class Txn;

export struct FullTextQueryContext {
    UniquePtr<QueryNode> query_tree_{};
    UniquePtr<QueryNode> optimized_query_tree_{};
    const FulltextSimilarity ft_similarity_{};
    const BM25Params bm25_params_{};
    const MinimumShouldMatchOption minimum_should_match_option_{};
    u32 minimum_should_match_ = 0;
    u32 topn_ = 0;
    EarlyTermAlgo early_term_algo_ = EarlyTermAlgo::kNaive;
    const Vector<String> &index_names_;

    FullTextQueryContext(const FulltextSimilarity ft_similarity,
                         const BM25Params &bm25_params,
                         const MinimumShouldMatchOption &minimum_should_match_option,
                         const u32 topn,
                         const Vector<String> &index_names)
        : ft_similarity_(ft_similarity), bm25_params_(bm25_params), minimum_should_match_option_(minimum_should_match_option), topn_(topn),
          index_names_(index_names) {}
};

export class QueryBuilder {
public:
    explicit QueryBuilder(BaseTableRef *base_table_ref) : base_table_ref_(base_table_ref), table_entry_(base_table_ref->table_entry_ptr_) {};

    void Init(IndexReader index_reader);

    ~QueryBuilder();

    Map<String, String> GetColumn2Analyzer(const Vector<String> &hints) const { return index_reader_.GetColumn2Analyzer(hints); }

    UniquePtr<DocIterator> CreateSearch(FullTextQueryContext &context);

private:
    BaseTableRef *base_table_ref_{nullptr};
    TableEntry *table_entry_{nullptr};
    IndexReader index_reader_;
};
} // namespace infinity
