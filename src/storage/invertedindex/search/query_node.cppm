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

export module infinity_core:query_node;

import :meta_info;
import :doc_iterator;
import :parse_fulltext_options;

import std.compat;

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

export std::string QueryNodeTypeToString(QueryNodeType type);

// Forward declarations for types that will be used in CreateSearchParams
struct IndexReader;
class Scorer;
class EarlyTerminateIterator;

export struct CreateSearchParams {
    const TableInfo *table_info;
    const IndexReader *index_reader;
    EarlyTermAlgo early_term_algo;
    FulltextSimilarity ft_similarity;
    const BM25Params &bm25_params;
    uint32_t minimum_should_match;
    uint32_t topn;

    [[nodiscard]] CreateSearchParams RemoveMSM() const {
        CreateSearchParams copy_value = *this;
        copy_value.minimum_should_match = 0;
        return copy_value;
    }
};

// step 1. get the query tree from parser
// step 2. push down the weight to the leaf term node
// step 3. optimize the query tree
// step 4. create the iterator from the query tree

// QueryNode is the base class for all nodes in the query tree
// It contains a weight for the node
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

    virtual uint32_t LeafCount() const = 0;

    // will do two jobs:
    // 1. push down the weight to the leaf term node
    // 2. optimize the query tree
    static std::unique_ptr<QueryNode> GetOptimizedQueryTree(std::unique_ptr<QueryNode> root);
    virtual void FilterOptimizeQueryTree();

    // recursively multiply and push down the weight to the leaf term nodes
    virtual void PushDownWeight(float factor = 1.0f) = 0;
    // create the iterator from the query tree, need to be called after optimization
    // is_top_level: used to determine whether to use early termination
    virtual std::unique_ptr<DocIterator> CreateSearch(CreateSearchParams params, bool is_top_level = true) const = 0;
    // print the query tree, for debugging
    virtual void PrintTree(std::ostream &os, const std::string &prefix = "", bool is_final = true) const = 0;

    virtual void GetQueryColumnsTerms(std::vector<std::string> &columns, std::vector<std::string> &terms) const = 0;
};

export struct TermQueryNode : public QueryNode {
    std::string term_;
    std::string column_;
    std::string index_;
    bool position_{false};

    TermQueryNode() : QueryNode(QueryNodeType::TERM) {}

    uint32_t LeafCount() const override { return 1; }
    void PushDownWeight(float factor) override { MultiplyWeight(factor); }
    std::unique_ptr<DocIterator> CreateSearch(CreateSearchParams params, bool is_top_level) const override;
    void PrintTree(std::ostream &os, const std::string &prefix, bool is_final) const override;
    void GetQueryColumnsTerms(std::vector<std::string> &columns, std::vector<std::string> &terms) const override;
};

export struct RankFeatureQueryNode : public QueryNode {
    std::string term_;
    std::string column_;
    std::string index_;
    float boost_;

    RankFeatureQueryNode() : QueryNode(QueryNodeType::TERM) {}

    uint32_t LeafCount() const override { return 1; }
    void PushDownWeight(float factor) override { MultiplyWeight(factor); }
    std::unique_ptr<DocIterator> CreateSearch(CreateSearchParams params, bool is_top_level) const override;
    void PrintTree(std::ostream &os, const std::string &prefix, bool is_final) const override;
    void GetQueryColumnsTerms(std::vector<std::string> &columns, std::vector<std::string> &terms) const override;
};

export struct PhraseQueryNode final : public QueryNode {
    std::vector<std::string> terms_;
    std::string column_;
    std::string index_;
    uint32_t slop_;

    PhraseQueryNode() : QueryNode(QueryNodeType::PHRASE) {}

    uint32_t LeafCount() const override { return 1; }
    void PushDownWeight(float factor) override { MultiplyWeight(factor); }
    std::unique_ptr<DocIterator> CreateSearch(CreateSearchParams params, bool is_top_level) const override;
    void PrintTree(std::ostream &os, const std::string &prefix, bool is_final) const override;
    void GetQueryColumnsTerms(std::vector<std::string> &columns, std::vector<std::string> &terms) const override;

