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

#include <string>
import stl;
import query_context;
import parser;
import logger;
import operator_state;
import global_block_id;
import block_index;
import base_table_ref;
import block_entry;
import knn_scan_data;
import column_buffer;
import block_column_entry;
import knn_distance;
import third_party;

import infinity_exception;
import table_collection_entry;
import default_values;

import segment_entry;
import block_column_entry;
import base_entry;
import knn_expression;
import column_expression;

import index_base;
import buffer_manager;
import merge_knn;
import faiss;
import index_def;
import ann_ivf_flat;
import annivfflat_index_data;
import buffer_handle;
import table_index_meta;
import segment_column_index_entry;
import column_index_entry;
import table_index_entry;
import data_block;
import index_hnsw;

import hnsw_alg;
import plain_store;
import lvq_store;
import dist_func_l2;
import dist_func_ip;

module physical_knn_scan;

namespace infinity {

void PhysicalKnnScan::Init() {}

bool PhysicalKnnScan::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto *knn_scan_operator_state = static_cast<KnnScanOperatorState *>(operator_state);
    auto elem_type = knn_scan_operator_state->knn_scan_function_data1_->shared_data_->elem_type_;
    auto dist_type = knn_scan_operator_state->knn_scan_function_data1_->shared_data_->knn_distance_type_;
    switch (elem_type) {
        case kElemFloat: {
            switch (dist_type) {
                case KnnDistanceType::kL2:
                case KnnDistanceType::kHamming: {
                    ExecuteInternal<f32, CompareMax>(query_context, knn_scan_operator_state);
                    break;
                }
                case KnnDistanceType::kCosine:
                case KnnDistanceType::kInnerProduct: {
                    ExecuteInternal<f32, CompareMin>(query_context, knn_scan_operator_state);
                    break;
                }
                default: {
                    Error<ExecutorException>("Not implemented");
                }
            }
            break;
        }
        default: {
            Error<ExecutorException>("Not implemented");
        }
    }
    return true;
}

TableCollectionEntry *PhysicalKnnScan::table_collection_ptr() const { return base_table_ref_->table_entry_ptr_; }

String PhysicalKnnScan::TableAlias() const { return base_table_ref_->alias_; }

BlockIndex *PhysicalKnnScan::GetBlockIndex() const { return base_table_ref_->block_index_.get(); }

Vector<SizeT> &PhysicalKnnScan::ColumnIDs() const { return base_table_ref_->column_ids_; }

Pair<Vector<BlockColumnEntry *>, Vector<SegmentColumnIndexEntry *>>
PhysicalKnnScan::PlanWithIndex(QueryContext *query_context) { // TODO: return base entry vector
    u64 txn_id = query_context->GetTxn()->TxnID();
    TxnTimeStamp begin_ts = query_context->GetTxn()->BeginTS();

    if (knn_expressions_.size() != 1) {
        Error<SchedulerException>("Currently, we only support one knn column scenario");
    }
    KnnExpression *knn_expr = static_cast<KnnExpression *>(knn_expressions_[0].get());
    ColumnExpression *column_expr = static_cast<ColumnExpression *>(knn_expr->arguments()[0].get());
    SizeT knn_column_id = column_expr->binding().column_idx;

    Vector<BlockColumnEntry *> block_column_entries;
    Vector<SegmentColumnIndexEntry *> index_entries;

    TableCollectionEntry *table_entry = base_table_ref_->table_entry_ptr_;
    HashMap<u32, Vector<SegmentColumnIndexEntry *>> index_entry_map;
    for (auto &[index_name, table_index_meta] : table_entry->index_meta_map_) {
        BaseEntry *base_entry{nullptr};
        auto status = TableIndexMeta::GetEntry(table_index_meta.get(), txn_id, begin_ts, base_entry);
        if (!status.ok()) {
            // FIXME: not found index means exception??
            Error<StorageException>("Cannot find index entry.");
        }

        TableIndexEntry *table_index_entry = (TableIndexEntry *)base_entry;
        auto column_index_iter = table_index_entry->column_index_map_.find(knn_column_id);
        if (column_index_iter == table_index_entry->column_index_map_.end()) {
            // knn_column_id isn't in this table index
            continue;
        }

        // Fill the segment with index
        ColumnIndexEntry *column_index_entry = table_index_entry->column_index_map_[knn_column_id].get();
        index_entry_map.reserve(column_index_entry->index_by_segment.size());
        for (auto &[segment_id, segment_column_index] : column_index_entry->index_by_segment) {
            index_entry_map[segment_id].emplace_back(segment_column_index.get());
        }
    }

    // Generate task set: index segment and no index block
    BlockIndex *block_index = base_table_ref_->block_index_.get();
    for (SegmentEntry *segment_entry : block_index->segments_) {
        if (auto iter = index_entry_map.find(segment_entry->segment_id_); iter != index_entry_map.end()) {
            index_entries.emplace_back(iter->second[0]);
        } else {
            for (auto &block_entry : segment_entry->block_entries_) {
                BlockColumnEntry *block_column_entry = block_entry->columns_[knn_column_id].get();
                block_column_entries.emplace_back(block_column_entry);
            }
        }
    }
    LOG_TRACE(Format("KnnScan: brute force task: {}, index task: {}", block_column_entries.size(), index_entries.size()));
    return {block_column_entries, index_entries};
}

