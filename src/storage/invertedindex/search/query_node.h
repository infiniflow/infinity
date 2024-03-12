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

import stl;

namespace infinity {

class QueryVisitor;

struct QueryNode {
    QueryNode() = default;
    virtual ~QueryNode() = default;

    virtual void Accept(QueryVisitor &visitor) = 0;
};

struct TermQueryNode : public QueryNode {
    String term_;
    String column_;
    float weight_;
    bool position_{false};
    void Accept(QueryVisitor &visitor) override;
};

class MultiQueryNode : public QueryNode {
public:
    MultiQueryNode() = default;

    ~MultiQueryNode() = default;

    MultiQueryNode(const MultiQueryNode &other) = delete;

    MultiQueryNode &operator=(const MultiQueryNode &other) = delete;

    const Vector<UniquePtr<QueryNode>> &GetChildren() const { return children_; }

    QueryNode &Add(UniquePtr<QueryNode> &&node) { return *children_.emplace_back(std::move(node)); }

protected:
    friend class QueryTreeBuilder;
    Vector<UniquePtr<QueryNode>> children_;
};

class And : public MultiQueryNode {
public:
    void Accept(QueryVisitor &visitor) override;
};
class AndNot : public MultiQueryNode {
public:
    void Accept(QueryVisitor &visitor) override;
};
class Or : public MultiQueryNode {
public:
    void Accept(QueryVisitor &visitor) override;
};
class Wand : public MultiQueryNode {
public:
    void Accept(QueryVisitor &visitor) override;
};
class Phrase : public MultiQueryNode {
public:
    void Accept(QueryVisitor &visitor) override;
};
class PrefixTerm : public MultiQueryNode {
public:
    void Accept(QueryVisitor &visitor) override;
};
class SuffixTerm : public MultiQueryNode {
public:
    void Accept(QueryVisitor &visitor) override;
};
class SubstringTerm : public MultiQueryNode {
public:
    void Accept(QueryVisitor &visitor) override;
};

} // namespace infinity