//  Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

module;

// #include "query_node.h"
#include <memory>

export module infinity_core:query_node;

import :stl;
import :meta_info;
import :column_index_reader;
import :doc_iterator;
import :parse_fulltext_options;
import :parse_fulltext_options;

namespace infinity {

export enum class QueryNodeType : char {
    // may be used in test cases,
    // but should not appear in parser output:
    INVALID,
    // filter
    FILTER,
    // may appear in parser output,
    // but should not appear in optimized query tree:
    NOT,
    // may appear in optimized query tree:
    TERM,
    PHRASE,
    AND,
    AND_NOT,
    OR,
    KEYWORD,
    // unimplemented:
    PREFIX_TERM,
    SUFFIX_TERM,
    SUBSTRING_TERM,
};

export struct CreateSearchParams {
    const TableInfo *table_info;
    const IndexReader *index_reader;
    EarlyTermAlgo early_term_algo;
    FulltextSimilarity ft_similarity;
    const BM25Params &bm25_params;
    u32 minimum_should_match;
    u32 topn;
    const Vector<String> &index_names_;

    [[nodiscard]] CreateSearchParams RemoveMSM() const {
        CreateSearchParams copy_value = *this;
        copy_value.minimum_should_match = 0;
        return copy_value;
    }
};

export std::string QueryNodeTypeToString(QueryNodeType type);

export struct QueryNode {
    QueryNodeType type_;
    float weight_ = 1.0f;

    QueryNode() = delete;
    explicit QueryNode(QueryNodeType type) : type_(type) {}
    virtual ~QueryNode() = default;

    QueryNodeType GetType() const { return type_; }
    float GetWeight() const { return weight_; }
    void MultiplyWeight(float factor) { weight_ *= factor; }
    void ResetWeight() { weight_ = 1.0f; }

    virtual u32 LeafCount() const = 0;

    // will do two jobs:
    // 1. push down the weight to the leaf term node
    // 2. optimize the query tree
    static UniquePtr<QueryNode> GetOptimizedQueryTree(UniquePtr<QueryNode> root);
    virtual void FilterOptimizeQueryTree();

    // recursively multiply and push down the weight to the leaf term nodes
    virtual void PushDownWeight(float factor = 1.0f) = 0;
    // create the iterator from the query tree, need to be called after optimization
    // is_top_level: used to determine whether to use early termination
    virtual UniquePtr<DocIterator> CreateSearch(CreateSearchParams params, bool is_top_level = true) const = 0;
    // print the query tree, for debugging
    virtual void PrintTree(std::ostream &os, const String &prefix = "", bool is_final = true) const = 0;

    virtual void GetQueryColumnsTerms(Vector<String> &columns, Vector<String> &terms) const = 0;
};

export struct TermQueryNode : public QueryNode {
    std::string term_;
    std::string column_;
    bool position_{false};

    TermQueryNode() : QueryNode(QueryNodeType::TERM) {}

    u32 LeafCount() const override { return 1; }
    void PushDownWeight(float factor) override { MultiplyWeight(factor); }
    UniquePtr<DocIterator> CreateSearch(CreateSearchParams params, bool is_top_level) const override;
    void PrintTree(std::ostream &os, const String &prefix, bool is_final) const override;
    void GetQueryColumnsTerms(Vector<String> &columns, Vector<String> &terms) const override;
};

export struct RankFeatureQueryNode : public QueryNode {
    std::string term_;
    std::string column_;
    float boost_;

    RankFeatureQueryNode() : QueryNode(QueryNodeType::TERM) {}

    u32 LeafCount() const override { return 1; }
    void PushDownWeight(float factor) override { MultiplyWeight(factor); }
    UniquePtr<DocIterator> CreateSearch(CreateSearchParams params, bool is_top_level) const override;
    void PrintTree(std::ostream &os, const String &prefix, bool is_final) const override;
    void GetQueryColumnsTerms(Vector<String> &columns, Vector<String> &terms) const override;
};

export struct PhraseQueryNode final : public QueryNode {
    Vector<std::string> terms_;
    String column_;
    u32 slop_;

    PhraseQueryNode() : QueryNode(QueryNodeType::PHRASE) {}

    u32 LeafCount() const override { return 1; }
    void PushDownWeight(float factor) override { MultiplyWeight(factor); }
    UniquePtr<DocIterator> CreateSearch(CreateSearchParams params, bool is_top_level) const override;
    void PrintTree(std::ostream &os, const String &prefix, bool is_final) const override;
    void GetQueryColumnsTerms(Vector<String> &columns, Vector<String> &terms) const override;

