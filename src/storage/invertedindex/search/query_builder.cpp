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
import match_data;
import table_entry;
import segment_index_entry;
import internal_types;
import index_base;
import infinity_exception;
import query_node;
import base_table_ref;
import segment_entry;
import blockmax_term_doc_iterator;
import logger;
import third_party;

namespace infinity {

void QueryBuilder::Init(IndexReader index_reader) {
    index_reader_ = index_reader;

    u64 total_row_count = 0;
    for (const auto &[segment_id, segment_info] : base_table_ref_->block_index_->segment_block_index_) {
        total_row_count += segment_info.segment_offset_;
    }

    scorer_.Init(total_row_count, &index_reader_);
}

QueryBuilder::~QueryBuilder() {}

UniquePtr<DocIterator> QueryBuilder::CreateSearch(FullTextQueryContext &context) {
    // Optimize the query tree.
    if (!context.optimized_query_tree_) {
        context.optimized_query_tree_ = QueryNode::GetOptimizedQueryTree(std::move(context.query_tree_));
    }
    // Create the iterator from the query tree.
    UniquePtr<DocIterator> result = context.optimized_query_tree_->CreateSearch(base_table_ref_->table_entry_ptr_, index_reader_, &scorer_);
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

UniquePtr<EarlyTerminateIterator> QueryBuilder::CreateEarlyTerminateSearch(FullTextQueryContext &context, EarlyTermAlg early_term_alg) {
    // Optimize the query tree.
    if (!context.optimized_query_tree_) {
        context.optimized_query_tree_ = QueryNode::GetOptimizedQueryTree(std::move(context.query_tree_));
    }
    // Create the iterator from the query tree.
    UniquePtr<EarlyTerminateIterator> result =
        context.optimized_query_tree_->CreateEarlyTerminateSearch(base_table_ref_->table_entry_ptr_, index_reader_, &scorer_, early_term_alg);
#ifdef INFINITY_DEBUG
    {
        OStringStream oss;
        oss << "EarlyTerminateIterator:\n";
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