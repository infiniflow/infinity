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

#include <algorithm>
#include <string>
import stl;
import query_context;
import parser;
import logger;
import operator_state;
import global_block_id;
import block_index;
import base_table_ref;
import knn_scan_data;
import column_buffer;
import knn_distance;
import third_party;

import infinity_exception;
import catalog;
import default_values;

import knn_expression;
import column_expression;

import index_base;
import buffer_manager;
import merge_knn;
import knn_result_handler;
import index_def;
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

import hnsw_alg;
import plain_store;
import lvq_store;
import dist_func_l2;
import dist_func_ip;
import knn_expression;
import value;

module physical_knn_scan;

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
            ColumnBuffer column_buffer = current_block_entry->GetColumnBlockEntry(column_id)->GetColumnData(buffer_mgr);
            output->column_vectors[output_column_id++]->AppendWith(column_buffer, 0, row_count);
        }
    }
    output->Finalize();
}

void MergeIntoBitmask(const u8 *__restrict input_bool_column,
                      const SharedPtr<Bitmask> &input_null_mask,
                      const SizeT count,
                      Bitmask &bitmask,
                      bool nullable,
                      SizeT bitmask_offset = 0) {
    if ((!nullable) || (input_null_mask->IsAllTrue())) {
        for (SizeT idx = 0; idx < count; ++idx) {
            if (input_bool_column[idx] == 0) {
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
             ++i, end_index = Min(end_index + BitmaskBuffer::UNIT_BITS, count)) {
            if (result_null_data[i] == BitmaskBuffer::UNIT_MAX) {
                // all data of 64 rows are not null
                for (; start_index < end_index; ++start_index) {
                    if (input_bool_column[start_index] == 0) {
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
                    if (!(input_null_mask->IsTrue(start_index)) || (input_bool_column[start_index] == 0)) {
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
    auto elem_type = knn_scan_operator_state->knn_scan_function_data_->shared_data_->elem_type_;
    auto dist_type = knn_scan_operator_state->knn_scan_function_data_->shared_data_->knn_distance_type_;
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

TableEntry *PhysicalKnnScan::table_collection_ptr() const { return base_table_ref_->table_entry_ptr_; }

String PhysicalKnnScan::TableAlias() const { return base_table_ref_->alias_; }

BlockIndex *PhysicalKnnScan::GetBlockIndex() const { return base_table_ref_->block_index_.get(); }

Vector<SizeT> &PhysicalKnnScan::ColumnIDs() const { return base_table_ref_->column_ids_; }

void PhysicalKnnScan::PlanWithIndex(QueryContext *query_context) { // TODO: return base entry vector
    u64 txn_id = query_context->GetTxn()->TxnID();
    TxnTimeStamp begin_ts = query_context->GetTxn()->BeginTS();

    KnnExpression *knn_expr = knn_expression_.get();
    ColumnExpression *column_expr = static_cast<ColumnExpression *>(knn_expr->arguments()[0].get());
    SizeT knn_column_id = column_expr->binding().column_idx;

    block_column_entries_ = MakeUnique<Vector<BlockColumnEntry *>>();
    index_entries_ = MakeUnique<Vector<SegmentColumnIndexEntry *>>();

    TableEntry *table_entry = base_table_ref_->table_entry_ptr_;
    HashMap<u32, Vector<SegmentColumnIndexEntry *>> index_entry_map;
    for (auto &[index_name, table_index_meta] : table_entry->index_meta_map()) {
        auto [table_index_entry, status] = table_index_meta->GetEntry(txn_id, begin_ts);
        if (!status.ok()) {
            // FIXME: not found index means exception??
            Error<StorageException>("Cannot find index entry.");
        }

        auto& index_map = table_index_entry->column_index_map();
        auto column_index_iter = index_map.find(knn_column_id);
        if (column_index_iter == index_map.end()) {
            // knn_column_id isn't in this table index
            continue;
        }

        // Fill the segment with index
        ColumnIndexEntry *column_index_entry = index_map[knn_column_id].get();
        const HashMap<u32, SharedPtr<SegmentColumnIndexEntry>>& index_by_segment = column_index_entry->index_by_segment();
        index_entry_map.reserve(index_by_segment.size());
        for (auto &[segment_id, segment_column_index] : index_by_segment) {
            index_entry_map[segment_id].emplace_back(segment_column_index.get());
        }
    }

    // Generate task set: index segment and no index block
    BlockIndex *block_index = base_table_ref_->block_index_.get();
    for (SegmentEntry *segment_entry : block_index->segments_) {
        if (auto iter = index_entry_map.find(segment_entry->segment_id()); iter != index_entry_map.end()) {
            index_entries_->emplace_back(iter->second[0]);
        } else {
            const auto& block_entries = segment_entry->block_entries();
            for (auto &block_entry : block_entries) {
                BlockColumnEntry *block_column_entry = block_entry->GetColumnBlockEntry(knn_column_id);
                block_column_entries_->emplace_back(block_column_entry);
            }
        }
    }
    LOG_TRACE(Format("KnnScan: brute force task: {}, index task: {}", block_column_entries_->size(), index_entries_->size()));
}

SizeT PhysicalKnnScan::BlockEntryCount() const { return base_table_ref_->block_index_->BlockCount(); }

template <typename DataType, template <typename, typename> typename C>
void PhysicalKnnScan::ExecuteInternal(QueryContext *query_context, KnnScanOperatorState *operator_state) {
    auto knn_scan_function_data = operator_state->knn_scan_function_data_.get();
    auto knn_scan_shared_data = knn_scan_function_data->shared_data_;

    auto dist_func = static_cast<KnnDistance1<DataType> *>(knn_scan_function_data->knn_distance_.get());
    auto merge_heap = static_cast<MergeKnn<DataType, C> *>(knn_scan_function_data->merge_knn_base_.get());
    auto query = static_cast<const DataType *>(knn_scan_shared_data->query_embedding_);

    SizeT index_task_n = knn_scan_shared_data->index_entries_->size();
    SizeT brute_task_n = knn_scan_shared_data->block_column_entries_->size();

    if (u64 block_column_idx = knn_scan_shared_data->current_block_idx_++; block_column_idx < brute_task_n) {
        LOG_TRACE(Format("KnnScan: {} brute force {}/{}", knn_scan_function_data->task_id_, block_column_idx + 1, brute_task_n));
        // brute force
        BlockColumnEntry *block_column_entry = knn_scan_shared_data->block_column_entries_->at(block_column_idx);
        const BlockEntry *block_entry = block_column_entry->block_entry();
        const auto row_count = block_entry->row_count();
        BufferManager *buffer_mgr = query_context->storage()->buffer_manager();

        Bitmask bitmask;
        bitmask.Initialize(std::bit_ceil(row_count));
        if (filter_expression_) {
            auto db_for_filter = knn_scan_function_data->db_for_filter_.get();
            auto &filter_state_ = knn_scan_function_data->filter_state_;
            auto &bool_column = knn_scan_function_data->bool_column_;
            // filter and build bitmask, if filter_expression_ != nullptr
            db_for_filter->Reset(row_count);
            ReadDataBlock(db_for_filter, buffer_mgr, row_count, block_entry, base_table_ref_->column_ids_);
            bool_column->Initialize(ColumnVectorType::kFlat, row_count);
            ExpressionEvaluator expr_evaluator;
            expr_evaluator.Init(db_for_filter);
            expr_evaluator.Execute(filter_expression_, filter_state_, bool_column);
            const auto *bool_column_ptr = (const u8 *)(bool_column->data());
            SharedPtr<Bitmask> &null_mask = bool_column->nulls_ptr_;
            MergeIntoBitmask(bool_column_ptr, null_mask, row_count, bitmask, true);
            bool_column->Reset();
        }

        ColumnBuffer column_buffer = block_column_entry->GetColumnData(buffer_mgr);

        auto data = reinterpret_cast<const DataType *>(column_buffer.GetAll());
        merge_heap->Search(query,
                           data,
                           knn_scan_shared_data->dimension_,
                           dist_func->dist_func_,
                           row_count,
                           block_entry->segment_id(),
                           block_entry->block_id(),
                           bitmask);
    } else if (u64 index_idx = knn_scan_shared_data->current_index_idx_++; index_idx < index_task_n) {
        LOG_TRACE(Format("KnnScan: {} index {}/{}", knn_scan_function_data->task_id_, index_idx + 1, index_task_n));
        // with index
        SegmentColumnIndexEntry *segment_column_index_entry = knn_scan_shared_data->index_entries_->at(index_idx);
        BufferManager *buffer_mgr = query_context->storage()->buffer_manager();

        auto segment_id = segment_column_index_entry->segment_id();
        SegmentEntry *segment_entry = nullptr;
        auto &segment_index_hashmap = base_table_ref_->block_index_->segment_index_;
        if (auto iter = segment_index_hashmap.find(segment_id); iter == segment_index_hashmap.end()) {
            Error<ExecutorException>(Format("Cannot find SegmentEntry for segment id: {}", segment_id));
        } else {
            segment_entry = iter->second;
        }
        auto segment_row_count = segment_entry->row_count();
        Bitmask bitmask;
        bitmask.Initialize(std::bit_ceil(segment_row_count));
        if (filter_expression_) {
            SizeT segment_row_count_real = 0;
            auto db_for_filter = knn_scan_function_data->db_for_filter_.get();
            auto &filter_state_ = knn_scan_function_data->filter_state_;
            auto &bool_column = knn_scan_function_data->bool_column_;
            // filter and build bitmask, if filter_expression_ != nullptr
            ExpressionEvaluator expr_evaluator;
            const auto& block_entries = segment_entry->block_entries();
            for (auto &block_entry : block_entries) {
                auto row_count = block_entry->row_count();
                db_for_filter->Reset(row_count);
                ReadDataBlock(db_for_filter, buffer_mgr, row_count, block_entry.get(), base_table_ref_->column_ids_);
                bool_column->Initialize(ColumnVectorType::kFlat, row_count);
                expr_evaluator.Init(db_for_filter);
                expr_evaluator.Execute(filter_expression_, filter_state_, bool_column);
                const auto *bool_column_ptr = (const u8 *)(bool_column->data());
                SharedPtr<Bitmask> &null_mask = bool_column->nulls_ptr_;
                MergeIntoBitmask(bool_column_ptr, null_mask, row_count, bitmask, true, segment_row_count_real);
                segment_row_count_real += row_count;
                bool_column->Reset();
            }
            if (segment_row_count_real != segment_row_count) {
                Error<ExecutorException>(Format("Segment_row_count mismatch: In segment {}: segment_row_count_real: {}, segment_row_count: {}",
                                                segment_id,
                                                segment_row_count_real,
                                                segment_row_count));
            }
        }
        // bool use_bitmask = !bitmask.IsAllTrue();

        switch (segment_column_index_entry->column_index_entry()->index_base_ptr()->index_type_) {
            case IndexType::kIVFFlat: {
                BufferHandle index_handle = segment_column_index_entry->GetIndex();
                auto index = static_cast<const AnnIVFFlatIndexData<DataType> *>(index_handle.GetData());
                i32 n_probes = 1;
                auto IVFFlatScan = [&]<typename AnnIVFFlatType>() {
                    AnnIVFFlatType ann_ivfflat_query(query,
                                                     knn_scan_shared_data->query_count_,
                                                     knn_scan_shared_data->topk_,
                                                     knn_scan_shared_data->dimension_,
                                                     knn_scan_shared_data->elem_type_);
                    ann_ivfflat_query.Begin();
                    ann_ivfflat_query.Search(index, segment_id, n_probes, bitmask);
                    ann_ivfflat_query.EndWithoutSort();
                    auto dists = ann_ivfflat_query.GetDistances();
                    auto row_ids = ann_ivfflat_query.GetIDs();
                    // TODO: now only work for one query
                    // FIXME: cant work for multiple queries
                    auto result_count =
                        std::lower_bound(dists, dists + knn_scan_shared_data->topk_, AnnIVFFlatType::InvalidValue(), AnnIVFFlatType::CompareDist) -
                        dists;
                    merge_heap->Search(dists, row_ids, result_count);
                };
                switch (knn_scan_shared_data->knn_distance_type_) {
                    case KnnDistanceType::kL2: {
                        IVFFlatScan.template operator()<AnnIVFFlatL2<DataType>>();
                        break;
                    }
                    case KnnDistanceType::kInnerProduct: {
                        IVFFlatScan.template operator()<AnnIVFFlatIP<DataType>>();
                        break;
                    }
                    default: {
                        Error<ExecutorException>("Not implemented");
                    }
                }
                break;
            }
            case IndexType::kHnsw: {
                BufferHandle index_handle = segment_column_index_entry->GetIndex();
                auto index_hnsw = static_cast<const IndexHnsw *>(segment_column_index_entry->column_index_entry()->index_base_ptr());
                auto KnnScanOld = [&](auto *index) {
                    Vector<DataType> dists(knn_scan_shared_data->topk_ * knn_scan_shared_data->query_count_);
                    Vector<RowID> row_ids(knn_scan_shared_data->topk_ * knn_scan_shared_data->query_count_);

                    for (const auto &opt_param : knn_scan_shared_data->opt_params_) {
                        if (opt_param.param_name_ == "ef") {
                            u64 ef = std::stoull(opt_param.param_value_);
                            index->SetEf(ef);
                        }
                    }

                    i64 result_n = -1;
                    for (u64 query_idx = 0; query_idx < knn_scan_shared_data->query_count_; ++query_idx) {
                        const DataType *query =
                            static_cast<const DataType *>(knn_scan_shared_data->query_embedding_) + query_idx * knn_scan_shared_data->dimension_;
                        MaxHeap<Pair<DataType, u64>> heap = index->KnnSearch(query, knn_scan_shared_data->topk_, bitmask);
                        if (result_n < 0) {
                            result_n = heap.size();
                        } else if (result_n != (i64)heap.size()) {
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
                auto KnnScanUseHeap = [&]<typename LabelType>(auto *index) {
                    if constexpr (!std::is_same_v<LabelType, u64>) {
                        Error<ExecutorException>("Bug: Hnsw LabelType must be u64");
                    }
                    for (const auto &opt_param : knn_scan_shared_data->opt_params_) {
                        if (opt_param.param_name_ == "ef") {
                            u64 ef = std::stoull(opt_param.param_value_);
                            index->SetEf(ef);
                        }
                    }
                    i64 result_n = -1;
                    for (u64 query_idx = 0; query_idx < knn_scan_shared_data->query_count_; ++query_idx) {
                        const DataType *query =
                            static_cast<const DataType *>(knn_scan_shared_data->query_embedding_) + query_idx * knn_scan_shared_data->dimension_;
                        auto search_result = index->KnnSearchReturnPair(query, knn_scan_shared_data->topk_, bitmask);
                        auto &[result_size, unique_ptr_pair] = search_result;
                        auto &[d_ptr, l_ptr] = unique_ptr_pair;
                        if (result_n < 0) {
                            result_n = result_size;
                        } else if (result_n != (i64)result_size) {
                            throw ExecutorException("Bug");
                        }
                        if (result_size <= 0) {
                            continue;
                        }
                        UniquePtr<RowID[]> row_ids_ptr;
                        RowID *row_ids = nullptr;
                        if constexpr (sizeof(RowID) == sizeof(LabelType)) {
                            row_ids = reinterpret_cast<RowID *>(l_ptr.get());
                        } else {
                            row_ids_ptr = MakeUniqueForOverwrite<RowID[]>(result_size);
                            row_ids = row_ids_ptr.get();
                        }
                        for (SizeT i = 0; i < result_size; ++i) {
                            row_ids[i] = RowID::FromUint64(l_ptr[i]);
                        }
                        switch (knn_scan_shared_data->knn_distance_type_) {
                            case KnnDistanceType::kInvalid: {
                                throw ExecutorException("Bug");
                            }
                            case KnnDistanceType::kL2:
                            case KnnDistanceType::kHamming: {
                                break;
                            }
                            case KnnDistanceType::kCosine:
                            case KnnDistanceType::kInnerProduct: {
                                for (SizeT i = 0; i < result_size; ++i) {
                                    d_ptr[i] = -d_ptr[i];
                                }
                                break;
                            }
                        }
                        merge_heap->Search(0, d_ptr.get(), row_ids, result_size);
                    }
                };
                auto KnnScan = [&](auto *index) {
                    using LabelType = typename std::remove_pointer_t<decltype(index)>::HnswLabelType;
                    KnnScanUseHeap.template operator()<LabelType>(index);
                };
                switch (index_hnsw->encode_type_) {
                    case HnswEncodeType::kPlain: {
                        switch (index_hnsw->metric_type_) {
                            case MetricType::kMerticInnerProduct: {
                                using Hnsw = KnnHnsw<f32, u64, PlainStore<f32>, PlainIPDist<f32>>;
                                // Fixme: const_cast here. may have bug.
                                KnnScan(const_cast<Hnsw *>(static_cast<const Hnsw *>(index_handle.GetData())));
                                break;
                            }
                            case MetricType::kMerticL2: {
                                using Hnsw = KnnHnsw<f32, u64, PlainStore<f32>, PlainL2Dist<f32>>;
                                KnnScan(const_cast<Hnsw *>(static_cast<const Hnsw *>(index_handle.GetData())));
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
                                KnnScan(const_cast<Hnsw *>(static_cast<const Hnsw *>(index_handle.GetData())));
                                break;
                            }
                            case MetricType::kMerticL2: {
                                using Hnsw = KnnHnsw<f32, u64, LVQStore<f32, i8, LVQL2Cache<f32, i8>>, LVQL2Dist<f32, i8>>;
                                KnnScan(const_cast<Hnsw *>(static_cast<const Hnsw *>(index_handle.GetData())));
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
        BlockIndex *block_index = knn_scan_shared_data->table_ref_->block_index_.get();

        merge_heap->End();
        i64 result_n = Min(knn_scan_shared_data->topk_, merge_heap->total_count());

        if (!operator_state->data_block_array_.empty()) {
            Error<ExecutorException>("In physical_knn_scan : operator_state->data_block_array_ is not empty.");
        }
        {
            SizeT total_data_row_count = knn_scan_shared_data->query_count_ * result_n;
            SizeT row_idx = 0;
            do {
                auto data_block = DataBlock::MakeUniquePtr();
                data_block->Init(*GetOutputTypes());
                operator_state->data_block_array_.emplace_back(Move(data_block));
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

                u32 segment_id = row_ids[top_idx].segment_id_;
                u32 segment_offset = row_ids[top_idx].segment_offset_;
                u16 block_id = segment_offset / DEFAULT_BLOCK_CAPACITY;
                u16 block_offset = segment_offset % DEFAULT_BLOCK_CAPACITY;

                BlockEntry *block_entry = block_index->GetBlockEntry(segment_id, block_id);
                if (block_entry == nullptr) {
                    Error<ExecutorException>(Format("Cannot find block segment id: {}, block id: {}", segment_id, block_id));
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
                    ColumnBuffer column_buffer =
                                     block_entry->GetColumnBlockEntry(column_id)->GetColumnData(query_context->storage()->buffer_manager());
                    auto &column_type = block_entry->GetColumnBlockEntry(column_id)->column_type();
                    if (column_type->Plain()) {
                        const_ptr_t ptr = column_buffer.GetValueAt(block_offset, *column_type);
                        output_block_ptr->AppendValueByPtr(i, ptr);
                    } else {
                        if (column_type->type() != LogicalType::kVarchar) {
                            Error<NotImplementException>("Not implement complex type reading from column buffer.");
                        }
                        auto [varchar_ptr, data_size] = column_buffer.GetVarcharAt(block_offset);
                        Value value = Value::MakeVarchar(varchar_ptr, data_size);
                        output_block_ptr->AppendValue(column_id, value);
                    }
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
