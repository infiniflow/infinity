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

export module infinity_core:logical_match;

import :logical_node_type;
import :column_binding;
import :logical_node;
import :base_expression;
import :match_expression;
import :base_table_ref;
import :meta_info;
import :common_query_filter;
import :column_index_reader;
import :doc_iterator;
import :query_node;
import :parse_fulltext_options;

import internal_types;
import data_type;

namespace infinity {

export class LogicalMatch : public LogicalNode {
public:
    explicit LogicalMatch(u64 node_id, std::shared_ptr<BaseTableRef> base_table_ref, std::shared_ptr<MatchExpression> match_expr);

    [[nodiscard]] std::vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::string>> GetOutputNames() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final;

    [[nodiscard]] TableInfo *table_info() const;

    [[nodiscard]] std::string TableAlias() const;

    [[nodiscard]] u64 TableIndex() const;

    std::string ToString(i64 &space) const final;

    inline std::string name() final { return "LogicalMatch"; }

    std::shared_ptr<BaseTableRef> base_table_ref_{};
    std::shared_ptr<MatchExpression> match_expr_{};
    std::shared_ptr<BaseExpression> filter_expression_{};
    std::shared_ptr<IndexReader> index_reader_;
    std::unique_ptr<QueryNode> query_tree_;
    float begin_threshold_;
    EarlyTermAlgo early_term_algo_{EarlyTermAlgo::kAuto};
    u32 top_n_{1};

    std::shared_ptr<CommonQueryFilter> common_query_filter_{};
    MinimumShouldMatchOption minimum_should_match_option_{};
    RankFeaturesOption rank_features_option_{};
    f32 score_threshold_{};
    FulltextSimilarity ft_similarity_{FulltextSimilarity::kBM25};
    BM25Params bm25_params_;
};

} // namespace infinity