    void AddTerm(const String &term) { terms_.emplace_back(term); }
};

export struct MultiQueryNode : public QueryNode {
    Vector<UniquePtr<QueryNode>> children_;

    MultiQueryNode(QueryNodeType type) : QueryNode(type) {}

    void Add(UniquePtr<QueryNode> &&node) { children_.emplace_back(std::move(node)); }
    u32 LeafCount() const override;
    void PushDownWeight(float factor) override {
        // no need to update weight for MultiQueryNode, because it will be reset to 1.0
        factor *= GetWeight();
        for (auto &child : children_) {
            child->PushDownWeight(factor);
        }
        ResetWeight();
    }
    UniquePtr<QueryNode> GetNewOptimizedQueryTree();
    virtual UniquePtr<QueryNode> InnerGetNewOptimizedQueryTree() = 0;
    void PrintTree(std::ostream &os, const String &prefix, bool is_final) const final;
    void GetQueryColumnsTerms(Vector<String> &columns, Vector<String> &terms) const final;
};

// "NotQueryNode" will be generated by parser
// need to be optimized to AndNotQueryNode
// otherwise, query statement is invalid
export struct NotQueryNode final : public MultiQueryNode {
    NotQueryNode() : MultiQueryNode(QueryNodeType::NOT) {}
    u32 LeafCount() const override { return 0; }
    UniquePtr<QueryNode> InnerGetNewOptimizedQueryTree() override;
    UniquePtr<DocIterator> CreateSearch(CreateSearchParams params, bool is_top_level) const override;
};

export struct AndQueryNode final : public MultiQueryNode {
    AndQueryNode() : MultiQueryNode(QueryNodeType::AND) {}
    UniquePtr<QueryNode> InnerGetNewOptimizedQueryTree() override;
    UniquePtr<DocIterator> CreateSearch(CreateSearchParams params, bool is_top_level) const override;
};

export struct AndNotQueryNode final : public MultiQueryNode {
    AndNotQueryNode() : MultiQueryNode(QueryNodeType::AND_NOT) {}
    uint32_t LeafCount() const override { return children_[0]->LeafCount(); }
    UniquePtr<QueryNode> InnerGetNewOptimizedQueryTree() override;
    UniquePtr<DocIterator> CreateSearch(CreateSearchParams params, bool is_top_level) const override;
};

export struct OrQueryNode final : public MultiQueryNode {
    OrQueryNode() : MultiQueryNode(QueryNodeType::OR) {}
    UniquePtr<QueryNode> InnerGetNewOptimizedQueryTree() override;
    UniquePtr<DocIterator> CreateSearch(CreateSearchParams params, bool is_top_level) const override;
};

export struct RankFeaturesQueryNode final : public MultiQueryNode {
    RankFeaturesQueryNode() : MultiQueryNode(QueryNodeType::OR) {}
    UniquePtr<QueryNode> InnerGetNewOptimizedQueryTree() override;
    UniquePtr<DocIterator> CreateSearch(CreateSearchParams params, bool is_top_level) const override;
};

export struct KeywordQueryNode final : public MultiQueryNode {
    KeywordQueryNode() : MultiQueryNode(QueryNodeType::KEYWORD) {}
    void PushDownWeight(float factor) override { MultiplyWeight(factor); }
    UniquePtr<QueryNode> InnerGetNewOptimizedQueryTree() override;
    UniquePtr<DocIterator> CreateSearch(CreateSearchParams params, bool is_top_level) const override;
};

// export using infinity::QueryNodeType;
// export using infinity::QueryNodeTypeToString;
// export using infinity::CreateSearchParams;
// export using infinity::QueryNode;
// export using infinity::TermQueryNode;
// export using infinity::MultiQueryNode;
// export using infinity::AndQueryNode;
// export using infinity::AndNotQueryNode;
// export using infinity::OrQueryNode;
// export using infinity::NotQueryNode;
// export using infinity::PhraseQueryNode;
// export using infinity::RankFeaturesQueryNode;
// export using infinity::KeywordQueryNode;
// export using infinity::RankFeatureQueryNode;

// unimplemented
// export using infinity::WandQueryNode;
// export using infinity::PrefixTermQueryNode;
// export using infinity::SuffixTermQueryNode;
// export using infinity::SubstringTermQueryNode;

} // namespace infinity
