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
import create_index_data;
import base_table_ref;
import table_collection_entry;
import table_index_entry;
import segment_column_index_entry;
import status;
import infinity_exception;
import buffer_handle;
import index_hnsw;
import index_base;
import hnsw_common;
import dist_func_l2;
import dist_func_ip;
import hnsw_alg;
import lvq_store;
import plain_store;
import buffer_manager;
import txn_store;
import third_party;
import logger;

module physical_create_index_do;

namespace infinity {
PhysicalCreateIndexDo::PhysicalCreateIndexDo(u64 id,
                                             UniquePtr<PhysicalOperator> left,
                                             SharedPtr<BaseTableRef> base_table_ref,
                                             SharedPtr<String> index_name,
                                             SharedPtr<Vector<String>> output_names,
                                             SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                                             SharedPtr<Vector<LoadMeta>> load_metas)
    : PhysicalOperator(PhysicalOperatorType::kCreateIndexDo, Move(left), nullptr, id, load_metas), base_table_ref_(base_table_ref),
      index_name_(index_name), output_names_(output_names), output_types_(output_types) {}

void PhysicalCreateIndexDo::Init() {}

// FIXME: fetch and add a block one time
template <typename Hnsw>
void InsertHnsw(HashMap<u32, atomic_u64> &create_index_idxes,
                const HashMap<u32, SharedPtr<SegmentColumnIndexEntry>> &segment_column_index_entries,
                BufferManager *buffer_mgr) {
    for (auto &[segment_id, create_index_idx] : create_index_idxes) {
        auto iter = segment_column_index_entries.find(segment_id);
        if (iter == segment_column_index_entries.end()) {
            Error<ExecutorException>("Segment id not found in column index entry.");
        }
        auto *segment_column_index_entry = iter->second.get();

        auto buffer_handle = SegmentColumnIndexEntry::GetIndex(segment_column_index_entry, buffer_mgr);
        auto *hnsw_index = static_cast<Hnsw *>(buffer_handle.GetDataMut());

        SizeT vertex_n = hnsw_index->GetVertexNum();
        while (true) {
            SizeT idx = create_index_idx.fetch_add(1);
            if (idx % 10000 == 0) {
                LOG_INFO(Format("Insert index: {}", idx));
            }
            if (idx >= vertex_n) {
                break;
            }
            hnsw_index->Build(idx);
        }
    }
}

bool PhysicalCreateIndexDo::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto *txn = query_context->GetTxn();
    auto *create_index_do_state = static_cast<CreateIndexDoOperatorState *>(operator_state);
    auto &create_index_idxes = create_index_do_state->create_index_shared_data_->create_index_idxes_;

    TableCollectionEntry *table_entry = nullptr;
    Status get_table_entry_status = txn->GetTableEntry(*base_table_ref_->schema_name(), *base_table_ref_->table_name(), table_entry);
    if (!get_table_entry_status.ok()) {
        operator_state->error_message_ = Move(get_table_entry_status.msg_);
        return false;
    }

    TxnTableStore *table_store = txn->GetTxnTableStore(table_entry);
    auto iter = table_store->txn_indexes_store_.find(*index_name_);
    if (iter == table_store->txn_indexes_store_.end()) {
        // the table is empty
        operator_state->SetComplete();
        return true;
    }
    TxnIndexStore &txn_index_store = iter->second;

    auto *table_index_entry = txn_index_store.table_index_entry_;
    if (table_index_entry->index_def_->index_array_.size() != 1) {
        Error<NotImplementException>("Not implemented");
    }
    auto *index_base = table_index_entry->index_def_->index_array_[0].get();
    auto *hnsw_def = static_cast<IndexHnsw *>(index_base);

    if (txn_index_store.index_entry_map_.size() != 1) {
        Error<NotImplementException>("Not implemented");
    }
    const auto &[column_id, segment_column_index_entries] = *txn_index_store.index_entry_map_.begin();

    auto *column_def = table_entry->columns_[column_id].get();
    if (column_def->type()->type() != LogicalType::kEmbedding) {
        Error<ExecutorException>("Create index on non-embedding column is not supported yet.");
    }
    TypeInfo *type_info = column_def->type()->type_info().get();
    auto embedding_info = static_cast<EmbeddingInfo *>(type_info);

    switch (embedding_info->Type()) {
        case kElemFloat: {
            switch (hnsw_def->encode_type_) {
                case HnswEncodeType::kPlain: {
                    switch (hnsw_def->metric_type_) {
                        case MetricType::kMerticInnerProduct: {
                            InsertHnsw<KnnHnsw<float, u64, PlainStore<float>, PlainIPDist<float>>>(create_index_idxes,
                                                                                                   segment_column_index_entries,
                                                                                                   txn->GetBufferMgr());
                            break;
                        }
                        case MetricType::kMerticL2: {
                            InsertHnsw<KnnHnsw<float, u64, PlainStore<float>, PlainL2Dist<float>>>(create_index_idxes,
                                                                                                   segment_column_index_entries,
                                                                                                   txn->GetBufferMgr());
                            break;
                        }
                        default: {
                            Error<StorageException>("Not implemented");
                        }
                    }
                    break;
                }
                case HnswEncodeType::kLVQ: {
                    switch (hnsw_def->metric_type_) {
                        case MetricType::kMerticInnerProduct: {
                            InsertHnsw<KnnHnsw<float, u64, LVQStore<float, i8, LVQIPCache<float, i8>>, LVQIPDist<float, i8>>>(
                                create_index_idxes,
                                segment_column_index_entries,
                                txn->GetBufferMgr());
                            break;
                        }
                        case MetricType::kMerticL2: {
                            InsertHnsw<KnnHnsw<float, u64, LVQStore<float, i8, LVQL2Cache<float, i8>>, LVQL2Dist<float, i8>>>(
                                create_index_idxes,
                                segment_column_index_entries,
                                txn->GetBufferMgr());
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
            Error<NotImplementException>("Create index on non-float embedding column is not supported yet.");
        }
    }
    operator_state->SetComplete();

    return true;
}

}; // namespace infinity