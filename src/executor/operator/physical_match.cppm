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

// #include "storage/invertedindex/search/query_node.h"

export module infinity_core:physical_match;

import :stl;
import :physical_operator;
import :parse_fulltext_options;
import :doc_iterator;
import :base_table_ref;
import :common_query_filter;
import :meta_info;
import :query_node;

namespace infinity {
class BaseExpression;
struct BM25Params;
enum class FulltextSimilarity;
class MatchExpression;

class ResultCacheManager;
struct DataBlock;

export class PhysicalMatch final : public PhysicalOperator {
public:
    explicit PhysicalMatch(u64 id,
                           SharedPtr<BaseTableRef> base_table_ref,
                           SharedPtr<MatchExpression> match_expr,
                           SharedPtr<IndexReader> index_reader,
                           UniquePtr<QueryNode> &&query_tree,
                           float begin_threshold,
                           EarlyTermAlgo early_term_algo,
                           u32 top_n,
                           const SharedPtr<CommonQueryFilter> &common_query_filter,
                           MinimumShouldMatchOption &&minimum_should_match_option,
                           RankFeaturesOption &&rank_features_option,
                           f32 score_threshold,
                           FulltextSimilarity ft_similarity,
                           const BM25Params &bm25_params,
                           u64 match_table_index,
                           SharedPtr<Vector<LoadMeta>> load_metas,
                           bool cache_result);

    ~PhysicalMatch() override;

    void Init(QueryContext* query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) override;

    SharedPtr<Vector<String>> GetOutputNames() const override;

    SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const override;

    SizeT TaskletCount() override { return 1; }

    void FillingTableRefs(HashMap<SizeT, SharedPtr<BaseTableRef>> &table_refs) override {
        table_refs.insert({base_table_ref_->table_index_, base_table_ref_});
    }

    String ToString(i64 &space) const;

    [[nodiscard]] inline String TableAlias() const { return base_table_ref_->alias_; }

    [[nodiscard]] inline TableInfo *table_info() const { return base_table_ref_->table_info_.get(); }

    [[nodiscard]] inline u64 table_index() const { return table_index_; }

    [[nodiscard]] inline const SharedPtr<MatchExpression> &match_expr() const { return match_expr_; }

    SharedPtr<BaseExpression> filter_expression() const { return common_query_filter_->original_filter_; }

    [[nodiscard]] inline const CommonQueryFilter *common_query_filter() const { return common_query_filter_.get(); }

    const SharedPtr<BaseTableRef> &base_table_ref() const { return base_table_ref_; }

    SizeT top_n() const { return top_n_; }

private:
    void AddCache(QueryContext *query_context, ResultCacheManager *cache_mgr, const Vector<UniquePtr<DataBlock>> &output_data_blocks);

private:
    u64 table_index_ = 0;
    SharedPtr<BaseTableRef> base_table_ref_;
    SharedPtr<MatchExpression> match_expr_;
    SharedPtr<IndexReader> index_reader_;
    UniquePtr<QueryNode> query_tree_;
    float begin_threshold_;
    EarlyTermAlgo early_term_algo_{EarlyTermAlgo::kAuto};
    u32 top_n_{1};

    // for filter
    SharedPtr<CommonQueryFilter> common_query_filter_;
    // for minimum_should_match
    MinimumShouldMatchOption minimum_should_match_option_{};
    // for rank features
    RankFeaturesOption rank_features_option_{};
    f32 score_threshold_{};
    FulltextSimilarity ft_similarity_{FulltextSimilarity::kBM25};
    BM25Params bm25_params_;

    bool ExecuteInner(QueryContext *query_context, OperatorState *operator_state);
};

} // namespace infinity
