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

export module query_node;

import stl;
import index_defines;
import index_config;
import query_visitor;

namespace infinity {

export class Node {
public:
    Node() = default;
    virtual ~Node() = default;
};

export class QueryNode : public Node {
public:
    QueryNode() = default;

    virtual ~QueryNode() = default;

    QueryNode(const QueryNode &other) = delete;

    QueryNode &operator=(const QueryNode &other) = delete;

    const Vector<Node *> GetChildren() const { return children_; }

protected:
    Vector<Node *> children_;
};

export template <typename T>
class NodeWrapper : QueryNode {
public:
    NodeWrapper() = default;

    ~NodeWrapper() = default;

    void Accept(QueryVisitor<T> &visitor) { visitor.Visit(static_cast<T &>(*this)); }
};

export class And : public NodeWrapper<And> {};
export class AndNot : public NodeWrapper<AndNot> {};
export class Or : public NodeWrapper<Or> {};
export class Wand : public NodeWrapper<Wand> {};
export class Phrase : public NodeWrapper<Phrase> {};

} // namespace infinity