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

export module infinity_core:query_builder;

import :doc_iterator;
import :column_index_reader;
import :meta_info;
import :default_values;
import :base_table_ref;
import :parse_fulltext_options;
import :query_node;

import internal_types;

namespace infinity {

export struct FullTextQueryContext {
    std::unique_ptr<QueryNode> query_tree_{};
    std::unique_ptr<QueryNode> optimized_query_tree_{};
    const FulltextSimilarity ft_similarity_{};
    const BM25Params bm25_params_{};
    const MinimumShouldMatchOption minimum_should_match_option_{};
    const RankFeaturesOption rank_features_option_{};
    u32 minimum_should_match_ = 0;
    u32 topn_ = 0;
    EarlyTermAlgo early_term_algo_ = EarlyTermAlgo::kNaive;
    const std::vector<std::string> &index_names_;

    FullTextQueryContext(const FulltextSimilarity ft_similarity,
                         const BM25Params &bm25_params,
                         const MinimumShouldMatchOption &minimum_should_match_option,
                         const RankFeaturesOption &rank_features_option,
                         const u32 topn,
                         const std::vector<std::string> &index_names)
        : ft_similarity_(ft_similarity), bm25_params_(bm25_params), minimum_should_match_option_(minimum_should_match_option),
          rank_features_option_(rank_features_option), topn_(topn), index_names_(index_names) {}
};

export class QueryBuilder {
public:
    explicit QueryBuilder(std::shared_ptr<TableInfo> table_info) : table_info_(std::move(table_info)) {};

    void Init(std::shared_ptr<IndexReader> index_reader);

    ~QueryBuilder();

    std::map<std::string, std::string> GetColumn2Analyzer(const std::vector<std::string> &hints) const { return index_reader_->GetColumn2Analyzer(hints); }

    std::unique_ptr<DocIterator> CreateSearch(FullTextQueryContext &context);

private:
    std::shared_ptr<TableInfo> table_info_{nullptr};
    std::shared_ptr<IndexReader> index_reader_{};
};
} // namespace infinity
