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

#ifndef QUERY_NODE_H
#define QUERY_NODE_H

#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace infinity {

enum class QueryNodeType : char {
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
    // unimplemented:
    WAND,
    PREFIX_TERM,
    SUFFIX_TERM,
    SUBSTRING_TERM,
};

std::string QueryNodeTypeToString(QueryNodeType type);

struct TableEntry;
struct IndexReader;
class Scorer;
class DocIterator;
class EarlyTerminateIterator;
enum class EarlyTermAlg;

// step 1. get the query tree from parser
// step 2. push down the weight to the leaf term node
// step 3. optimize the query tree
// step 4. create the iterator from the query tree

// QueryNode is the base class for all nodes in the query tree
// It contains a weight for the node
struct QueryNode {
    QueryNodeType type_;
    float weight_ = 1.0f;

    QueryNode() = delete;
    explicit QueryNode(QueryNodeType type) : type_(type) {}
    virtual ~QueryNode() = default;

    QueryNodeType GetType() const { return type_; }
    float GetWeight() const { return weight_; }
    void MultiplyWeight(float factor) { weight_ *= factor; }
    void ResetWeight() { weight_ = 1.0f; }

    // will do two jobs:
    // 1. push down the weight to the leaf term node
    // 2. optimize the query tree
    static std::unique_ptr<QueryNode> GetOptimizedQueryTree(std::unique_ptr<QueryNode> root);
    virtual void FilterOptimizeQueryTree();

    // recursively multiply and push down the weight to the leaf term nodes
    virtual void PushDownWeight(float factor = 1.0f) = 0;
    // create the iterator from the query tree, need to be called after optimization
    virtual std::unique_ptr<DocIterator> CreateSearch(const TableEntry *table_entry, IndexReader &index_reader, Scorer *scorer) const = 0;
    virtual std::unique_ptr<EarlyTerminateIterator>
    CreateEarlyTerminateSearch(const TableEntry *table_entry, IndexReader &index_reader, Scorer *scorer, EarlyTermAlg early_term_alg) const = 0;
    // print the query tree, for debugging
    virtual void PrintTree(std::ostream &os, const std::string &prefix = "", bool is_final = true) const = 0;
};

struct TermQueryNode final : public QueryNode {
    std::string term_;
    std::string column_;
    bool position_{false};

    TermQueryNode() : QueryNode(QueryNodeType::TERM) {}

    void PushDownWeight(float factor) override { MultiplyWeight(factor); }
    std::unique_ptr<DocIterator> CreateSearch(const TableEntry *table_entry, IndexReader &index_reader, Scorer *scorer) const override;
    std::unique_ptr<EarlyTerminateIterator>
    CreateEarlyTerminateSearch(const TableEntry *table_entry, IndexReader &index_reader, Scorer *scorer, EarlyTermAlg early_term_alg) const override;
    void PrintTree(std::ostream &os, const std::string &prefix, bool is_final) const override;
};

struct PhraseQueryNode final : public QueryNode {
    std::vector<std::string> terms_;
    std::string column_;
    uint32_t slop_;

    PhraseQueryNode() : QueryNode(QueryNodeType::PHRASE) {}

    void PushDownWeight(float factor) override { MultiplyWeight(factor); }
    std::unique_ptr<DocIterator> CreateSearch(const TableEntry *table_entry, IndexReader &index_reader, Scorer *scorer) const override;
    std::unique_ptr<EarlyTerminateIterator>
    CreateEarlyTerminateSearch(const TableEntry *table_entry, IndexReader &index_reader, Scorer *scorer, EarlyTermAlg early_term_alg) const override;
    void PrintTree(std::ostream &os, const std::string &prefix, bool is_final) const override;

    void AddTerm(const std::string &term) { terms_.emplace_back(term); }
};

struct MultiQueryNode : public QueryNode {
    std::vector<std::unique_ptr<QueryNode>> children_;

    MultiQueryNode(QueryNodeType type) : QueryNode(type) {}

    void Add(std::unique_ptr<QueryNode> &&node) { children_.emplace_back(std::move(node)); }
    void PushDownWeight(float factor) final {
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
};

// "NotQueryNode" will be generated by parser
// need to be optimized to AndNotQueryNode
// otherwise, query statement is invalid
struct NotQueryNode final : public MultiQueryNode {
    NotQueryNode() : MultiQueryNode(QueryNodeType::NOT) {}
    std::unique_ptr<QueryNode> InnerGetNewOptimizedQueryTree() override;
    std::unique_ptr<DocIterator> CreateSearch(const TableEntry *table_entry, IndexReader &index_reader, Scorer *scorer) const override;
    std::unique_ptr<EarlyTerminateIterator>
    CreateEarlyTerminateSearch(const TableEntry *table_entry, IndexReader &index_reader, Scorer *scorer, EarlyTermAlg early_term_alg) const override;
};
struct AndQueryNode final : public MultiQueryNode {
    AndQueryNode() : MultiQueryNode(QueryNodeType::AND) {}
    std::unique_ptr<QueryNode> InnerGetNewOptimizedQueryTree() override;
    std::unique_ptr<DocIterator> CreateSearch(const TableEntry *table_entry, IndexReader &index_reader, Scorer *scorer) const override;
    std::unique_ptr<EarlyTerminateIterator>
    CreateEarlyTerminateSearch(const TableEntry *table_entry, IndexReader &index_reader, Scorer *scorer, EarlyTermAlg early_term_alg) const override;
};
struct AndNotQueryNode final : public MultiQueryNode {
    AndNotQueryNode() : MultiQueryNode(QueryNodeType::AND_NOT) {}
    std::unique_ptr<QueryNode> InnerGetNewOptimizedQueryTree() override;
    std::unique_ptr<DocIterator> CreateSearch(const TableEntry *table_entry, IndexReader &index_reader, Scorer *scorer) const override;
    std::unique_ptr<EarlyTerminateIterator>
    CreateEarlyTerminateSearch(const TableEntry *table_entry, IndexReader &index_reader, Scorer *scorer, EarlyTermAlg early_term_alg) const override;
};
struct OrQueryNode final : public MultiQueryNode {
    OrQueryNode() : MultiQueryNode(QueryNodeType::OR) {}
    std::unique_ptr<QueryNode> InnerGetNewOptimizedQueryTree() override;
    std::unique_ptr<DocIterator> CreateSearch(const TableEntry *table_entry, IndexReader &index_reader, Scorer *scorer) const override;
    std::unique_ptr<EarlyTerminateIterator>
    CreateEarlyTerminateSearch(const TableEntry *table_entry, IndexReader &index_reader, Scorer *scorer, EarlyTermAlg early_term_alg) const override;
};

// unimplemented
struct WandQueryNode;
// struct PhraseQueryNode;
struct PrefixTermQueryNode;
struct SuffixTermQueryNode;
struct SubstringTermQueryNode;

} // namespace infinity

#endif // QUERY_NODE_H