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

#include "type/complex/row_id.h"
#include <string>

module physical_knn_scan;

import stl;
import query_context;
import operator_state;
import physical_operator_type;
import base_table_ref;
import table_entry;
import block_column_entry;
import segment_index_entry;
import load_meta;
import knn_expression;
import data_type;
import common_query_filter;
import physical_filter_scan_base;

import logger;
import block_index;
import knn_scan_data;
import knn_filter;
import knn_distance;
import third_party;
import txn;
import infinity_exception;
import default_values;
import column_expression;

import buffer_manager;
import merge_knn;
import knn_result_handler;
import ann_ivf_flat;
import annivfflat_index_data;
import buffer_handle;
import data_block;
import bitmask;
import column_vector;
import index_hnsw;
import status;
import create_index_info;
import knn_expr;
import block_entry;
import segment_entry;
import abstract_hnsw;

namespace infinity {

void ReadDataBlock(DataBlock *output,
                   BufferManager *buffer_mgr,
                   const auto row_count,
                   const BlockEntry *current_block_entry,
                   const Vector<SizeT> &column_ids) {
    auto block_id = current_block_entry->block_id();
    auto segment_id = current_block_entry->segment_id();
    for (SizeT output_column_id = 0; auto column_id : column_ids) {
        if (column_id == COLUMN_IDENTIFIER_ROW_ID) {
            u32 segment_offset = block_id * DEFAULT_BLOCK_CAPACITY;
            output->column_vectors[output_column_id++]->AppendWith(RowID(segment_id, segment_offset), row_count);
        } else {
            ColumnVector column_vector = current_block_entry->GetColumnBlockEntry(column_id)->GetColumnVector(buffer_mgr);
            output->column_vectors[output_column_id++]->AppendWith(column_vector, 0, row_count);
        }
    }
    output->Finalize();
}

// void MergeIntoBitmask(const VectorBuffer *input_bool_column_buffer,
//                       const SharedPtr<Bitmask> &input_null_mask,
//                       const SizeT count,
//                       Bitmask &bitmask,
//                       bool nullable,
//                       SizeT bitmask_offset = 0) {
//     if ((!nullable) || (input_null_mask->IsAllTrue())) {
//         for (SizeT idx = 0; idx < count; ++idx) {
//             if (!(input_bool_column_buffer->GetCompactBit(idx))) {
//                 bitmask.SetFalse(idx + bitmask_offset);
//             }
//         }
//     } else {
//         const u64 *result_null_data = input_null_mask->GetData();
//         u64 *bitmask_data = bitmask.GetData();
//         SizeT unit_count = BitmaskBuffer::UnitCount(count);
//         bool bitmask_use_unit = (bitmask_offset % BitmaskBuffer::UNIT_BITS) == 0;
//         SizeT bitmask_unit_offset = bitmask_offset / BitmaskBuffer::UNIT_BITS;
//         for (SizeT i = 0, start_index = 0, end_index = BitmaskBuffer::UNIT_BITS; i < unit_count;
//              ++i, end_index = std::min(end_index + BitmaskBuffer::UNIT_BITS, count)) {
//             if (result_null_data[i] == BitmaskBuffer::UNIT_MAX) {
//                 // all data of 64 rows are not null
//                 for (; start_index < end_index; ++start_index) {
//                     if (!(input_bool_column_buffer->GetCompactBit(start_index))) {
//                         bitmask.SetFalse(start_index + bitmask_offset);
//                     }
//                 }
//             } else if (result_null_data[i] == BitmaskBuffer::UNIT_MIN) {
//                 // all data of 64 rows are null
//                 if (bitmask_use_unit) {
//                     if (bitmask.GetData() == nullptr) {
//                         bitmask.SetFalse(start_index + bitmask_offset);
//                     }
//                     bitmask_data[i + bitmask_unit_offset] = BitmaskBuffer::UNIT_MIN;
//                     start_index = end_index;
//                 } else {
//                     for (; start_index < end_index; ++start_index) {
//                         bitmask.SetFalse(start_index + bitmask_offset);
//                     }
//                 }
//             } else {
//                 for (; start_index < end_index; ++start_index) {
//                     if (!(input_null_mask->IsTrue(start_index)) || !(input_bool_column_buffer->GetCompactBit(start_index))) {
//                         bitmask.SetFalse(start_index + bitmask_offset);
//                     }
//                 }
//             }
//         }
//     }
// }

void PhysicalKnnScan::Init() {}

void PhysicalKnnScan::InitBlockParallelOption() {
    // TODO: Set brute force block parallel option
    // 0. [0, 50), 1 thread
    // 1. [50, 100), 2 thread
    block_parallel_options_.emplace_back(50, 2);
    // 2. [100, +Inf), 3 threads
    block_parallel_options_.emplace_back(100, 3);
}

SizeT PhysicalKnnScan::BlockScanTaskCount() const {
    const u32 block_cnt = block_column_entries_size_;
    SizeT brute_task_n = 1;
    for (const auto &[block_n, job_n] : block_parallel_options_) {
        if (block_cnt < block_n) {
            break;
        }
        brute_task_n = job_n;
    }
    return brute_task_n;
}

SizeT PhysicalKnnScan::TaskletCount() { return BlockScanTaskCount() + index_entries_size_; }

bool PhysicalKnnScan::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto *knn_scan_operator_state = static_cast<KnnScanOperatorState *>(operator_state);
    auto elem_type = knn_scan_operator_state->knn_scan_function_data_->knn_scan_shared_data_->elem_type_;
    auto dist_type = knn_scan_operator_state->knn_scan_function_data_->knn_scan_shared_data_->knn_distance_type_;
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
                    Status status = Status::NotSupport("Not implemented KNN distance");
                    LOG_ERROR(status.message());
                    RecoverableError(status);
                }
            }
            break;
        }
        default: {
            Status status = Status::NotSupport("Not implemented embedding data type");
            LOG_ERROR(status.message());
            RecoverableError(status);
        }
    }
    return true;
}

