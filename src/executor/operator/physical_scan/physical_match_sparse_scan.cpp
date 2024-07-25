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

#include <vector>

module physical_match_sparse_scan;

import stl;
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import table_entry;
import match_sparse_expression;
import base_table_ref;
import block_index;
import column_expression;
import create_index_info;
import data_type;
import common_query_filter;
import default_values;
import logical_type;
import match_sparse_scan_function_data;
import block_entry;
import logger;
import infinity_exception;
import third_party;
import buffer_manager;
import expression_evaluator;
import expression_state;
import base_expression;
import column_vector;
import data_block;
import sparse_info;
import internal_types;
import knn_result_handler;
import merge_knn;
import match_sparse_scan_function_data;
import fix_heap;
import global_block_id;
import bitmask;
import txn;
import bmp_index_file_worker;
import segment_index_entry;
import buffer_handle;
import sparse_util;
import bmp_util;
import knn_filter;
import segment_entry;
import abstract_bmp;

namespace infinity {

PhysicalMatchSparseScan::PhysicalMatchSparseScan(u64 id,
                                                 u64 table_index,
                                                 SharedPtr<BaseTableRef> base_table_ref,
                                                 SharedPtr<MatchSparseExpression> match_sparse_expression,
                                                 const SharedPtr<CommonQueryFilter> &common_query_filter,
                                                 SharedPtr<Vector<LoadMeta>> load_metas)
    : PhysicalFilterScanBase(id, PhysicalOperatorType::kMatchSparseScan, nullptr, nullptr, base_table_ref, common_query_filter, load_metas),
      table_index_(table_index), match_sparse_expr_(std::move(match_sparse_expression)) {}

void PhysicalMatchSparseScan::Init() { search_column_id_ = match_sparse_expr_->column_expr_->binding().column_idx; }

SharedPtr<Vector<String>> PhysicalMatchSparseScan::GetOutputNames() const {
    SharedPtr<Vector<String>> result_names = MakeShared<Vector<String>>();
    const Vector<String> &column_names = *base_table_ref_->column_names_;
    result_names->reserve(column_names.size() + 2);
    for (const auto &name : column_names) {
        result_names->emplace_back(name);
    }
    result_names->emplace_back(COLUMN_NAME_SCORE);
    result_names->emplace_back(COLUMN_NAME_ROW_ID);
    return result_names;
}

SharedPtr<Vector<SharedPtr<DataType>>> PhysicalMatchSparseScan::GetOutputTypes() const {
    SharedPtr<Vector<SharedPtr<DataType>>> result_types = MakeShared<Vector<SharedPtr<DataType>>>();
    const Vector<SharedPtr<DataType>> &column_types = *base_table_ref_->column_types_;
    result_types->reserve(column_types.size() + 2);
    for (const auto &type : column_types) {
        result_types->emplace_back(type);
    }
    result_types->emplace_back(MakeShared<DataType>(match_sparse_expr_->Type()));
    result_types->emplace_back(MakeShared<DataType>(LogicalType::kRowID));
    return result_types;
}

SizeT PhysicalMatchSparseScan::TaskletCount() {
    SizeT ret = base_table_ref_->block_index_->BlockCount();
    if (base_table_ref_->index_index_.get() != nullptr) {
        const auto &index_snapshots = base_table_ref_->index_index_->index_snapshots_vec_;
        if (index_snapshots.size() != 1) {
            UnrecoverableError("Multiple index snapshots are not supported.");
        }
        ret = index_snapshots[0]->segment_index_entries_.size();
    }
    return ret;
}

SizeT PhysicalMatchSparseScan::GetTaskletCount(QueryContext *query_context) {
    Txn *txn = query_context->GetTxn();
    TableEntry *table_entry = base_table_ref_->table_entry_ptr_;
    BlockIndex *block_index = base_table_ref_->block_index_.get();
    SizeT ret = block_index->BlockCount();

    ColumnExpression *column_expr = static_cast<ColumnExpression *>(match_sparse_expr_->arguments()[0].get());
    SizeT knn_column_id = column_expr->binding().column_idx;
    {
        auto map_guard = table_entry->IndexMetaMap();
        for (const auto &[idx_name, table_index_meta] : *map_guard) {
            auto [table_index_entry, status] = table_index_meta->GetEntryNolock(txn->TxnID(), txn->BeginTS());
            if (!status.ok()) {
                // maybe already dropped
                LOG_WARN(status.message());
                continue;
            }
            const String column_name = table_index_entry->index_base()->column_name();
            SizeT column_id = table_entry->GetColumnIdByName(column_name);
            if (column_id != knn_column_id) {
                continue;
            }
            if (auto index_type = table_index_entry->index_base()->index_type_; index_type != IndexType::kBMP) {
                LOG_TRACE(fmt::format("KnnScan: PlanWithIndex(): Skipping non-knn index."));
                continue;
            }
            if (base_table_ref_->index_index_.get() == nullptr) {
                base_table_ref_->index_index_ = MakeShared<IndexIndex>();
            }
            IndexIndex *index_index = base_table_ref_->index_index_.get();
            auto index_snapshot = index_index->Insert(table_index_entry, txn);
            ret = index_snapshot->segment_index_entries_.size();
            break;
        }
    }

    return ret;
}

Vector<SharedPtr<Vector<SegmentID>>> PhysicalMatchSparseScan::PlanWithIndex(Vector<SharedPtr<Vector<GlobalBlockID>>> &block_groups,
                                                                            i64 parallel_count) {
    Vector<SharedPtr<Vector<SegmentID>>> segment_groups(parallel_count);
    for (i64 i = 0; i < parallel_count; ++i) {
        segment_groups[i] = MakeShared<Vector<SegmentID>>();
    }
    if ((i64)block_groups.size() != parallel_count) {
        UnrecoverableError("block_groups.size() != parallel_count");
    }
    IndexIndex *index_index = base_table_ref_->index_index_.get();
    if (index_index != nullptr) {
        block_groups.assign(parallel_count, MakeShared<Vector<GlobalBlockID>>());
        SizeT group_idx = 0;
        for (const auto &[idx_name, index_snapshot] : index_index->index_snapshots_) {
            for (const auto &[segment_id, segment_index_entry] : index_snapshot->segment_index_entries_) {
                segment_groups[group_idx]->push_back(segment_id);
                group_idx = (group_idx + 1) % parallel_count;
            }
            break;
        }
    }
    return segment_groups;
}

bool PhysicalMatchSparseScan::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto *match_sparse_scan_state = static_cast<MatchSparseScanOperatorState *>(operator_state);
    MatchSparseScanFunctionData &function_data = match_sparse_scan_state->match_sparse_scan_function_data_;
    SharedPtr<DataBlock> query_data = function_data.query_data_;