    void AddTerm(const std::string &term) { terms_.emplace_back(term); }
};

export struct MultiQueryNode : public QueryNode {
    std::vector<std::unique_ptr<QueryNode>> children_;

    MultiQueryNode(QueryNodeType type) : QueryNode(type) {}

    void Add(std::unique_ptr<QueryNode> &&node) { children_.emplace_back(std::move(node)); }
    uint32_t LeafCount() const override;
    void PushDownWeight(float factor) override {
        // no need to update weight for MultiQueryNode, because it will be reset to 1.0
        factor *= GetWeight();
        for (auto &child : children_) {
            child->PushDownWeight(factor);
        }
        ResetWeight();
    }
    std::unique_ptr<QueryNode> GetNewOptimizedQueryTree();
    virtual std::unique_ptr<QueryNode> InnerGetNewOptimizedQueryTree() = 0;
    void PrintTree(std::ostream &os, const std::string &prefix, bool is_final) const final;
    void GetQueryColumnsTerms(std::vector<std::string> &columns, std::vector<std::string> &terms) const final;
};

// "NotQueryNode" will be generated by parser
// need to be optimized to AndNotQueryNode
// otherwise, query statement is invalid
export struct NotQueryNode final : public MultiQueryNode {
    NotQueryNode() : MultiQueryNode(QueryNodeType::NOT) {}
    uint32_t LeafCount() const override { return 0; }
    std::unique_ptr<QueryNode> InnerGetNewOptimizedQueryTree() override;
    std::unique_ptr<DocIterator> CreateSearch(CreateSearchParams params, bool is_top_level) const override;
};

export struct AndQueryNode final : public MultiQueryNode {
    AndQueryNode() : MultiQueryNode(QueryNodeType::AND) {}
    std::unique_ptr<QueryNode> InnerGetNewOptimizedQueryTree() override;
    std::unique_ptr<DocIterator> CreateSearch(CreateSearchParams params, bool is_top_level) const override;
};

export struct AndNotQueryNode final : public MultiQueryNode {
    AndNotQueryNode() : MultiQueryNode(QueryNodeType::AND_NOT) {}
    uint32_t LeafCount() const override { return children_[0]->LeafCount(); }
    std::unique_ptr<QueryNode> InnerGetNewOptimizedQueryTree() override;
    std::unique_ptr<DocIterator> CreateSearch(CreateSearchParams params, bool is_top_level) const override;
};

export struct OrQueryNode final : public MultiQueryNode {
    OrQueryNode() : MultiQueryNode(QueryNodeType::OR) {}
    std::unique_ptr<QueryNode> InnerGetNewOptimizedQueryTree() override;
    std::unique_ptr<DocIterator> CreateSearch(CreateSearchParams params, bool is_top_level) const override;
};

export struct RankFeaturesQueryNode final : public MultiQueryNode {
    RankFeaturesQueryNode() : MultiQueryNode(QueryNodeType::OR) {}
    std::unique_ptr<QueryNode> InnerGetNewOptimizedQueryTree() override;
    std::unique_ptr<DocIterator> CreateSearch(CreateSearchParams params, bool is_top_level) const override;
};

export struct KeywordQueryNode final : public MultiQueryNode {
    KeywordQueryNode() : MultiQueryNode(QueryNodeType::KEYWORD) {}
    void PushDownWeight(float factor) override { MultiplyWeight(factor); }
    std::unique_ptr<QueryNode> InnerGetNewOptimizedQueryTree() override;
    std::unique_ptr<DocIterator> CreateSearch(CreateSearchParams params, bool is_top_level) const override;
};

// unimplemented
struct PrefixTermQueryNode;
struct SuffixTermQueryNode;
struct SubstringTermQueryNode;

} // namespace infinity