TableEntry *PhysicalKnnScan::table_collection_ptr() const { return base_table_ref_->table_entry_ptr_; }

String PhysicalKnnScan::TableAlias() const { return base_table_ref_->alias_; }

Vector<SizeT> &PhysicalKnnScan::ColumnIDs() const { return base_table_ref_->column_ids_; }

SizeT PhysicalKnnScan::GetColumnID() const {
    KnnExpression *knn_expr = knn_expression_.get();
    ColumnExpression *column_expr = static_cast<ColumnExpression *>(knn_expr->arguments()[0].get());
    return column_expr->binding().column_idx;
}

void PhysicalKnnScan::PlanWithIndex(QueryContext *query_context) { // TODO: return base entry vector
    InitBlockParallelOption();                                     // PlanWithIndex() will be called in physical planner
    Txn *txn = query_context->GetTxn();
    TransactionID txn_id = txn->TxnID();
    TxnTimeStamp begin_ts = txn->BeginTS();

    SizeT knn_column_id = GetColumnID();

    block_column_entries_ = MakeUnique<Vector<BlockColumnEntry *>>();
    index_entries_ = MakeUnique<Vector<SegmentIndexEntry *>>();

    TableEntry *table_entry = base_table_ref_->table_entry_ptr_;
    Map<u32, SharedPtr<SegmentIndexEntry>> index_entry_map;

    {
        auto map_guard = table_entry->IndexMetaMap();
        for (auto &[index_name, table_index_meta] : *map_guard) {
            auto [table_index_entry, status] = table_index_meta->GetEntryNolock(txn_id, begin_ts);
            if (!status.ok()) {
                // already dropped
                LOG_WARN(status.message());
                continue;
            }

            const String column_name = table_index_entry->index_base()->column_name();
            SizeT column_id = table_entry->GetColumnIdByName(column_name);
            if (column_id != knn_column_id) {
                // knn_column_id isn't in this table index
                continue;
            }
            // check index type
            if (auto index_type = table_index_entry->index_base()->index_type_;
                index_type != IndexType::kIVFFlat and index_type != IndexType::kHnsw) {
                LOG_TRACE(fmt::format("KnnScan: PlanWithIndex(): Skipping non-knn index."));
                continue;
            }

            // Fill the segment with index
            index_entry_map = table_index_entry->index_by_segment();
        }
    }

    // Generate task set: index segment and no index block
    BlockIndex *block_index = base_table_ref_->block_index_.get();
    for (const auto &[segment_id, segment_info] : block_index->segment_block_index_) {
        if (auto iter = index_entry_map.find(segment_id); iter != index_entry_map.end()) {
            index_entries_->emplace_back(iter->second.get());
        } else {
            const auto &block_map = segment_info.block_map_;
            for (const auto *block_entry : block_map) {
                BlockColumnEntry *block_column_entry = block_entry->GetColumnBlockEntry(knn_column_id);
                block_column_entries_->emplace_back(block_column_entry);
            }
        }
    }
    block_column_entries_size_ = block_column_entries_->size();
    index_entries_size_ = index_entries_->size();
    LOG_TRACE(fmt::format("KnnScan: brute force task: {}, index task: {}", block_column_entries_size_, index_entries_size_));
}