    if (!function_data.evaluated_) {
        ExpressionEvaluator evaluator;
        evaluator.Init(nullptr);

        SharedPtr<BaseExpression> query_expr = match_sparse_expr_->query_sparse_expr_;

        Vector<SharedPtr<DataType>> output_types;
        output_types.push_back(MakeShared<DataType>(query_expr->Type()));
        query_data->Init(output_types);
        SharedPtr<ExpressionState> expr_state = ExpressionState::CreateState(query_expr);
        evaluator.Execute(query_expr, expr_state, query_data->column_vectors[0]);

        function_data.evaluated_ = true;
    }

    const auto &column_type = match_sparse_expr_->column_expr_->Type();
    const auto *sparse_info = static_cast<const SparseInfo *>(column_type.type_info().get());
    switch (sparse_info->DataType()) {
        case EmbeddingDataType::kElemBit: {
            ExecuteInner<bool>(query_context, match_sparse_scan_state, sparse_info, match_sparse_expr_->metric_type_);
            break;
        }
        case EmbeddingDataType::kElemFloat: {
            ExecuteInner<float>(query_context, match_sparse_scan_state, sparse_info, match_sparse_expr_->metric_type_);
            break;
        }
        case EmbeddingDataType::kElemDouble: {
            ExecuteInner<double>(query_context, match_sparse_scan_state, sparse_info, match_sparse_expr_->metric_type_);
            break;
        }
        case EmbeddingDataType::kElemInt8: {
            ExecuteInner<i8>(query_context, match_sparse_scan_state, sparse_info, match_sparse_expr_->metric_type_);
            break;
        }
        case EmbeddingDataType::kElemInt16: {
            ExecuteInner<i16>(query_context, match_sparse_scan_state, sparse_info, match_sparse_expr_->metric_type_);
            break;
        }
        case EmbeddingDataType::kElemInt32: {
            ExecuteInner<i32>(query_context, match_sparse_scan_state, sparse_info, match_sparse_expr_->metric_type_);
            break;
        }
        case EmbeddingDataType::kElemInt64: {
            ExecuteInner<i64>(query_context, match_sparse_scan_state, sparse_info, match_sparse_expr_->metric_type_);
            break;
        }
        case EmbeddingDataType::kElemUInt8: {
            ExecuteInner<u8>(query_context, match_sparse_scan_state, sparse_info, match_sparse_expr_->metric_type_);
            break;
        }
        default: {
            UnrecoverableError("Not implemented yet");
        }
    }
    return true;
}

