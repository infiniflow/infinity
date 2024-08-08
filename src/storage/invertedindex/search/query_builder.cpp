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

#include <iostream>
#include <vector>
module query_builder;

import stl;

import doc_iterator;
import column_index_reader;
import table_entry;
import segment_index_entry;
import internal_types;
import index_base;
import infinity_exception;
import query_node;
import base_table_ref;
import segment_entry;
import term_doc_iterator;
import logger;
import third_party;

namespace infinity {

void QueryBuilder::Init(IndexReader index_reader) { index_reader_ = index_reader; }

QueryBuilder::~QueryBuilder() {}

UniquePtr<DocIterator> QueryBuilder::CreateSearch(FullTextQueryContext &context, EarlyTermAlgo early_term_algo) {
    // Optimize the query tree.
    if (!context.optimized_query_tree_) {
        context.optimized_query_tree_ = QueryNode::GetOptimizedQueryTree(std::move(context.query_tree_));
    }
    // Create the iterator from the query tree.
    UniquePtr<DocIterator> result = context.optimized_query_tree_->CreateSearch(table_entry_, index_reader_, early_term_algo);
#ifdef INFINITY_DEBUG
    {
        OStringStream oss;
        oss << "DocIterator:\n";
        if (result) {
            result->PrintTree(oss);
        } else {
            oss << "Empty tree!\n";
        }
        LOG_DEBUG(std::move(oss).str());
    }
#endif
    return result;
}

} // namespace infinity