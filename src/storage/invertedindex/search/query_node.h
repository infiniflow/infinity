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

#pragma once

#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace infinity {

enum class QueryNodeType : char {
    INVALID,
    TERM,
    AND,
    AND_NOT,
    OR,
    WAND,
    PHRASE,
    PREFIX_TERM,
    SUFFIX_TERM,
    SUBSTRING_TERM,
    NOT,
};

std::string QueryNodeTypeToString(QueryNodeType type);

struct TableEntry;
struct IndexReader;
class Scorer;
class DocIterator;

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

    static std::unique_ptr<QueryNode> GetOptimizedQueryTree(std::unique_ptr<QueryNode> root) {
        root->PushDownWeight();
        return root->OptimizeInPlace(root);
    }

    virtual void PushDownWeight() = 0;
    virtual std::unique_ptr<QueryNode> OptimizeInPlace(std::unique_ptr<QueryNode> &self_node) = 0;
    virtual std::unique_ptr<DocIterator>
    CreateSearch(const TableEntry *table_entry, IndexReader &index_reader, std::unique_ptr<Scorer> &scorer) const = 0;
    virtual void PrintTree(std::ostream &os, const std::string &prefix = "", bool is_final = true) const = 0;
};

struct TermQueryNode final : public QueryNode {
    std::string term_;
    std::string column_;
    bool position_{false};
    TermQueryNode() : QueryNode(QueryNodeType::TERM) {}

    void PushDownWeight() final {}
    std::unique_ptr<QueryNode> OptimizeInPlace(std::unique_ptr<QueryNode> &self_node) final { return std::move(self_node); }
    std::unique_ptr<DocIterator> CreateSearch(const TableEntry *table_entry, IndexReader &index_reader, std::unique_ptr<Scorer> &scorer) const final;
    void PrintTree(std::ostream &os, const std::string &prefix, bool is_final) const final;
};

struct MultiQueryNode : public QueryNode {
    std::vector<std::unique_ptr<QueryNode>> children_;

    MultiQueryNode(QueryNodeType type) : QueryNode(type) {}
    void Add(std::unique_ptr<QueryNode> &&node) { children_.emplace_back(std::move(node)); }

    void PushDownWeight() final {
        float factor = GetWeight();
        for (auto &child : children_) {
            child->MultiplyWeight(factor);
            child->PushDownWeight();
        }
        ResetWeight();
    }
    std::unique_ptr<QueryNode> OptimizeInPlace(std::unique_ptr<QueryNode> &self_node) final {
        for (auto &child : children_) {
            std::unique_ptr<QueryNode> new_child = child->OptimizeInPlace(child);
            child = std::move(new_child);
        }
        return OptimizeInPlaceInner(self_node);
    }
    virtual std::unique_ptr<QueryNode> OptimizeInPlaceInner(std::unique_ptr<QueryNode> &self_node) = 0;
    void PrintTree(std::ostream &os, const std::string &prefix, bool is_final) const final;
};

struct AndQueryNode final : public MultiQueryNode {
    AndQueryNode() : MultiQueryNode(QueryNodeType::AND) {}
    std::unique_ptr<QueryNode> OptimizeInPlaceInner(std::unique_ptr<QueryNode> &) final;
    std::unique_ptr<DocIterator> CreateSearch(const TableEntry *table_entry, IndexReader &index_reader, std::unique_ptr<Scorer> &scorer) const final;
};
struct AndNotQueryNode final : public MultiQueryNode {
    AndNotQueryNode() : MultiQueryNode(QueryNodeType::AND_NOT) {}
    std::unique_ptr<QueryNode> OptimizeInPlaceInner(std::unique_ptr<QueryNode> &) final;
    std::unique_ptr<DocIterator> CreateSearch(const TableEntry *table_entry, IndexReader &index_reader, std::unique_ptr<Scorer> &scorer) const final;
};
struct OrQueryNode final : public MultiQueryNode {
    OrQueryNode() : MultiQueryNode(QueryNodeType::OR) {}
    std::unique_ptr<QueryNode> OptimizeInPlaceInner(std::unique_ptr<QueryNode> &) final;
    std::unique_ptr<DocIterator> CreateSearch(const TableEntry *table_entry, IndexReader &index_reader, std::unique_ptr<Scorer> &scorer) const final;
};
struct WandQueryNode final : public MultiQueryNode {
    WandQueryNode() : MultiQueryNode(QueryNodeType::WAND) {}
    std::unique_ptr<QueryNode> OptimizeInPlaceInner(std::unique_ptr<QueryNode> &self_node) final;
    std::unique_ptr<DocIterator> CreateSearch(const TableEntry *table_entry, IndexReader &index_reader, std::unique_ptr<Scorer> &scorer) const final;
};
struct PhraseQueryNode final : public MultiQueryNode {
    PhraseQueryNode() : MultiQueryNode(QueryNodeType::PHRASE) {}
    std::unique_ptr<QueryNode> OptimizeInPlaceInner(std::unique_ptr<QueryNode> &self_node) final;
    std::unique_ptr<DocIterator> CreateSearch(const TableEntry *table_entry, IndexReader &index_reader, std::unique_ptr<Scorer> &scorer) const final;
};
struct PrefixTermQueryNode final : public MultiQueryNode {
    PrefixTermQueryNode() : MultiQueryNode(QueryNodeType::PREFIX_TERM) {}
    std::unique_ptr<QueryNode> OptimizeInPlaceInner(std::unique_ptr<QueryNode> &self_node) final;
    std::unique_ptr<DocIterator> CreateSearch(const TableEntry *table_entry, IndexReader &index_reader, std::unique_ptr<Scorer> &scorer) const final;
};
struct SuffixTermQueryNode final : public MultiQueryNode {
    SuffixTermQueryNode() : MultiQueryNode(QueryNodeType::SUFFIX_TERM) {}
    std::unique_ptr<QueryNode> OptimizeInPlaceInner(std::unique_ptr<QueryNode> &self_node) final;
    std::unique_ptr<DocIterator> CreateSearch(const TableEntry *table_entry, IndexReader &index_reader, std::unique_ptr<Scorer> &scorer) const final;
};
struct SubstringTermQueryNode final : public MultiQueryNode {
    SubstringTermQueryNode() : MultiQueryNode(QueryNodeType::SUBSTRING_TERM) {}
    std::unique_ptr<QueryNode> OptimizeInPlaceInner(std::unique_ptr<QueryNode> &self_node) final;
    std::unique_ptr<DocIterator> CreateSearch(const TableEntry *table_entry, IndexReader &index_reader, std::unique_ptr<Scorer> &scorer) const final;
};
// "NotQueryNode" will be generated by parser
// need to be optimized to AndNotQueryNode
// otherwise, query statement is invalid
struct NotQueryNode final : public MultiQueryNode {
    NotQueryNode() : MultiQueryNode(QueryNodeType::NOT) {}
    std::unique_ptr<QueryNode> OptimizeInPlaceInner(std::unique_ptr<QueryNode> &) final;
    std::unique_ptr<DocIterator> CreateSearch(const TableEntry *table_entry, IndexReader &index_reader, std::unique_ptr<Scorer> &scorer) const final;
};

} // namespace infinity