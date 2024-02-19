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

#include "query_builder.h"
#include "query.h"

import stl;
import index_defines;
import index_config;

namespace infinity {
QueryBuilder::QueryBuilder() {}

QueryBuilder::~QueryBuilder() {}

UniquePtr<QueryNode> QueryBuilder::Build() { return std::move(root_); }

void QueryBuilder::AddTerm(const String &term, float weight) {
    QueryNode *term_query = new QueryNode;
    term_query->term_ = term;
    term_query->weight_ = weight;
    AddTermQuery(term_query);
}

void QueryBuilder::AddAnd(int child_count) { AddMultiQuery(new And, child_count); }

void QueryBuilder::AddAndNot(int child_count) { AddMultiQuery(new AndNot, child_count); }

void QueryBuilder::AddOr(int child_count) { AddMultiQuery(new Or, child_count); }

void QueryBuilder::AddWAnd(int child_count) { AddMultiQuery(new Wand, child_count); }

void QueryBuilder::AddPhrase(int child_count) { AddMultiQuery(new Phrase, child_count); }

void QueryBuilder::AddTermQuery(QueryNode *n) {
    UniquePtr<QueryNode> node(n);
    if (nodes_.empty()) {
        if (!root_) {
            root_ = std::move(node);
        }
        return;
    }
    nodes_.top().node_->Add(std::move(node));
    if (--nodes_.top().left_child_count_ == 0) {
        QueryNode *completed(nodes_.top().node_);
        nodes_.pop();
        AddTermQuery(completed);
    }
}

void QueryBuilder::AddMultiQuery(MultiQuery *n, int child_count) {
    UniquePtr<MultiQuery> node(n);
    if (!root_) {
        node->children_.reserve(child_count);
        nodes_.push(NodeInfo(node.release(), child_count));
        if (child_count == 0) {
            QueryNode *completed(nodes_.top().node_);
            nodes_.pop();
            AddTermQuery(completed);
        }
    }
}

} // namespace infinity