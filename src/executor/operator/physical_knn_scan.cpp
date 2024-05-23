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

module physical_knn_scan;

import stl;
import query_context;

import logger;
import operator_state;
import global_block_id;
import block_index;
import base_table_ref;
import knn_scan_data;
import knn_filter;
import vector_buffer;
import knn_distance;
import third_party;
import txn;
import infinity_exception;
import default_values;
import segment_iter;
import knn_expression;
import column_expression;

import index_base;
import buffer_manager;
import merge_knn;
import knn_result_handler;
import ann_ivf_flat;
import annivfflat_index_data;
import buffer_handle;
import data_block;
import bitmask;
import bitmask_buffer;
import column_vector;
import expression_evaluator;
import expression_state;
import index_hnsw;
import status;
import hnsw_alg;
import knn_expression;
import value;
import status;
import buffer_obj;
import create_index_info;
import knn_expr;
import chunk_index_entry;

import block_entry;
import segment_index_entry;
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

void MergeIntoBitmask(const VectorBuffer *input_bool_column_buffer,
                      const SharedPtr<Bitmask> &input_null_mask,
                      const SizeT count,
                      Bitmask &bitmask,
                      bool nullable,
                      SizeT bitmask_offset = 0) {
    if ((!nullable) || (input_null_mask->IsAllTrue())) {
        for (SizeT idx = 0; idx < count; ++idx) {
            if (!(input_bool_column_buffer->GetCompactBit(idx))) {
                bitmask.SetFalse(idx + bitmask_offset);
            }
        }
    } else {
        const u64 *result_null_data = input_null_mask->GetData();
        u64 *bitmask_data = bitmask.GetData();
        SizeT unit_count = BitmaskBuffer::UnitCount(count);
        bool bitmask_use_unit = (bitmask_offset % BitmaskBuffer::UNIT_BITS) == 0;
        SizeT bitmask_unit_offset = bitmask_offset / BitmaskBuffer::UNIT_BITS;
        for (SizeT i = 0, start_index = 0, end_index = BitmaskBuffer::UNIT_BITS; i < unit_count;
             ++i, end_index = std::min(end_index + BitmaskBuffer::UNIT_BITS, count)) {
            if (result_null_data[i] == BitmaskBuffer::UNIT_MAX) {
                // all data of 64 rows are not null
                for (; start_index < end_index; ++start_index) {
                    if (!(input_bool_column_buffer->GetCompactBit(start_index))) {
                        bitmask.SetFalse(start_index + bitmask_offset);
                    }
                }
            } else if (result_null_data[i] == BitmaskBuffer::UNIT_MIN) {
                // all data of 64 rows are null
                if (bitmask_use_unit) {
                    if (bitmask.GetData() == nullptr) {
                        bitmask.SetFalse(start_index + bitmask_offset);
                    }
                    bitmask_data[i + bitmask_unit_offset] = BitmaskBuffer::UNIT_MIN;
                    start_index = end_index;
                } else {
                    for (; start_index < end_index; ++start_index) {
                        bitmask.SetFalse(start_index + bitmask_offset);
                    }
                }
            } else {
                for (; start_index < end_index; ++start_index) {
                    if (!(input_null_mask->IsTrue(start_index)) || !(input_bool_column_buffer->GetCompactBit(start_index))) {
                        bitmask.SetFalse(start_index + bitmask_offset);
                    }
                }
            }
        }
    }
}

void PhysicalKnnScan::Init() {}

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

BlockIndex *PhysicalKnnScan::GetBlockIndex() const { return base_table_ref_->block_index_.get(); }

Vector<SizeT> &PhysicalKnnScan::ColumnIDs() const { return base_table_ref_->column_ids_; }