SizeT PhysicalKnnScan::BlockEntryCount() const { return base_table_ref_->block_index_->BlockCount(); }

template <typename DataType, template <typename, typename> typename C>
void PhysicalKnnScan::ExecuteInternal(QueryContext *query_context, KnnScanOperatorState *operator_state) {
    auto knn_scan_function_data = operator_state->knn_scan_function_data1_.get();
    auto knn_scan_shared_data = knn_scan_function_data->shared_data_.get();

    auto dist_func = static_cast<KnnDistance1<DataType> *>(knn_scan_function_data->knn_distance_.get());
    auto merge_heap = static_cast<MergeKnn<DataType, C> *>(knn_scan_function_data->merge_knn_base_.get());
    auto query = static_cast<const DataType *>(knn_scan_shared_data->query_embedding_);

    SizeT index_task_n = knn_scan_shared_data->index_entries_.size();
    SizeT brute_task_n = knn_scan_shared_data->block_column_entries_.size();

    if (u64 block_column_idx = knn_scan_shared_data->current_block_idx_++; block_column_idx < brute_task_n) {
        LOG_TRACE(Format("KnnScan: {} brute force {}/{}", knn_scan_function_data->task_id_, block_column_idx + 1, brute_task_n));
        // brute force
        BlockColumnEntry *block_column_entry = knn_scan_shared_data->block_column_entries_[block_column_idx];
        const BlockEntry *block_entry = block_column_entry->block_entry_;

        BufferManager *buffer_mgr = query_context->storage()->buffer_manager();
        ColumnBuffer column_buffer = BlockColumnEntry::GetColumnData(block_column_entry, buffer_mgr);

        auto data = reinterpret_cast<const DataType *>(column_buffer.GetAll());
        u16 row_count = block_entry->row_count_;

        Vector<DataType> dists = dist_func->Calculate(data, row_count, query, knn_scan_shared_data->dimension_);
        Vector<RowID> row_ids(row_count);
        for (u16 i = 0; i < row_count; ++i) {
            row_ids[i] = RowID(block_entry->segment_entry_->segment_id_, block_entry->block_id_ * DEFAULT_BLOCK_CAPACITY + i);
        }
        merge_heap->Search(dists.data(), row_ids.data(), row_count);
    } else if (u64 index_idx = knn_scan_shared_data->current_index_idx_++; index_idx < index_task_n) {
        LOG_TRACE(Format("KnnScan: {} index {}/{}", knn_scan_function_data->task_id_, index_idx + 1, index_task_n));
        // with index
        SegmentColumnIndexEntry *segment_column_index_entry = knn_scan_shared_data->index_entries_[index_idx];
        BufferManager *buffer_mgr = query_context->storage()->buffer_manager();

        switch (segment_column_index_entry->column_index_entry_->index_base_->index_type_) {
            case IndexType::kIVFFlat: {
                BufferHandle index_handle = SegmentColumnIndexEntry::GetIndex(segment_column_index_entry, buffer_mgr);
                auto index = static_cast<const AnnIVFFlatIndexData<DataType> *>(index_handle.GetData());
                i32 n_probes = 1;
                auto segment_id = segment_column_index_entry->segment_id_;
                switch (knn_scan_shared_data->knn_distance_type_) {
                    case KnnDistanceType::kL2: {
                        AnnIVFFlatL2 ann_ivfflat_query{query,
                                                       knn_scan_shared_data->query_count_,
                                                       knn_scan_shared_data->topk_,
                                                       knn_scan_shared_data->dimension_,
                                                       knn_scan_shared_data->elem_type_};
                        ann_ivfflat_query.Begin();
                        ann_ivfflat_query.Search(index, segment_id, n_probes);
                        ann_ivfflat_query.End();
                        auto dists = ann_ivfflat_query.GetDistances();
                        auto row_ids = ann_ivfflat_query.GetIDs();
                        merge_heap->Search(dists, row_ids, knn_scan_shared_data->topk_);
                        break;
                    }
                    case KnnDistanceType::kInnerProduct: {
                        AnnIVFFlatIP ann_ivfflat_query{query,
                                                       knn_scan_shared_data->query_count_,
                                                       knn_scan_shared_data->topk_,
                                                       knn_scan_shared_data->dimension_,
                                                       knn_scan_shared_data->elem_type_};
                        ann_ivfflat_query.Begin();
                        ann_ivfflat_query.Search(index, segment_id, n_probes);
                        ann_ivfflat_query.End();
                        auto dists = ann_ivfflat_query.GetDistances();
                        auto row_ids = ann_ivfflat_query.GetIDs();
                        merge_heap->Search(dists, row_ids, knn_scan_shared_data->topk_);
                        break;
                    }
                    default: {
                        Error<ExecutorException>("Not implemented");
                    }
                }
                break;
            }
            case IndexType::kHnsw: {
                BufferHandle index_handle = SegmentColumnIndexEntry::GetIndex(segment_column_index_entry, buffer_mgr);
                auto index_hnsw = static_cast<IndexHnsw *>(segment_column_index_entry->column_index_entry_->index_base_.get());
                auto KnnScan = [&](auto *index) {
                    Vector<DataType> dists(knn_scan_shared_data->topk_ * knn_scan_shared_data->query_count_);
                    Vector<RowID> row_ids(knn_scan_shared_data->topk_ * knn_scan_shared_data->query_count_);

                    i64 result_n = -1;
                    for (u64 query_idx = 0; query_idx < knn_scan_shared_data->query_count_; ++query_idx) {
                        const DataType *query =
                            static_cast<const DataType *>(knn_scan_shared_data->query_embedding_) + query_idx * knn_scan_shared_data->dimension_;
                        MaxHeap<Pair<DataType, u64>> heap = index->KnnSearch(query, knn_scan_shared_data->topk_);
                        if (result_n < 0) {
                            result_n = heap.size();
                        } else if (result_n != heap.size()) {
                            throw ExecutorException("Bug");
                        }
                        u64 id = 0;
                        while (!heap.empty()) {
                            const auto &[dist, row_id] = heap.top();
                            row_ids[query_idx * knn_scan_shared_data->topk_ + id] = RowID::FromUint64(row_id);
                            switch (knn_scan_shared_data->knn_distance_type_) {
                                case KnnDistanceType::kInvalid: {
                                    throw ExecutorException("Bug");
                                }
                                case KnnDistanceType::kL2:
                                case KnnDistanceType::kHamming: {
                                    dists[query_idx * knn_scan_shared_data->topk_ + id] = +dist;
                                    break;
                                }
                                case KnnDistanceType::kCosine:
                                case KnnDistanceType::kInnerProduct: {
                                    dists[query_idx * knn_scan_shared_data->topk_ + id] = -dist;
                                    break;
                                }
                            }
                            ++id;
                            heap.pop();
                        }
                    }
                    merge_heap->Search(dists.data(), row_ids.data(), result_n);
                };
                switch (index_hnsw->encode_type_) {
                    case HnswEncodeType::kPlain: {
                        switch (index_hnsw->metric_type_) {
                            case MetricType::kMerticInnerProduct: {
                                using Hnsw = KnnHnsw<f32, u64, PlainStore<f32>, PlainIPDist<f32>>;
                                KnnScan(static_cast<const Hnsw *>(index_handle.GetData()));
                                break;
                            }
                            case MetricType::kMerticL2: {
                                using Hnsw = KnnHnsw<f32, u64, PlainStore<f32>, PlainL2Dist<f32>>;
                                KnnScan(static_cast<const Hnsw *>(index_handle.GetData()));
                                break;
                            }
                            default: {
                                Error<ExecutorException>("Not implemented");
                            }
                        }
                        break;
                    }
                    case HnswEncodeType::kLVQ: {
                        switch (index_hnsw->metric_type_) {
                            case MetricType::kMerticInnerProduct: {
                                using Hnsw = KnnHnsw<f32, u64, LVQStore<f32, i8, LVQIPCache<f32, i8>>, LVQIPDist<f32, i8>>;
                                KnnScan(static_cast<const Hnsw *>(index_handle.GetData()));
                                break;
                            }
                            case MetricType::kMerticL2: {
                                using Hnsw = KnnHnsw<f32, u64, LVQStore<f32, i8, LVQL2Cache<f32, i8>>, LVQL2Dist<f32, i8>>;
                                KnnScan(static_cast<const Hnsw *>(index_handle.GetData()));
                                break;
                            }
                            default: {
                                Error<ExecutorException>("Not implemented");
                            }
                        }
                        break;
                    }
                    default: {
                        Error<ExecutorException>("Not implemented");
                    }
                }
                break;
            }
            default: {
                Error<ExecutorException>("Not implemented");
            }
        }
    }
    if (knn_scan_shared_data->current_index_idx_ >= index_task_n && knn_scan_shared_data->current_block_idx_ >= brute_task_n) {
        LOG_TRACE(Format("KnnScan: {} task finished", knn_scan_function_data->task_id_));
        // all task Complete
        SizeT column_n = knn_scan_shared_data->table_ref_->column_ids_.size();
        BlockIndex *block_index = knn_scan_shared_data->table_ref_->block_index_.get();

        i64 result_n = Min(knn_scan_shared_data->topk_, merge_heap->total_count());

        UniquePtr<DataBlock> data_block = DataBlock::MakeUniquePtr();
        data_block->Init(*GetOutputTypes());
        operator_state->data_block_array_.emplace_back(Move(data_block));
        SizeT row_idx = DEFAULT_BLOCK_CAPACITY;

        SizeT total_data_row_count = knn_scan_shared_data->query_count_ * result_n;
        for (; row_idx < total_data_row_count; row_idx += DEFAULT_BLOCK_CAPACITY) {
            data_block = DataBlock::MakeUniquePtr();
            data_block->Init(*GetOutputTypes());
            operator_state->data_block_array_.emplace_back(Move(data_block));
        }

        SizeT output_block_row_id = 0;
        SizeT output_block_idx = 0;
        DataBlock* output_block_ptr = operator_state->data_block_array_.back().get();
        for (u64 query_idx = 0; query_idx < knn_scan_shared_data->query_count_; ++query_idx) {
            DataType *result_dists = merge_heap->GetDistancesByIdx(query_idx);
            RowID *row_ids = merge_heap->GetIDsByIdx(query_idx);

            for (i64 top_idx = result_n - 1; top_idx >= 0; --top_idx) {
                SizeT id = query_idx * knn_scan_shared_data->query_count_ + top_idx;

                u32 segment_id = row_ids[top_idx].segment_id_;
                u32 segment_offset = row_ids[top_idx].segment_offset_;
                u16 block_id = segment_offset / DEFAULT_BLOCK_CAPACITY;
                u16 block_offset = segment_offset % DEFAULT_BLOCK_CAPACITY;

                BlockEntry *block_entry = block_index->GetBlockEntry(segment_id, block_id);
                if (block_entry == nullptr) {
                    throw ExecutorException(Format("Cannot find block segment id: {}, block id: {}", segment_id, block_id));
                }

                if (output_block_row_id == DEFAULT_BLOCK_CAPACITY) {
                    output_block_ptr->Finalize();
                    output_block_row_id -= 0;
                    ++output_block_idx;
                    output_block_ptr = operator_state->data_block_array_[output_block_idx].get();
                }

                for (SizeT column_id : base_table_ref_->column_ids_) {
                    ColumnBuffer column_buffer =
                        BlockColumnEntry::GetColumnData(block_entry->columns_[column_id].get(), query_context->storage()->buffer_manager());

                    const_ptr_t ptr = column_buffer.GetValueAt(block_offset, *output_types_->at(column_id));
                    output_block_ptr->AppendValueByPtr(column_id, ptr);
                }
                SizeT last = base_table_ref_->column_ids_.size();
                output_block_ptr->AppendValueByPtr(last++, (ptr_t)&result_dists[id]);
                output_block_ptr->AppendValueByPtr(last, (ptr_t)&row_ids[id]);

                ++ output_block_row_id;
            }
        }
        output_block_ptr->Finalize();
        operator_state->SetComplete();
    }
}

} // namespace infinity
