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

namespace infinity {

QueryBuilder::QueryBuilder(TableEntry *table_entry) : table_entry_(table_entry) {}

bool QueryBuilder::Init() {
    HashMap<String, UniquePtr<TableIndexMeta>> &index_meta_map = table_entry_->index_meta_map();
    for (auto &meta : index_meta_map) {
        List<SharedPtr<TableIndexEntry>> &index_entry_list = meta.second->index_entry_list();
        for (auto &index_entry : index_entry_list) {
            if (index_entry->index_base()->index_type_ == IndexType::kFullText) {
                u64 column_id = table_entry_->GetColumnIdByName(meta.first);
                UniquePtr<ColumnIndexReader> column_index_reader = MakeUnique<ColumnIndexReader>();
                String index_dir = *(index_entry->index_dir());
                HashMap<SegmentID, SharedPtr<SegmentIndexEntry>> &index_by_segment = index_entry->index_by_segment();
                const SharedPtr<IndexBase> &index_def = index_entry->table_index_def();
                Vector<String> base_names;
                Vector<RowID> base_rowids;
                MemoryIndexer *memory_indexer = nullptr;
                for (auto entry : index_by_segment) {
                    Vector<String> &base_names = entry.second->GetFulltextBaseNames();
                    Vector<u64> &base_row_ids = entry.second->GetFulltextBaseRowIDs();
                    memory_indexer = entry.second->GetMemoryIndexer();
                    for (auto name : base_names)
                        base_names.push_back(name);
                    for (auto base_row_id : base_row_ids) {
                        RowID row_id(base_row_id);
                        base_rowids.push_back(row_id);
                    }
                }
                optionflag_t flag = reinterpret_cast<IndexFullText *>(index_def.get())->flag_;
                column_index_reader->Open(index_dir, base_names, base_rowids, flag, memory_indexer);
                index_reader_.column_index_readers_[column_id] = std::move(column_index_reader);
            }
        }
    }
    index_reader_.session_pool_ = MakeShared<MemoryPool>();
    return false;
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