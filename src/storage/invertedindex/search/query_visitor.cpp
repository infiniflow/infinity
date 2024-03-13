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

/*
module;

#include "query_node.h"
#include <vector>

module query_visitor;

import stl;
import index_defines;
import term_queries;

namespace infinity {

UniquePtr<TermQuery> BuildQuery(QueryNode &node) {
    QueryVisitor visitor;
    node.Accept(visitor);
    UniquePtr<TermQuery> result = visitor.Build();
    return result;
}

template <typename NodeType>
void QueryVisitor::BuildMultiQuery(MultiQuery *q, NodeType &n) {
    UniquePtr<MultiQuery> query(q);
    const Vector<UniquePtr<QueryNode>> &children = n.GetChildren();
    for (u32 i = 0; i < children.size(); ++i) {
        query->AddChild(BuildQuery(*children[i]));
    }
    result_.reset(query.release());
}

template <>
void QueryVisitor::Visit<And>(And &node) {
    BuildMultiQuery(new AndQuery, node);
}

template <>
void QueryVisitor::Visit<AndNot>(AndNot &node) {
    BuildMultiQuery(new AndNotQuery, node);
}

template <>
void QueryVisitor::Visit<Or>(Or &node) {
    BuildMultiQuery(new OrQuery, node);
}

template <>
void QueryVisitor::Visit<Wand>(Wand &node) {
    BuildMultiQuery(new WandQuery, node);
}

template <>
void QueryVisitor::Visit<TermQueryNode>(TermQueryNode &node) {
    UniquePtr<TermQuery> query = MakeUnique<TermQuery>();
    query->SetColumn(node.column_);
    result_.reset(query.release());
}

template <>
void QueryVisitor::Visit<Phrase>(Phrase &node) {}

template <>
void QueryVisitor::Visit<PrefixTerm>(PrefixTerm &node) {}

template <>
void QueryVisitor::Visit<SuffixTerm>(SuffixTerm &node) {}

template <>
void QueryVisitor::Visit<SubstringTerm>(SubstringTerm &node) {}

} // namespace infinity
*/