SizeT PhysicalKnnScan::BlockEntryCount() const { return base_table_ref_->block_index_->BlockCount(); }

template <typename DataType, template <typename, typename> typename C>
void PhysicalKnnScan::ExecuteInternal(QueryContext *query_context, KnnScanOperatorState *operator_state) {
    Txn *txn = query_context->GetTxn();
    TxnTimeStamp begin_ts = txn->BeginTS();

    if (!common_query_filter_->TryFinishBuild(txn)) {
        // not ready, abort and wait for next time
        return;
    }

    auto knn_scan_function_data = operator_state->knn_scan_function_data_.get();
    auto knn_scan_shared_data = knn_scan_function_data->knn_scan_shared_data_;

    auto dist_func = static_cast<KnnDistance1<DataType> *>(knn_scan_function_data->knn_distance_.get());
    auto merge_heap = static_cast<MergeKnn<DataType, C> *>(knn_scan_function_data->merge_knn_base_.get());
    auto query = static_cast<const DataType *>(knn_scan_shared_data->query_embedding_);

    const SizeT index_task_n = knn_scan_shared_data->index_entries_->size();
    const SizeT brute_task_n = knn_scan_shared_data->block_column_entries_->size();
    BlockIndex *block_index = knn_scan_shared_data->table_ref_->block_index_.get();
    BufferManager *buffer_mgr = query_context->storage()->buffer_manager();
    SizeT knn_column_id = GetColumnID();

    if (u64 block_column_idx =
            knn_scan_function_data->execute_block_scan_job_ ? knn_scan_shared_data->current_block_idx_++ : std::numeric_limits<u64>::max();
        block_column_idx < brute_task_n) {
        LOG_TRACE(fmt::format("KnnScan: {} brute force {}/{}", knn_scan_function_data->task_id_, block_column_idx + 1, brute_task_n));
        // brute force
        // TODO: now will try to finish all block scan job in the task
        do {
            BlockColumnEntry *block_column_entry = knn_scan_shared_data->block_column_entries_->at(block_column_idx);
            const BlockEntry *block_entry = block_column_entry->block_entry();
            const auto block_id = block_entry->block_id();
            const SegmentID segment_id = block_entry->GetSegmentEntry()->segment_id();
            const auto row_count = block_entry->row_count();
            Bitmask bitmask;
            if (this->CalculateFilterBitmask(segment_id, block_id, row_count, bitmask)) {
                // LOG_TRACE(fmt::format("KnnScan: {} brute force {}/{} not skipped after common_query_filter",
                //                       knn_scan_function_data->task_id_,
                //                       block_column_idx + 1,
                //                       brute_task_n));
                block_entry->SetDeleteBitmask(begin_ts, bitmask);
                ColumnVector column_vector = block_column_entry->GetColumnVector(buffer_mgr);
                auto data = reinterpret_cast<const DataType *>(column_vector.data());
                merge_heap->Search(query, data, knn_scan_shared_data->dimension_, dist_func->dist_func_, row_count, segment_id, block_id, bitmask);
            }
            block_column_idx = knn_scan_shared_data->current_block_idx_++;
        } while (block_column_idx < brute_task_n);
    } else if (u64 index_idx = knn_scan_shared_data->current_index_idx_++; index_idx < index_task_n) {
        LOG_TRACE(fmt::format("KnnScan: {} index {}/{}", knn_scan_function_data->task_id_, index_idx + 1, index_task_n));
        // with index
        SegmentIndexEntry *segment_index_entry = knn_scan_shared_data->index_entries_->at(index_idx);

        auto segment_id = segment_index_entry->segment_id();
        SegmentEntry *segment_entry = nullptr;
        const auto &segment_index_hashmap = base_table_ref_->block_index_->segment_block_index_;
        if (auto iter = segment_index_hashmap.find(segment_id); iter == segment_index_hashmap.end()) {
            String error_message = fmt::format("Cannot find SegmentEntry for segment id: {}", segment_id);
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        } else {
            segment_entry = iter->second.segment_entry_;
        }
        if (auto it = common_query_filter_->filter_result_.find(segment_id); it != common_query_filter_->filter_result_.end()) {
            LOG_TRACE(fmt::format("KnnScan: {} index {}/{} not skipped after common_query_filter",
                                  knn_scan_function_data->task_id_,
                                  index_idx + 1,
                                  index_task_n));
            auto segment_row_count = segment_entry->row_count();
            Bitmask bitmask;
            const std::variant<Vector<u32>, Bitmask> &filter_result = it->second;
            if (std::holds_alternative<Vector<u32>>(filter_result)) {
                const Vector<u32> &filter_result_vector = std::get<Vector<u32>>(filter_result);
                bitmask.Initialize(std::ceil(segment_row_count));
                bitmask.SetAllFalse();
                for (u32 row_id : filter_result_vector) {
                    bitmask.SetTrue(row_id);
                }
            } else {
                bitmask.ShallowCopy(std::get<Bitmask>(filter_result));
            }
            bool use_bitmask = !bitmask.IsAllTrue();

            switch (segment_index_entry->table_index_entry()->index_base()->index_type_) {
                case IndexType::kIVFFlat: {
                    BufferHandle index_handle = segment_index_entry->GetIndex();
                    auto index = static_cast<const AnnIVFFlatIndexData<DataType> *>(index_handle.GetData());
                    i32 n_probes = 1;
                    auto IVFFlatScanTemplate = [&]<typename AnnIVFFlatType, typename... OptionalFilter>(OptionalFilter &&...filter) {
                        AnnIVFFlatType ann_ivfflat_query(query,
                                                         knn_scan_shared_data->query_count_,
                                                         knn_scan_shared_data->topk_,
                                                         knn_scan_shared_data->dimension_,
                                                         knn_scan_shared_data->elem_type_);
                        ann_ivfflat_query.Begin();
                        ann_ivfflat_query.Search(index, segment_id, n_probes, std::forward<OptionalFilter>(filter)...);
                        ann_ivfflat_query.EndWithoutSort();
                        auto dists = ann_ivfflat_query.GetDistances();
                        auto row_ids = ann_ivfflat_query.GetIDs();
                        // TODO: now only work for one query
                        // FIXME: cant work for multiple queries
                        auto result_count = std::lower_bound(dists,
                                                             dists + knn_scan_shared_data->topk_,
                                                             AnnIVFFlatType::InvalidValue(),
                                                             AnnIVFFlatType::CompareDist) -
                                            dists;
                        merge_heap->Search(dists, row_ids, result_count);
                    };
                    auto IVFFlatScan = [&]<typename... OptionalFilter>(OptionalFilter &&...filter) {
                        switch (knn_scan_shared_data->knn_distance_type_) {
                            case KnnDistanceType::kL2: {
                                IVFFlatScanTemplate.template operator()<AnnIVFFlatL2<DataType>>(std::forward<OptionalFilter>(filter)...);
                                break;
                            }
                            case KnnDistanceType::kInnerProduct: {
                                IVFFlatScanTemplate.template operator()<AnnIVFFlatIP<DataType>>(std::forward<OptionalFilter>(filter)...);
                                break;
                            }
                            case KnnDistanceType::kCosine: {
                                IVFFlatScanTemplate.template operator()<AnnIVFFlatCOS<DataType>>(std::forward<OptionalFilter>(filter)...);
                                break;
                            }
                            default: {
                                Status status = Status::NotSupport("Not implemented KNN distance");
                                LOG_ERROR(status.message());
                                RecoverableError(status);
                            }
                        }
                    };
                    if (use_bitmask) {
                        if (segment_entry->CheckAnyDelete(begin_ts)) {
                            DeleteWithBitmaskFilter filter(bitmask, segment_entry, begin_ts);
                            IVFFlatScan(filter);
                        } else {
                            BitmaskFilter<SegmentOffset> filter(bitmask);
                            IVFFlatScan(filter);
                        }
                    } else {
                        SegmentOffset max_segment_offset = block_index->GetSegmentOffset(segment_id);
                        if (segment_entry->CheckAnyDelete(begin_ts)) {
                            DeleteFilter filter(segment_entry, begin_ts, max_segment_offset);
                            IVFFlatScan(filter);
                        } else {
                            IVFFlatScan();
                        }
                    }
                    break;
                }
                case IndexType::kHnsw: {
                    const auto *index_hnsw = static_cast<const IndexHnsw *>(segment_index_entry->table_index_entry()->index_base());

                    auto hnsw_search = [&](BufferHandle index_handle, bool with_lock, int chunk_id = -1) {
                        AbstractHnsw<f32, SegmentOffset> abstract_hnsw(index_handle.GetDataMut(), index_hnsw);

                        bool rerank = false;
                        for (const auto &opt_param : knn_scan_shared_data->opt_params_) {
                            if (opt_param.param_name_ == "ef") {
                                u64 ef = std::stoull(opt_param.param_value_);
                                abstract_hnsw.SetEf(ef);
                            } else if (opt_param.param_name_ == "rerank") {
                                if (abstract_hnsw.RerankDist()) {
                                    rerank = true;
                                } else {
                                    LOG_WARN("rerank for this hnsw type is not valid");
                                }
                            }
                        }

                        i64 result_n = -1;
                        for (u64 query_idx = 0; query_idx < knn_scan_shared_data->query_count_; ++query_idx) {
                            const DataType *query =
                                static_cast<const DataType *>(knn_scan_shared_data->query_embedding_) + query_idx * knn_scan_shared_data->dimension_;

                            SizeT result_n1 = 0;
                            UniquePtr<DataType[]> d_ptr = nullptr;
                            UniquePtr<SegmentOffset[]> l_ptr = nullptr;
                            if (use_bitmask) {
                                if (segment_entry->CheckAnyDelete(begin_ts)) {
                                    DeleteWithBitmaskFilter filter(bitmask, segment_entry, begin_ts);
                                    std::tie(result_n1, d_ptr, l_ptr) =
                                        abstract_hnsw.KnnSearch(query, knn_scan_shared_data->topk_, filter, with_lock);
                                } else {
                                    BitmaskFilter<SegmentOffset> filter(bitmask);
                                    std::tie(result_n1, d_ptr, l_ptr) =
                                        abstract_hnsw.KnnSearch(query, knn_scan_shared_data->topk_, filter, with_lock);
                                }
                            } else {
                                SegmentOffset max_segment_offset = block_index->GetSegmentOffset(segment_id);
                                if (segment_entry->CheckAnyDelete(begin_ts)) {
                                    DeleteFilter filter(segment_entry, begin_ts, max_segment_offset);
                                    std::tie(result_n1, d_ptr, l_ptr) =
                                        abstract_hnsw.KnnSearch(query, knn_scan_shared_data->topk_, filter, with_lock);
                                } else {
                                    if (!with_lock) {
                                        std::tie(result_n1, d_ptr, l_ptr) = abstract_hnsw.KnnSearch(query, knn_scan_shared_data->topk_, false);
                                    } else {
                                        AppendFilter filter(max_segment_offset);
                                        std::tie(result_n1, d_ptr, l_ptr) = abstract_hnsw.KnnSearch(query, knn_scan_shared_data->topk_, filter, true);
                                    }
                                }
                            }

                            if (result_n < 0) {
                                result_n = result_n1;
                            } else if (result_n != (i64)result_n1) {
                                String error_message = "KnnScan: result_n mismatch";
                                LOG_CRITICAL(error_message);
                                UnrecoverableError(error_message);
                            }

                            if (abstract_hnsw.RerankDist() && rerank) {
                                Vector<SizeT> idxes(result_n);
                                std::iota(idxes.begin(), idxes.end(), 0);
                                std::sort(idxes.begin(), idxes.end(), [&](SizeT i, SizeT j) { return l_ptr[i] < l_ptr[j]; }); // sort by segment offset
                                BlockID prev_block_id = -1;
                                ColumnVector column_vector;
                                for (SizeT idx : idxes) {
                                    SegmentOffset segment_offset = l_ptr[idx];
                                    BlockID block_id = segment_offset / DEFAULT_BLOCK_CAPACITY;
                                    BlockOffset block_offset = segment_offset % DEFAULT_BLOCK_CAPACITY;
                                    if (block_id != prev_block_id) {
                                        prev_block_id = block_id;
                                        BlockEntry *block_entry = block_index->GetBlockEntry(segment_id, block_id);
                                        BlockColumnEntry *block_column_entry = block_entry->GetColumnBlockEntry(knn_column_id);
                                        column_vector = block_column_entry->GetColumnVector(buffer_mgr);
                                    }
                                    const auto *data = reinterpret_cast<const DataType *>(column_vector.data());
                                    data += block_offset * knn_scan_shared_data->dimension_;
                                    merge_heap->Search(query, data, knn_scan_shared_data->dimension_, dist_func->dist_func_, segment_id, segment_offset);
                                }
                            } else {
                                switch (knn_scan_shared_data->knn_distance_type_) {
                                    case KnnDistanceType::kInvalid: {
                                        String error_message = "Invalid distance type";
                                        LOG_CRITICAL(error_message);
                                        UnrecoverableError(error_message);
                                    }
                                    case KnnDistanceType::kL2:
                                    case KnnDistanceType::kHamming: {
                                        break;
                                    }
                                    // FIXME:
                                    case KnnDistanceType::kCosine:
                                    case KnnDistanceType::kInnerProduct: {
                                        for (i64 i = 0; i < result_n; ++i) {
                                            d_ptr[i] = -d_ptr[i];
                                        }
                                        break;
                                    }
                                }

                                auto row_ids = MakeUniqueForOverwrite<RowID[]>(result_n);
                                for (i64 i = 0; i < result_n; ++i) {
                                    row_ids[i] = RowID{segment_id, l_ptr[i]};
                                }

                                merge_heap->Search(0, d_ptr.get(), row_ids.get(), result_n);
                            }
                        }
                    };

                    auto [chunk_index_entries, memory_index_entry] = segment_index_entry->GetHnswIndexSnapshot();
                    int i = 0;
                    for (auto &chunk_index_entry : chunk_index_entries) {
                        if (chunk_index_entry->CheckVisible(txn)) {
                            BufferHandle index_handle = chunk_index_entry->GetIndex();
                            hnsw_search(index_handle, false, i++);
                        }
                    }
                    if (memory_index_entry.get() != nullptr) {
                        BufferHandle index_handle = memory_index_entry->GetIndex();
                        hnsw_search(index_handle, true);
                    }

                    break;
                }
                default: {
                    Status status = Status::NotSupport("Not implemented index type");
                    LOG_ERROR(status.message());
                    RecoverableError(status);
                }
            }
        }
    }
    if (knn_scan_shared_data->current_index_idx_ >= index_task_n && knn_scan_shared_data->current_block_idx_ >= brute_task_n) {
        LOG_TRACE(fmt::format("KnnScan: {} task finished", knn_scan_function_data->task_id_));
        // all task Complete

        merge_heap->End();
        i64 result_n = std::min(knn_scan_shared_data->topk_, merge_heap->total_count());

        SizeT query_n = knn_scan_shared_data->query_count_;
        Vector<char *> result_dists_list;
        Vector<RowID *> row_ids_list;
        for (SizeT query_id = 0; query_id < query_n; ++query_id) {
            result_dists_list.emplace_back(reinterpret_cast<char *>(merge_heap->GetDistancesByIdx(query_id)));
            row_ids_list.emplace_back(merge_heap->GetIDsByIdx(query_id));
        }

        this->SetOutput(result_dists_list, row_ids_list, sizeof(DataType), result_n, query_context, operator_state);
        operator_state->SetComplete();
    }
}

} // namespace infinity
