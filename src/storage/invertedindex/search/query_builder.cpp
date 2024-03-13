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
import term_doc_iterator;
import and_iterator;
import and_not_iterator;
import or_iterator;
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

namespace infinity {

QueryBuilder::QueryBuilder(TableEntry *table_entry) : table_entry_(table_entry) {
    HashMap<String, UniquePtr<TableIndexMeta>> &index_meta_map = table_entry_->index_meta_map();
    for (auto &meta : index_meta_map) {
        List<SharedPtr<TableIndexEntry>> &index_entry_list = meta.second->index_entry_list();
        for (auto &index_entry : index_entry_list) {
            if (index_entry->index_base()->index_type_ == IndexType::kFullText) {
                u64 column_id = table_entry_->GetColumnIdByName(meta.first);
                u64 ts = index_entry->GetFulltexSegmentUpdateTs();
                if (index_reader_.NeedRefreshColumnIndexReader(column_id, ts)) {
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
                    index_reader_.SetColumnIndexReader(column_id, ts, std::move(column_index_reader));
                }
            }
        }
    }
    index_reader_.session_pool_ = MakeShared<MemoryPool>();
}

QueryBuilder::~QueryBuilder() {}

UniquePtr<DocIterator> QueryBuilder::CreateSearch(QueryContext &context) {
    // Optimize the query tree.
    UniquePtr<QueryNode> root = QueryNode::GetOptimizedQueryTree(std::move(context.query_tree_));
    // put it back
    context.query_tree_ = std::move(root);
    // Create the iterator from the query tree.
    return CreateSearchForQueryNode(*context.query_tree_);
}

UniquePtr<DocIterator> QueryBuilder::CreateSearchForQueryNode(const QueryNode &query_node) {
    switch (query_node.GetType()) {
        case QueryNodeType::TERM: {
            const auto &term_node = static_cast<const TermQueryNode &>(query_node);
            ColumnID column_id = table_entry_->GetColumnIdByName(term_node.column_);
            ColumnIndexReader *column_index_reader = index_reader_.GetColumnIndexReader(column_id);
            if (!column_index_reader)
                return nullptr;
            PostingIterator *posting_iterator = column_index_reader->Lookup(term_node.term_, index_reader_.session_pool_.get());
            if (posting_iterator == nullptr)
                return nullptr;
            auto search = MakeUnique<TermDocIterator>(posting_iterator, column_id);
            scorer_->AddDocIterator(search.get(), column_id);
            return std::move(search);
        }
        case QueryNodeType::AND: {
            const auto &and_node = static_cast<const AndQueryNode &>(query_node);
            Vector<UniquePtr<DocIterator>> sub_doc_iters;
            sub_doc_iters.reserve(and_node.children_.size());
            for (auto &child : and_node.children_) {
                auto iter = CreateSearchForQueryNode(*child);
                if (iter) {
                    sub_doc_iters.emplace_back(std::move(iter));
                }
            }
            if (sub_doc_iters.empty()) {
                return nullptr;
            } else if (sub_doc_iters.size() == 1) {
                return std::move(sub_doc_iters[0]);
            } else {
                return MakeUnique<AndIterator>(std::move(sub_doc_iters));
            }
        }
        case QueryNodeType::AND_NOT: {
            const auto &and_not_node = static_cast<const AndNotQueryNode &>(query_node);
            Vector<UniquePtr<DocIterator>> sub_doc_iters;
            sub_doc_iters.reserve(and_not_node.children_.size());
            // check if the first child is a valid query
            auto first_iter = CreateSearchForQueryNode(*and_not_node.children_[0]);
            if (!first_iter) {
                // no need to continue if the first child is invalid
                return nullptr;
            }
            sub_doc_iters.emplace_back(std::move(first_iter));
            for (u32 i = 1; i < and_not_node.children_.size(); ++i) {
                auto iter = CreateSearchForQueryNode(*and_not_node.children_[i]);
                if (iter) {
                    sub_doc_iters.emplace_back(std::move(iter));
                }
            }
            if (sub_doc_iters.size() == 1) {
                return std::move(sub_doc_iters[0]);
            } else {
                return MakeUnique<AndNotIterator>(std::move(sub_doc_iters));
            }
        }
        case QueryNodeType::OR: {
            const auto &or_node = static_cast<const OrQueryNode &>(query_node);
            Vector<UniquePtr<DocIterator>> sub_doc_iters;
            sub_doc_iters.reserve(or_node.children_.size());
            for (auto &child : or_node.children_) {
                auto iter = CreateSearchForQueryNode(*child);
                if (iter) {
                    sub_doc_iters.emplace_back(std::move(iter));
                }
            }
            if (sub_doc_iters.empty()) {
                return nullptr;
            } else if (sub_doc_iters.size() == 1) {
                return std::move(sub_doc_iters[0]);
            } else {
                return MakeUnique<OrIterator>(std::move(sub_doc_iters));
            }
        }
        case QueryNodeType::WAND:
        case QueryNodeType::PHRASE:
        case QueryNodeType::PREFIX_TERM:
        case QueryNodeType::SUFFIX_TERM:
        case QueryNodeType::SUBSTRING_TERM: {
            UnrecoverableError("Unsupported query node type");
            return nullptr;
        }
        case QueryNodeType::NOT: {
            UnrecoverableError("NOT query node should be optimized into AND_NOT query node");
            return nullptr;
        }
    }
}

} // namespace infinity