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

#include "query_tree_builder.h"

module query_builder;

import stl;
import doc_iterator;
import term_queries;
import column_index_reader;
import query_visitor;
namespace infinity {
QueryBuilder::QueryBuilder() {}

QueryBuilder::~QueryBuilder() {}

UniquePtr<DocIterator> QueryBuilder::CreateSearch(QueryContext &context) {
    QueryVisitor visitor;
    context.query_tree_->Accept(visitor);
    UniquePtr<TermQuery> root = visitor.Build();
    root = TermQuery::Optimize(std::move(root));
    return root->CreateSearch();
}
} // namespace infinity