template <typename DataType>
void PhysicalMatchSparseScan::ExecuteInner(QueryContext *query_context,
                                           MatchSparseScanOperatorState *operator_state,
                                           const SparseInfo *sparse_info,
                                           const SparseMetricType &metric_type) {
    switch (sparse_info->IndexType()) {
        case EmbeddingDataType::kElemInt8: {
            ExecuteInner<DataType, i8>(query_context, operator_state, metric_type);
            break;
        }
        case EmbeddingDataType::kElemInt16: {
            ExecuteInner<DataType, i16>(query_context, operator_state, metric_type);
            break;
        }
        case EmbeddingDataType::kElemInt32: {
            ExecuteInner<DataType, i32>(query_context, operator_state, metric_type);
            break;
        }
        case EmbeddingDataType::kElemInt64: {
            ExecuteInner<DataType, i64>(query_context, operator_state, metric_type);
            break;
        }
        default: {
            String embedding_str = EmbeddingType::EmbeddingDataType2String(sparse_info->IndexType());
            UnrecoverableError(fmt::format("Invalid index type: {}", embedding_str));
        }
    }
}

template <typename DataType, typename IdxType>
void PhysicalMatchSparseScan::ExecuteInner(QueryContext *query_context,
                                           MatchSparseScanOperatorState *operator_state,
                                           const SparseMetricType &metric_type) {
    switch (metric_type) {
        case SparseMetricType::kInnerProduct: {
            ExecuteInner<DataType, IdxType, CompareMin>(query_context, operator_state);
            break;
        }
        default: {
            UnrecoverableError(fmt::format("SparseMetricType: {} is not supported.", (i8)metric_type));
        }
    }
}

template <typename DataT, typename IdxType, template <typename, typename> typename C>
void PhysicalMatchSparseScan::ExecuteInner(QueryContext *query_context, MatchSparseScanOperatorState *match_sparse_scan_state) {
    DataType result_type = match_sparse_expr_->Type();
    switch (result_type.type()) {
        case LogicalType::kTinyInt: {
            ExecuteInner<DataT, IdxType, i8, C>(query_context, match_sparse_scan_state);
            break;
        }
        case LogicalType::kSmallInt: {
            ExecuteInner<DataT, IdxType, i16, C>(query_context, match_sparse_scan_state);
            break;
        }
        case LogicalType::kInteger: {
            ExecuteInner<DataT, IdxType, i32, C>(query_context, match_sparse_scan_state);
            break;
        }
        case LogicalType::kBigInt: {
            ExecuteInner<DataT, IdxType, i64, C>(query_context, match_sparse_scan_state);
            break;
        }
        case LogicalType::kFloat: {
            ExecuteInner<DataT, IdxType, float, C>(query_context, match_sparse_scan_state);
            break;
        }
        case LogicalType::kDouble: {
            ExecuteInner<DataT, IdxType, double, C>(query_context, match_sparse_scan_state);
            break;
        }
        default: {
            UnrecoverableError("Invalid result type.");
        }
    }
}

