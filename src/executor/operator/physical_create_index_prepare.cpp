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

import stl;
import parser;
import physical_operator_type;
import physical_operator;
import query_context;
import operator_state;
import load_meta;

import index_def;
import table_collection_entry;
import status;
import infinity_exception;
import base_entry;
import segment_entry;
import table_index_entry;
import column_index_entry;
import segment_column_index_entry;
import index_base;
import index_file_worker;
import segment_iter;
import buffer_manager;
import buffer_handle;
import index_hnsw;
import default_values;
import txn_store;

import hnsw_common;
import dist_func_l2;
import dist_func_ip;
import hnsw_alg;
import lvq_store;
import plain_store;

module physical_create_index_prepare;

namespace infinity {
PhysicalCreateIndexPrepare::PhysicalCreateIndexPrepare(u64 id,
                                                       SharedPtr<String> schema_name,
                                                       SharedPtr<String> table_name,
                                                       SharedPtr<IndexDef> index_definition,
                                                       ConflictType conflict_type,
                                                       SharedPtr<Vector<String>> output_names,
                                                       SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                                                       SharedPtr<Vector<LoadMeta>> load_metas)
    : PhysicalOperator(PhysicalOperatorType::kCreateIndexPrepare, nullptr, nullptr, id, load_metas), schema_name_(schema_name),
      table_name_(table_name), index_def_ptr_(index_definition), conflict_type_(conflict_type), output_names_(output_names),
      output_types_(output_types) {}

void PhysicalCreateIndexPrepare::Init() {}

template <typename Hnsw>
void InsertHnswPrepare(BufferHandle buffer_handle, const SegmentEntry *segment_entry, u32 column_id) {
    auto hnsw_index = static_cast<Hnsw *>(buffer_handle.GetDataMut());

    u32 segment_offset = 0;
    Vector<u64> row_ids;
    const auto &block_entries = segment_entry->block_entries_;
    for (SizeT i = 0; i < block_entries.size(); ++i) {
        const auto &block_entry = block_entries[i];
        SizeT block_row_cnt = block_entry->row_count_;

        for (SizeT block_offset = 0; block_offset < block_row_cnt; ++block_offset) {
            RowID row_id(segment_entry->segment_id_, segment_offset + block_offset);
            row_ids.push_back(row_id.ToUint64());
        }
        segment_offset += DEFAULT_BLOCK_CAPACITY;
    }
    OneColumnIterator<float> one_column_iter(segment_entry, column_id);

    hnsw_index->StoreData(one_column_iter, row_ids.data(), row_ids.size());
}

bool PhysicalCreateIndexPrepare::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto *txn = query_context->GetTxn();
    TxnTimeStamp begin_ts = txn->BeginTS();
    BufferManager *buffer_mgr = txn->GetBufferMgr();

    TableCollectionEntry *table_entry = nullptr;
    Status get_table_status = txn->GetTableEntry(*schema_name_, *table_name_, table_entry);
    if (!get_table_status.ok()) {
        operator_state->error_message_ = Move(get_table_status.msg_);
        return false;
    }

    TableIndexEntry *table_index_entry = nullptr;
    Status create_index_status = txn->CreateIndex(table_entry, index_def_ptr_, conflict_type_, table_index_entry);
    if (!create_index_status.ok()) {
        operator_state->error_message_ = Move(create_index_status.msg_);
        return false;
    }

    if (table_index_entry->irs_index_entry_.get() != nullptr) {
        Error<NotImplementException>("TableCollectionEntry::CreateIndexFilePrepare");
    }
    if (table_index_entry->column_index_map_.size() != 1) {
        Error<NotImplementException>("TableCollectionEntry::CreateIndexFilePrepare");
    }
    auto [column_id, base_entry] = *table_index_entry->column_index_map_.begin();
    SharedPtr<ColumnDef> column_def = table_entry->columns_[column_id];
    auto *column_index_entry = static_cast<ColumnIndexEntry *>(base_entry.get());

    for (const auto &[segment_id, segment_entry] : table_entry->segment_map_) {
        u64 column_id = column_def->id();
        IndexBase *index_base = column_index_entry->index_base_.get();
        UniquePtr<CreateIndexParam> create_index_param = SegmentEntry::GetCreateIndexParam(segment_entry.get(), index_base, column_def.get());
        SharedPtr<SegmentColumnIndexEntry> segment_column_index_entry =
            SegmentColumnIndexEntry::NewIndexEntry(column_index_entry, segment_entry->segment_id_, begin_ts, buffer_mgr, create_index_param.get());

        if (index_base->index_type_ != IndexType::kHnsw) {
            Error<StorageException>("Only HNSW index is supported.");
        }
        auto *index_hnsw = static_cast<IndexHnsw *>(index_base);
        if (column_def->type()->type() != LogicalType::kEmbedding) {
            Error<StorageException>("HNSW supports embedding type.");
        }
        TypeInfo *type_info = column_def->type()->type_info().get();
        auto embedding_info = static_cast<EmbeddingInfo *>(type_info);

        BufferHandle buffer_handle = SegmentColumnIndexEntry::GetIndex(segment_column_index_entry.get(), buffer_mgr);
        switch (embedding_info->Type()) {
            case kElemFloat: {
                switch (index_hnsw->encode_type_) {
                    case HnswEncodeType::kPlain: {
                        switch (index_hnsw->metric_type_) {
                            case MetricType::kMerticInnerProduct: {
                                InsertHnswPrepare<KnnHnsw<float, u64, PlainStore<float>, PlainIPDist<float>>>(buffer_handle,
                                                                                                              segment_entry.get(),
                                                                                                              column_id);
                                break;
                            }
                            case MetricType::kMerticL2: {
                                InsertHnswPrepare<KnnHnsw<float, u64, PlainStore<float>, PlainL2Dist<float>>>(buffer_handle,
                                                                                                              segment_entry.get(),
                                                                                                              column_id);
                                break;
                            }
                            default: {
                                Error<StorageException>("Not implemented");
                            }
                        }
                        break;
                    }
                    case HnswEncodeType::kLVQ: {
                        switch (index_hnsw->metric_type_) {
                            case MetricType::kMerticInnerProduct: {
                                InsertHnswPrepare<KnnHnsw<float, u64, LVQStore<float, i8, LVQIPCache<float, i8>>, LVQIPDist<float, i8>>>(
                                    buffer_handle,
                                    segment_entry.get(),
                                    column_id);
                                break;
                            }
                            case MetricType::kMerticL2: {
                                InsertHnswPrepare<KnnHnsw<float, u64, LVQStore<float, i8, LVQL2Cache<float, i8>>, LVQL2Dist<float, i8>>>(
                                    buffer_handle,
                                    segment_entry.get(),
                                    column_id);
                                break;
                            }
                            default: {
                                Error<StorageException>("Not implemented");
                            }
                        }
                        break;
                    }
                    default: {
                        Error<StorageException>("Not implemented");
                    }
                }
                break;
            }
            default: {
                Error<StorageException>("Not implemented");
            }
        }
        TxnTableStore *table_store = txn->GetTxnTableStore(table_entry);
        table_store->CreateIndexFile(table_index_entry, column_id, segment_id, segment_column_index_entry);

        column_index_entry->index_by_segment.emplace(segment_id, segment_column_index_entry);
    }

    operator_state->SetComplete();
    return true;
}
} // namespace infinity