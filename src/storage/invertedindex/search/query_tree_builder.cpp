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

#include "query_tree_builder.h"
#include "query_node.h"

import stl;
import index_defines;
import index_config;

namespace infinity {
QueryTreeBuilder::QueryTreeBuilder() {}

QueryTreeBuilder::~QueryTreeBuilder() {}

UniquePtr<QueryNode> QueryTreeBuilder::Build() { return std::move(root_); }

void QueryTreeBuilder::AddTerm(const String &term, const String &column, float weight) {
    TermQueryNode *term_query = new TermQueryNode;
    term_query->term_ = term;
    term_query->column_ = column;
    term_query->weight_ = weight;
    AddCompleteNode(term_query);
}

void QueryTreeBuilder::AddAnd(int child_count) { AddIntermediateNode(new And, child_count); }

void QueryTreeBuilder::AddAndNot(int child_count) { AddIntermediateNode(new AndNot, child_count); }

void QueryTreeBuilder::AddOr(int child_count) { AddIntermediateNode(new Or, child_count); }

void QueryTreeBuilder::AddWAnd(int child_count) { AddIntermediateNode(new Wand, child_count); }

void QueryTreeBuilder::AddPhrase(int child_count) { AddIntermediateNode(new Phrase, child_count); }

void QueryTreeBuilder::AddCompleteNode(QueryNode *n) {
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
        AddCompleteNode(completed);
    }
}

void QueryTreeBuilder::AddIntermediateNode(MultiQueryNode *n, int child_count) {
    UniquePtr<MultiQueryNode> node(n);
    if (!root_) {
        node->children_.reserve(child_count);
        nodes_.push(NodeInfo(node.release(), child_count));
        if (child_count == 0) {
            QueryNode *completed(nodes_.top().node_);
            nodes_.pop();
            AddCompleteNode(completed);
        }
    }
}

} // namespace infinity