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

module infinity_core:query_builder.impl;

import :query_builder;
import :doc_iterator;
import :column_index_reader;
import :index_base;
import :infinity_exception;
import :query_node;
import :base_table_ref;
import :term_doc_iterator;
import :logger;
import :parse_fulltext_options;
import :query_node;

import std;
import third_party;

import internal_types;

namespace infinity {

void QueryBuilder::Init(std::shared_ptr<IndexReader> index_reader) { index_reader_ = std::move(index_reader); }

QueryBuilder::~QueryBuilder() {}

std::unique_ptr<DocIterator> QueryBuilder::CreateSearch(FullTextQueryContext &context) {
    // Optimize the query tree.
    if (!context.optimized_query_tree_) {
        context.optimized_query_tree_ = QueryNode::GetOptimizedQueryTree(std::move(context.query_tree_));
        if (!context.minimum_should_match_option_.empty()) {
            const auto leaf_count = context.optimized_query_tree_->LeafCount();
            context.minimum_should_match_ = GetMinimumShouldMatchParameter(context.minimum_should_match_option_, leaf_count);
        }
    }
    // Create the iterator from the query tree.
    const CreateSearchParams params{table_info_.get(),
                                    index_reader_.get(),
                                    context.early_term_algo_,
                                    context.ft_similarity_,
                                    context.bm25_params_,
                                    context.minimum_should_match_,
                                    context.topn_};
    if (!context.rank_features_option_.empty()) {
        auto rank_features_node = std::make_unique<RankFeaturesQueryNode>();
        for (auto rank_feature : context.rank_features_option_) {
            auto rank_feature_node = std::make_unique<RankFeatureQueryNode>();
            rank_feature_node->term_ = rank_feature.feature_;
            rank_feature_node->column_ = rank_feature.field_;
            rank_feature_node->boost_ = rank_feature.boost_;
            rank_features_node->Add(std::move(rank_feature_node));
        }
        auto query_tree = std::make_unique<OrQueryNode>();
        query_tree->Add(std::move(context.optimized_query_tree_));
        query_tree->Add(std::move(rank_features_node));
        context.optimized_query_tree_ = std::move(query_tree);
    }
    auto result = context.optimized_query_tree_->CreateSearch(params);
#ifdef INFINITY_DEBUG
    {
        std::ostringstream oss;
        oss << "DocIterator:\n";
        if (result) {
            result->PrintTree(oss);
        } else {
            oss << "Empty tree!\n";
        }
        LOG_DEBUG(std::move(oss).str());
    }
#endif
    return result;
}

} // namespace infinity
