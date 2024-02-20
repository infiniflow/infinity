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
import query_visitor;

namespace infinity {

struct QueryNode {
    String term_;
    float weight_;
    bool position_{false};
};

class MultiQuery : public QueryNode {
public:
    MultiQuery() = default;

    ~MultiQuery() = default;

    MultiQuery(const MultiQuery &other) = delete;

    MultiQuery &operator=(const MultiQuery &other) = delete;

    const Vector<UniquePtr<QueryNode>> &GetChildren() const { return children_; }

    QueryNode &Add(UniquePtr<QueryNode> &&node) { return *children_.emplace_back(std::move(node)); }

    template <typename T>
    void Accept(QueryVisitor &visitor) {
        visitor.Visit(static_cast<T &>(*this));
    }

protected:
    friend class QueryBuilder;
    Vector<UniquePtr<QueryNode>> children_;
};

class And : public MultiQuery {};
class AndNot : public MultiQuery {};
class Or : public MultiQuery {};
class Wand : public MultiQuery {};
class Phrase : public MultiQuery {};

} // namespace infinity