template <typename DataT, typename IdxType, typename ResultType, template <typename, typename> typename C>
void PhysicalMatchSparseScan::ExecuteInner(QueryContext *query_context, MatchSparseScanOperatorState *match_sparse_scan_state) {
    MatchSparseScanFunctionData &function_data = match_sparse_scan_state->match_sparse_scan_function_data_;

    using MergeHeap = MergeKnn<ResultType, C, ResultType>;
    auto *merge_heap = static_cast<MergeHeap *>(function_data.merge_knn_base_.get());
    if constexpr (std::is_same_v<DataT, bool>) {
        using DistFuncT = SparseBitDistance<IdxType, ResultType>;
        auto *dist_func = static_cast<DistFuncT *>(function_data.sparse_distance_.get());

        ExecuteInnerT<DistFuncT, MergeHeap, ResultType>(dist_func, merge_heap, query_context, match_sparse_scan_state);
    } else {
        using DistFuncT = SparseDistance<DataT, IdxType, ResultType>;
        auto *dist_func = static_cast<DistFuncT *>(function_data.sparse_distance_.get());

        ExecuteInnerT<DistFuncT, MergeHeap, ResultType>(dist_func, merge_heap, query_context, match_sparse_scan_state);
    }
}

template <typename DistFunc, typename MergeHeap, typename ResultType>
void PhysicalMatchSparseScan::ExecuteInnerT(DistFunc *dist_func,
                                            MergeHeap *merge_heap,
                                            QueryContext *query_context,
                                            MatchSparseScanOperatorState *match_sparse_scan_state) {
    Txn *txn = query_context->GetTxn();
    const TxnTimeStamp begin_ts = txn->BeginTS();
    if (!common_query_filter_->TryFinishBuild(txn)) {
        // not ready, abort and wait for next time
        return;
    }

    SizeT query_n = match_sparse_expr_->query_n_;
    SizeT topn = match_sparse_expr_->topn_;
    MatchSparseScanFunctionData &function_data = match_sparse_scan_state->match_sparse_scan_function_data_;

    if (merge_heap == nullptr) {
        auto merge_knn_ptr = MakeUnique<MergeHeap>(query_n, topn);
        merge_heap = merge_knn_ptr.get();
        merge_knn_ptr->Begin();
        function_data.merge_knn_base_ = std::move(merge_knn_ptr);

        auto dist_func_ptr = MakeUnique<DistFunc>(match_sparse_expr_->metric_type_);
        dist_func = dist_func_ptr.get();
        function_data.sparse_distance_ = std::move(dist_func_ptr);
    }

    BufferManager *buffer_mgr = query_context->storage()->buffer_manager();
    const Vector<GlobalBlockID> &block_ids = *function_data.global_block_ids_;
    const Vector<SegmentID> &segment_ids = *function_data.segment_ids_;
    auto &block_ids_idx = function_data.current_block_ids_idx_;
    auto &segment_ids_idx = function_data.current_segment_ids_idx_;

    const ColumnVector &query_vector = *function_data.query_data_->column_vectors[0];

    auto get_ele = [](const ColumnVector &column_vector, SizeT idx) -> SparseVecRef<typename DistFunc::DataT, typename DistFunc::IndexT> {
        const auto *ele = reinterpret_cast<const SparseT *>(column_vector.data()) + idx;
        const auto &[nnz, chunk_id, chunk_offset] = *ele;
        if (nnz == 0) {
            return SparseVecRef<typename DistFunc::DataT, typename DistFunc::IndexT>(0, nullptr, nullptr);
        }
        const char *sparse_ptr = column_vector.buffer_->fix_heap_mgr_->GetRawPtrFromChunk(chunk_id, chunk_offset);
        const auto *indices = reinterpret_cast<const typename DistFunc::IndexT *>(sparse_ptr);
        const auto *data = reinterpret_cast<const typename DistFunc::DataT *>(sparse_ptr + nnz * sizeof(typename DistFunc::IndexT));
        return SparseVecRef<typename DistFunc::DataT, typename DistFunc::IndexT>(nnz, indices, data);
    };

    auto task_id = block_ids_idx;
    while (task_id < block_ids.size()) {
        block_ids_idx++;
        LOG_DEBUG(fmt::format("MatchSparseScan: block {}", task_id));
        const auto [segment_id, block_id] = block_ids[task_id];

        const BlockIndex *block_index = base_table_ref_->block_index_.get();
        BlockOffset row_cnt = block_index->GetBlockOffset(segment_id, block_id);
        const BlockEntry *block_entry = block_index->GetBlockEntry(segment_id, block_id);
        LOG_DEBUG(fmt::format("MatchSparseScan: segment_id: {}, block_id: {}", segment_id, block_id));

        Bitmask bitmask;
        if (!this->CalculateFilterBitmask(segment_id, block_id, row_cnt, bitmask)) {
            break;
        }
        block_entry->SetDeleteBitmask(begin_ts, bitmask);

        auto *block_column_entry = block_entry->GetColumnBlockEntry(search_column_id_);
        ColumnVector column_vector = block_column_entry->GetConstColumnVector(buffer_mgr);

        for (SizeT query_id = 0; query_id < query_n; ++query_id) {
            auto query_sparse = get_ele(query_vector, query_id);
            for (BlockOffset i = 0; i < row_cnt; ++i) {
                if (!bitmask.IsTrue(i)) {
                    continue;
                }

                auto sparse = get_ele(column_vector, i);

                ResultType d = dist_func->Calculate(query_sparse, sparse);
                RowID row_id(segment_id, block_id * DEFAULT_BLOCK_CAPACITY + i);

                merge_heap->Search(query_id, &d, &row_id, 1);
            }
        }
        break;
    }
    task_id = segment_ids_idx;
    while (task_id < segment_ids.size()) {
        segment_ids_idx++;
        LOG_DEBUG(fmt::format("MatchSparseScan: segment {}", task_id));
        SegmentID segment_id = segment_ids[task_id];

        const IndexIndex *index_index = base_table_ref_->index_index_.get();
        const BlockIndex *block_index = base_table_ref_->block_index_.get();
        IndexSnapshot *index_snapshot = index_index->index_snapshots_vec_[0];
        auto iter = index_snapshot->segment_index_entries_.find(segment_id);
        if (iter == index_snapshot->segment_index_entries_.end()) {
            UnrecoverableError(fmt::format("Cannot find segment index with id: {}", segment_id));
        }
        SegmentIndexEntry *segment_index_entry = iter->second;
        const auto &index_base = segment_index_entry->table_index_entry()->index_base();
        if (index_base->index_type_ != IndexType::kBMP) {
            UnrecoverableError(fmt::format("IndexType: {} is not supported.", (i8)index_base->index_type_));
        }

        auto it = common_query_filter_->filter_result_.find(segment_id);
        if (it == common_query_filter_->filter_result_.end()) {
            break;
        }
        SizeT segment_row_count = 0;
        SegmentEntry *segment_entry = nullptr;
        {
            auto segment_it = block_index->segment_block_index_.find(segment_id);
            if (segment_it == block_index->segment_block_index_.end()) {
                UnrecoverableError(fmt::format("Cannot find segment with id: {}", segment_id));
            }
            segment_entry = segment_it->second.segment_entry_;
            segment_row_count = segment_it->second.segment_offset_;
        }
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

        auto bmp_search = [&](AbstractBMP index, SizeT query_id, bool with_lock, const auto &filter) {
            auto query = get_ele(query_vector, query_id);
            std::visit(
                [&](auto &&index) {
                    using T = std::decay_t<decltype(index)>;
                    if constexpr (std::is_same_v<T, std::nullptr_t>) {
                        UnrecoverableError("Invalid index type.");
                    } else {
                        using IndexT = std::decay_t<decltype(*index)>;
                        if constexpr (std::is_same_v<typename IndexT::DataT, typename DistFunc::DataT> &&
                                      std::is_same_v<typename IndexT::IdxT, typename DistFunc::IndexT>) {
                            BmpSearchOptions options = BMPUtil::ParseBmpSearchOptions(match_sparse_expr_->opt_params_);
                            options.use_lock_ = with_lock;
                            auto [doc_ids, scores] = index->SearchKnn(query, topn, options, filter);
                            SizeT res_n = doc_ids.size();
                            for (SizeT i = 0; i < res_n; ++i) {
                                RowID row_id(segment_id, doc_ids[i]);
                                ResultType d = scores[i];
                                merge_heap->Search(query_id, &d, &row_id, 1);
                            }
                        } else {
                            UnrecoverableError("Invalid index type.");
                        }
                    }
                },
                index);
        };

        auto bmp_scan = [&](const auto &filter) {
            const auto [chunk_index_entries, memory_index_entry] = segment_index_entry->GetBMPIndexSnapshot();
            for (SizeT query_id = 0; query_id < query_n; ++query_id) {
                for (auto chunk_index_entry : chunk_index_entries) {
                    BufferHandle buffer_handle = chunk_index_entry->GetIndex();
                    const auto *bmp_index = reinterpret_cast<const AbstractBMP *>(buffer_handle.GetData());
                    bmp_search(*bmp_index, query_id, false, filter);
                }
                if (memory_index_entry.get() != nullptr) {
                    bmp_search(memory_index_entry->get(), query_id, true, filter);
                }
            }
        };

        if (use_bitmask) {
            if (segment_entry->CheckAnyDelete(begin_ts)) {
                DeleteWithBitmaskFilter filter(bitmask, segment_entry, begin_ts);
                bmp_scan(filter);
            } else {
                BitmaskFilter<SegmentOffset> filter(bitmask);
                bmp_scan(filter);
            }
        } else {
            SegmentOffset max_segment_offset = block_index->GetSegmentOffset(segment_id);
            if (segment_entry->CheckAnyDelete(begin_ts)) {
                DeleteFilter filter(segment_entry, begin_ts, max_segment_offset);
                bmp_scan(filter);
            } else {
                bmp_scan(nullptr);
            }
        }

        break;
    }
    if (block_ids_idx == block_ids.size() && segment_ids_idx == segment_ids.size()) {
        LOG_DEBUG(fmt::format("MatchSparseScan: {} task finished", block_ids_idx));
        merge_heap->End();
        i64 result_n = std::min(topn, (SizeT)merge_heap->total_count());

        Vector<char *> result_dists_list;
        Vector<RowID *> row_ids_list;
        for (SizeT query_id = 0; query_id < query_n; ++query_id) {
            result_dists_list.push_back(reinterpret_cast<char *>(merge_heap->GetDistancesByIdx(query_id)));
            row_ids_list.push_back(merge_heap->GetIDsByIdx(query_id));
        }

        this->SetOutput(result_dists_list, row_ids_list, sizeof(ResultType), result_n, query_context, match_sparse_scan_state);

        match_sparse_scan_state->SetComplete();
    }
}

} // namespace infinity
