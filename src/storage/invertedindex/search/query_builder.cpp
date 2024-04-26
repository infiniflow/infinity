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
import memory_pool;
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

QueryBuilder::QueryBuilder(TransactionID txn_id, TxnTimeStamp begin_ts, SharedPtr<BaseTableRef> &base_table_ref)
    : txn_id_(txn_id), begin_ts_(begin_ts), table_entry_(base_table_ref->table_entry_ptr_),
      index_reader_(table_entry_->GetFullTextIndexReader(txn_id_, begin_ts_)) {
    u64 total_row_count = 0;
    for (SegmentEntry *segment_entry : base_table_ref->block_index_->segments_) {
        total_row_count += segment_entry->row_count();
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
    UniquePtr<DocIterator> result = context.optimized_query_tree_->CreateSearch(table_entry_, index_reader_, &scorer_);
#ifdef INFINITY_DEBUG
    {
        OStringStream oss;
        oss << "DocIterator:\n";
        if (result) {
            result->PrintTree(oss);
        } else {
            oss << "Empty tree!\n";
        }
        LOG_INFO(std::move(oss).str());
    }
#endif
    return result;
}

UniquePtr<EarlyTerminateIterator> QueryBuilder::CreateEarlyTerminateSearch(FullTextQueryContext &context) {
    // Optimize the query tree.
    if (!context.optimized_query_tree_) {
        context.optimized_query_tree_ = QueryNode::GetOptimizedQueryTree(std::move(context.query_tree_));
    }
    // Create the iterator from the query tree.
    UniquePtr<EarlyTerminateIterator> result = context.optimized_query_tree_->CreateEarlyTerminateSearch(table_entry_, index_reader_, &scorer_);
#ifdef INFINITY_DEBUG
    {
        OStringStream oss;
        oss << "EarlyTerminateIterator:\n";
        if (result) {
            result->PrintTree(oss);
        } else {
            oss << "Empty tree!\n";
        }
        LOG_INFO(std::move(oss).str());
    }
#endif
    return result;
}

} // namespace infinity