void PhysicalKnnScan::PlanWithIndex(QueryContext *query_context) { // TODO: return base entry vector
    Txn *txn = query_context->GetTxn();
    TransactionID txn_id = txn->TxnID();
    TxnTimeStamp begin_ts = txn->BeginTS();

    KnnExpression *knn_expr = knn_expression_.get();
    ColumnExpression *column_expr = static_cast<ColumnExpression *>(knn_expr->arguments()[0].get());
    SizeT knn_column_id = column_expr->binding().column_idx;

    block_column_entries_ = MakeUnique<Vector<BlockColumnEntry *>>();
    index_entries_ = MakeUnique<Vector<SegmentIndexEntry *>>();

    TableEntry *table_entry = base_table_ref_->table_entry_ptr_;
    Map<u32, SharedPtr<SegmentIndexEntry>> index_entry_map;

    {
        auto map_guard = table_entry->IndexMetaMap();
        for (auto &[index_name, table_index_meta] : *map_guard) {
            auto [table_index_entry, status] = table_index_meta->GetEntryNolock(txn_id, begin_ts);
            if (!status.ok()) {
                // Table index entry isn't found
                LOG_ERROR(status.message());
                RecoverableError(status);
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
    LOG_TRACE(fmt::format("KnnScan: brute force task: {}, index task: {}", block_column_entries_->size(), index_entries_->size()));
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

    SizeT index_task_n = knn_scan_shared_data->index_entries_->size();
    SizeT brute_task_n = knn_scan_shared_data->block_column_entries_->size();
    BlockIndex *block_index = knn_scan_shared_data->table_ref_->block_index_.get();

    if (u64 block_column_idx = knn_scan_shared_data->current_block_idx_++; block_column_idx < brute_task_n) {
        LOG_TRACE(fmt::format("KnnScan: {} brute force {}/{}", knn_scan_function_data->task_id_, block_column_idx + 1, brute_task_n));
        // brute force
        BlockColumnEntry *block_column_entry = knn_scan_shared_data->block_column_entries_->at(block_column_idx);
        const BlockEntry *block_entry = block_column_entry->block_entry();
        const auto block_id = block_entry->block_id();
        const SegmentID segment_id = block_entry->GetSegmentEntry()->segment_id();
        if (auto it = common_query_filter_->filter_result_.find(segment_id); it != common_query_filter_->filter_result_.end()) {
            LOG_TRACE(fmt::format("KnnScan: {} brute force {}/{} not skipped after common_query_filter",
                                  knn_scan_function_data->task_id_,
                                  block_column_idx + 1,
                                  brute_task_n));
            const auto row_count = block_entry->row_count();
            BufferManager *buffer_mgr = query_context->storage()->buffer_manager();

            // filter for segment
            const std::variant<Vector<u32>, Bitmask> &filter_result = it->second;
            Bitmask bitmask;
            bitmask.Initialize(std::bit_ceil(row_count));
            const u32 block_start_offset = block_id * DEFAULT_BLOCK_CAPACITY;
            const u32 block_end_offset = block_start_offset + row_count;
            if (std::holds_alternative<Vector<u32>>(filter_result)) {
                const Vector<u32> &filter_result_vector = std::get<Vector<u32>>(filter_result);
                const auto it1 = std::lower_bound(filter_result_vector.begin(), filter_result_vector.end(), block_start_offset);
                const auto it2 = std::lower_bound(filter_result_vector.begin(), filter_result_vector.end(), block_end_offset);
                bitmask.SetAllFalse();
                for (auto it = it1; it < it2; ++it) {
                    bitmask.SetTrue(*it - block_start_offset);
                }
            } else {
                u32 u64_start_offset = block_start_offset / 64;
                u32 u64_end_offset = (block_end_offset - 1) / 64;
                if (const u64 *filter_data = std::get<Bitmask>(filter_result).GetData(); filter_data) {
                    bitmask.SetAllFalse();
                    u64 *data = bitmask.GetData();
                    for (u32 i = u64_start_offset; i <= u64_end_offset; ++i) {
                        data[i - u64_start_offset] = filter_data[i];
                    }
                }
            }
            block_entry->SetDeleteBitmask(begin_ts, bitmask);

            ColumnVector column_vector = block_column_entry->GetColumnVector(buffer_mgr);

            auto data = reinterpret_cast<const DataType *>(column_vector.data());
            merge_heap->Search(query,
                               data,
                               knn_scan_shared_data->dimension_,
                               dist_func->dist_func_,
                               row_count,
                               block_entry->segment_id(),
                               block_entry->block_id(),
                               bitmask);
        }
    } else if (u64 index_idx = knn_scan_shared_data->current_index_idx_++; index_idx < index_task_n) {
        LOG_TRACE(fmt::format("KnnScan: {} index {}/{}", knn_scan_function_data->task_id_, index_idx + 1, index_task_n));
        // with index
        SegmentIndexEntry *segment_index_entry = knn_scan_shared_data->index_entries_->at(index_idx);

        auto segment_id = segment_index_entry->segment_id();
        SegmentEntry *segment_entry = nullptr;
        const auto &segment_index_hashmap = base_table_ref_->block_index_->segment_block_index_;
        if (auto iter = segment_index_hashmap.find(segment_id); iter == segment_index_hashmap.end()) {
            UnrecoverableError(fmt::format("Cannot find SegmentEntry for segment id: {}", segment_id));
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

                        for (const auto &opt_param : knn_scan_shared_data->opt_params_) {
                            if (opt_param.param_name_ == "ef") {
                                u64 ef = std::stoull(opt_param.param_value_);
                                abstract_hnsw.SetEf(ef);
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
                                UnrecoverableError("KnnScan: result_n mismatch");
                            }

                            switch (knn_scan_shared_data->knn_distance_type_) {
                                case KnnDistanceType::kInvalid: {
                                    UnrecoverableError("Invalid distance type");
                                }
                                case KnnDistanceType::kL2:
                                case KnnDistanceType::kHamming: {
                                    break;
                                }
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

                                BlockID block_id = l_ptr[i] / DEFAULT_BLOCK_CAPACITY;
                                auto *block_entry = block_index->GetBlockEntry(segment_id, block_id);
                                if (block_entry == nullptr) {
                                    UnrecoverableError(
                                        fmt::format("Cannot find segment id: {}, block id: {}, index chunk is {}", segment_id, block_id, chunk_id));
                                } // this is for debug
                            }
                            merge_heap->Search(0, d_ptr.get(), row_ids.get(), result_n);
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

        if (!operator_state->data_block_array_.empty()) {
            UnrecoverableError("In physical_knn_scan : operator_state->data_block_array_ is not empty.");
        }
        {
            SizeT total_data_row_count = knn_scan_shared_data->query_count_ * result_n;
            SizeT row_idx = 0;
            do {
                auto data_block = DataBlock::MakeUniquePtr();
                data_block->Init(*GetOutputTypes());
                operator_state->data_block_array_.emplace_back(std::move(data_block));
                row_idx += DEFAULT_BLOCK_CAPACITY;
            } while (row_idx < total_data_row_count);
        }

        SizeT output_block_row_id = 0;
        SizeT output_block_idx = 0;
        DataBlock *output_block_ptr = operator_state->data_block_array_[output_block_idx].get();
        for (u64 query_idx = 0; query_idx < knn_scan_shared_data->query_count_; ++query_idx) {
            DataType *result_dists = merge_heap->GetDistancesByIdx(query_idx);
            RowID *row_ids = merge_heap->GetIDsByIdx(query_idx);

            for (i64 top_idx = 0; top_idx < result_n; ++top_idx) {
                SizeT id = query_idx * knn_scan_shared_data->query_count_ + top_idx;

                SegmentID segment_id = row_ids[top_idx].segment_id_;
                SegmentOffset segment_offset = row_ids[top_idx].segment_offset_;
                BlockID block_id = segment_offset / DEFAULT_BLOCK_CAPACITY;
                BlockOffset block_offset = segment_offset % DEFAULT_BLOCK_CAPACITY;

                BlockEntry *block_entry = block_index->GetBlockEntry(segment_id, block_id);
                if (block_entry == nullptr) {
                    UnrecoverableError(fmt::format("Cannot find segment id: {}, block id: {}", segment_id, block_id));
                }

                if (output_block_row_id == DEFAULT_BLOCK_CAPACITY) {
                    output_block_ptr->Finalize();
                    ++output_block_idx;
                    output_block_ptr = operator_state->data_block_array_[output_block_idx].get();
                    output_block_row_id = 0;
                }

                SizeT column_n = base_table_ref_->column_ids_.size();
                for (SizeT i = 0; i < column_n; ++i) {
                    SizeT column_id = base_table_ref_->column_ids_[i];
                    auto *block_column_entry = block_entry->GetColumnBlockEntry(column_id);
                    ColumnVector &&column_vector = block_column_entry->GetColumnVector(query_context->storage()->buffer_manager());

                    output_block_ptr->column_vectors[i]->AppendWith(column_vector, block_offset, 1);
                }
                output_block_ptr->AppendValueByPtr(column_n, (ptr_t)&result_dists[id]);
                output_block_ptr->AppendValueByPtr(column_n + 1, (ptr_t)&row_ids[id]);

                ++output_block_row_id;
            }
        }
        output_block_ptr->Finalize();
        operator_state->SetComplete();
    }
}

} // namespace infinity
