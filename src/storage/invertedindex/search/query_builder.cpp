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
import memory_pool;
import doc_iterator;
import term_queries;
import column_index_reader;
import query_visitor;
import match_data;

namespace infinity {

QueryBuilder::QueryBuilder() {
    /*
    const Vector<u64> &column_ids = indexer_->GetColumnIDs();
    for (u32 i = 0; i < column_ids.size(); ++i) {
        UniquePtr<ColumnIndexReader> column_index_reader = MakeUnique<ColumnIndexReader>(column_ids[i], indexer_);
        column_index_reader->Open(indexer_->GetIndexConfig());
        index_reader_.column_index_readers_[column_ids[i]] = std::move(column_index_reader);
    }
    index_reader_.session_pool_ = MakeShared<MemoryPool>();
    // TODO get num of docs
    scorer_ = MakeUnique<Scorer>(0);
    */
}

QueryBuilder::~QueryBuilder() {}

UniquePtr<DocIterator> QueryBuilder::CreateSearch(QueryContext &context) {
    QueryVisitor visitor;
    context.query_tree_->Accept(visitor);
    UniquePtr<TermQuery> root = visitor.Build();
    root = TermQuery::Optimize(std::move(root));
    return root->CreateSearch(index_reader_, scorer_.get());
}
} // namespace infinity