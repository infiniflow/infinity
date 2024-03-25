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

#include <utility>
#include <vector>
module query_builder;

import stl;
import memory_pool;
import doc_iterator;
import column_index_reader;
import match_data;
import table_entry;
import table_index_meta;
import table_index_entry;
import create_index_info;
import segment_index_entry;
import memory_indexer;
import index_defines;
import internal_types;
import index_base;
import index_full_text;
import posting_iterator;
import infinity_exception;
import query_node;
import base_table_ref;
import segment_entry;

namespace infinity {

QueryBuilder::QueryBuilder(TransactionID txn_id, TxnTimeStamp begin_ts, SharedPtr<BaseTableRef> &base_table_ref)
    : txn_id_(txn_id), begin_ts_(begin_ts), table_entry_(base_table_ref->table_entry_ptr_) {
    for (auto map_guard = table_entry_->IndexMetaMap(); auto &[index_name, table_index_meta] : *map_guard) {
        auto [table_index_entry, status] = table_index_meta->GetEntryNolock(txn_id_, begin_ts_);
        if (!status.ok()) {
            // Table index entry isn't found
            RecoverableError(status);
        }
        // check index type
        const IndexBase *index_base = table_index_entry->index_base();
        if (auto index_type = index_base->index_type_; index_type != IndexType::kFullText) {
            // non-fulltext index
            continue;
        }
        String column_name = index_base->column_name();
        auto column_id = table_entry_->GetColumnIdByName(column_name);
        u64 ts = table_index_entry->GetFulltexSegmentUpdateTs();
        if (index_reader_.NeedRefreshColumnIndexReader(column_id, ts)) {
            const IndexFullText *index_full_text = reinterpret_cast<const IndexFullText *>(index_base);
            // update column2analyzer_
            column2analyzer_[column_name] = index_full_text->analyzer_;
            // update index_reader_
            UniquePtr<ColumnIndexReader> column_index_reader = MakeUnique<ColumnIndexReader>();
            optionflag_t flag = index_full_text->flag_;
            String index_dir = *(table_index_entry->index_dir());
            Map<SegmentID, SharedPtr<SegmentIndexEntry>> index_by_segment = table_index_entry->GetIndexBySegmentSnapshot();
            column_index_reader->Open(flag, std::move(index_dir), std::move(index_by_segment));
            index_reader_.SetColumnIndexReader(column_id, ts, std::move(column_index_reader));
        }
    }
    index_reader_.session_pool_ = MakeShared<MemoryPool>();
    u64 total_row_count = 0;
    for (SegmentEntry *segment_entry : base_table_ref->block_index_->segments_) {
        total_row_count += segment_entry->row_count();
    }
    scorer_.Init(total_row_count);
}

QueryBuilder::~QueryBuilder() {}

UniquePtr<DocIterator> QueryBuilder::CreateSearch(FullTextQueryContext &context) {
    // Optimize the query tree.
    context.query_tree_ = QueryNode::GetOptimizedQueryTree(std::move(context.query_tree_));
    // Create the iterator from the query tree.
    return context.query_tree_->CreateSearch(table_entry_, index_reader_, &scorer_);
}

} // namespace infinity