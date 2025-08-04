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
#include <string>

module infinity_core:physical_match_sparse_scan.impl;

import :physical_match_sparse_scan;

import :stl;
import :query_context;
import :operator_state;
import :physical_operator;
import :physical_operator_type;
import :match_sparse_expression;
import :base_table_ref;
import :block_index;
import :column_expression;
import create_index_info;
import data_type;
import :common_query_filter;
import :default_values;
import logical_type;
import :match_sparse_scan_function_data;
import :logger;
import :infinity_exception;
import :third_party;
import :buffer_manager;
import :expression_evaluator;
import :expression_state;
import :base_expression;
import :column_vector;
import :data_block;
import sparse_info;
import internal_types;
import :knn_result_handler;
import :match_sparse_scan_function_data;
import :global_block_id;
import :roaring_bitmap;
import :bmp_index_file_worker;
import :buffer_handle;
import :sparse_util;
import :bmp_util;
import :knn_filter;
import :bmp_handler;
import :status;
import :buffer_obj;

import :table_meeta;
import :table_index_meeta;
import :segment_index_meta;
import :chunk_index_meta;
import :segment_meta;
import :block_meta;
import :table_meeta;
import :new_txn;
import :new_catalog;
import :index_base;
import :column_meta;
import :mem_index;
import :kv_store;
import match_sparse_expr;

namespace infinity {

PhysicalMatchSparseScan::PhysicalMatchSparseScan(u64 id,
                                                 u64 table_index,
                                                 SharedPtr<BaseTableRef> base_table_ref,
                                                 SharedPtr<MatchSparseExpression> match_sparse_expression,
                                                 const SharedPtr<CommonQueryFilter> &common_query_filter,
                                                 SharedPtr<Vector<LoadMeta>> load_metas)
    : PhysicalFilterScanBase(id,
                             PhysicalOperatorType::kMatchSparseScan,
                             nullptr,
                             nullptr,
                             table_index,
                             base_table_ref,
                             common_query_filter,
                             load_metas),
      match_sparse_expr_(std::move(match_sparse_expression)) {}

void PhysicalMatchSparseScan::Init(QueryContext *query_context) { search_column_id_ = match_sparse_expr_->column_expr_->binding().column_idx; }

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
        const auto &index_snapshots = base_table_ref_->index_index_->new_index_snapshots_vec_;
        if (!index_snapshots.empty()) {
            if (index_snapshots.size() != 1) {
                UnrecoverableError("Multiple index snapshots are not supported.");
            }
            ret = index_snapshots[0]->segment_index_metas_.size();
        }
    }
    return ret;
}

void PhysicalMatchSparseScan::PlanWithIndex(QueryContext *query_context) {
    SizeT search_column_id = match_sparse_expr_->column_expr_->binding().column_idx;
    auto &search_column_name = base_table_ref_->table_info_->column_defs_[search_column_id]->name();

    Status status;
    TableMeeta *table_meta = table_meta = base_table_ref_->block_index_->table_meta_.get();

    NewTxn *new_txn = query_context->GetNewTxn();
    KVInstance *kv_instance = new_txn->kv_instance();
    TxnTimeStamp begin_ts = new_txn->BeginTS();
    //    TxnTimeStamp commit_ts = new_txn->CommitTS();

    Vector<String> *index_id_strs_ptr = nullptr;
    Vector<String> *index_names_ptr = nullptr;
    status = table_meta->GetIndexIDs(kv_instance, begin_ts, index_id_strs_ptr, &index_names_ptr);
    if (!status.ok()) {
        RecoverableError(status);
    }

    if (match_sparse_expr_->index_name_.empty()) {
        for (SizeT i = 0; i < index_id_strs_ptr->size(); ++i) {
            const String &index_id_str = (*index_id_strs_ptr)[i];
            const String &index_name = (*index_names_ptr)[i];
            auto table_index_meta = MakeShared<TableIndexMeeta>(index_id_str, *table_meta);

            SharedPtr<IndexBase> index_base;
            std::tie(index_base, status) = table_index_meta->GetIndexBase(kv_instance);
            if (!status.ok()) {
                RecoverableError(status);
            }
            if (index_base->column_name() != search_column_name) {
                continue;
            }
            if (index_base->index_type_ != IndexType::kBMP) {
                LOG_TRACE(fmt::format("KnnScan: PlanWithIndex(): Skipping non-knn index."));
                continue;
            }
            if (base_table_ref_->index_index_.get() == nullptr) {
                base_table_ref_->index_index_ = MakeShared<IndexIndex>();
            }
            IndexIndex *index_index = base_table_ref_->index_index_.get();
            auto index_snapshot = index_index->Insert(kv_instance, index_name, table_index_meta);
            break;
        }
    } else {
        auto iter = std::find(index_names_ptr->begin(), index_names_ptr->end(), match_sparse_expr_->index_name_);
        if (iter == index_names_ptr->end()) {
            status = Status::IndexNotExist(match_sparse_expr_->index_name_);
            RecoverableError(std::move(status));
        }
        const String &index_id_str = (*index_id_strs_ptr)[iter - index_names_ptr->begin()];
        const String &index_name = match_sparse_expr_->index_name_;
        auto table_index_meta = MakeShared<TableIndexMeeta>(index_id_str, *table_meta);

        SharedPtr<IndexBase> index_base;
        std::tie(index_base, status) = table_index_meta->GetIndexBase(kv_instance);
        if (!status.ok()) {
            RecoverableError(status);
        }
        if (index_base->column_name() != search_column_name) {
            // knn_column_id isn't in this table index
            LOG_ERROR(fmt::format("Column {} not found", index_base->column_name()));
            Status error_status = Status::ColumnNotExist(index_base->column_name());
            RecoverableError(std::move(error_status));
        }
        // check index type
        if (auto index_type = index_base->index_type_; index_type != IndexType::kBMP) {
            LOG_ERROR("Invalid index type");
            Status error_status = Status::InvalidIndexType("invalid index");
            RecoverableError(std::move(error_status));
        }
        if (base_table_ref_->index_index_.get() == nullptr) {
            base_table_ref_->index_index_ = MakeShared<IndexIndex>();
        }
        IndexIndex *index_index = base_table_ref_->index_index_.get();
        auto index_snapshot = index_index->Insert(kv_instance, index_name, table_index_meta);
    }
    return;
}

Vector<SharedPtr<Vector<SegmentID>>>
PhysicalMatchSparseScan::PlanWithIndex(Vector<SharedPtr<Vector<GlobalBlockID>>> &block_groups, i64 parallel_count, QueryContext *query_context) {
    Vector<SharedPtr<Vector<SegmentID>>> segment_groups(parallel_count);
    for (i64 i = 0; i < parallel_count; ++i) {
        segment_groups[i] = MakeShared<Vector<SegmentID>>();
    }
    if ((i64)block_groups.size() != parallel_count) {
        UnrecoverableError("block_groups.size() != parallel_count");
    }
    IndexIndex *index_index = base_table_ref_->index_index_.get();
    if (index_index != nullptr && !index_index->new_index_snapshots_.empty()) {
        block_groups.assign(parallel_count, MakeShared<Vector<GlobalBlockID>>());
        SizeT group_idx = 0;
        for (const auto &[idx_name, index_snapshot] : index_index->new_index_snapshots_) {
            for (const auto &[segment_id, segment_index_meta] : index_snapshot->segment_index_metas_) {
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
    Status status;
    NewTxn *new_txn = query_context->GetNewTxn();
    TxnTimeStamp begin_ts = new_txn->BeginTS();
    TxnTimeStamp commit_ts = new_txn->CommitTS();
    KVInstance *kv_instance = new_txn->kv_instance();
    if (!common_query_filter_->TryFinishBuild()) {
        // not ready, abort and wait for next time
        return;
    }

    SizeT query_n = match_sparse_expr_->query_n_;
    SizeT topn = match_sparse_expr_->topn_;
    MatchSparseScanFunctionData &function_data = match_sparse_scan_state->match_sparse_scan_function_data_;

    if (merge_heap == nullptr) {
        const auto knn_threshold = GetKnnThreshold(match_sparse_expr_->opt_params_);
        auto merge_knn_ptr = MakeUnique<MergeHeap>(query_n, topn, knn_threshold);
        merge_heap = merge_knn_ptr.get();
        merge_knn_ptr->Begin();
        function_data.merge_knn_base_ = std::move(merge_knn_ptr);

        auto dist_func_ptr = MakeUnique<DistFunc>(match_sparse_expr_->metric_type_);
        dist_func = dist_func_ptr.get();
        function_data.sparse_distance_ = std::move(dist_func_ptr);
    }

    const Vector<GlobalBlockID> &block_ids = *function_data.global_block_ids_;
    const Vector<SegmentID> &segment_ids = *function_data.segment_ids_;
    auto &block_ids_idx = function_data.current_block_ids_idx_;
    auto &segment_ids_idx = function_data.current_segment_ids_idx_;

    const ColumnVector &query_vector = *function_data.query_data_->column_vectors[0];

    auto get_ele = [](const ColumnVector &column_vector, SizeT idx) -> SparseVecRef<typename DistFunc::DataT, typename DistFunc::IndexT> {
        const auto *ele = reinterpret_cast<const SparseT *>(column_vector.data()) + idx;
        const auto &[nnz, file_offset] = *ele;
        return column_vector.buffer_->template GetSparse<typename DistFunc::DataT, typename DistFunc::IndexT>(file_offset, nnz);
    };

    auto task_id = block_ids_idx;
    while (task_id < block_ids.size()) {
        block_ids_idx++;
        LOG_DEBUG(fmt::format("MatchSparseScan: block {}", task_id));
        const auto [segment_id, block_id] = block_ids[task_id];

        const BlockIndex *block_index = base_table_ref_->block_index_.get();
        BlockOffset row_cnt = 0;
        ColumnVector column_vector;
        Bitmask bitmask;

        BlockMeta *block_meta = block_index->GetBlockMeta(segment_id, block_id);
        LOG_DEBUG(fmt::format("MatchSparseScan: segment_id: {}, block_id: {}", segment_id, block_id));

        std::tie(row_cnt, status) = block_meta->GetRowCnt1(kv_instance, begin_ts, commit_ts);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        if (!this->CalculateFilterBitmask(segment_id, block_id, row_cnt, bitmask)) {
            break;
        }
        status = NewCatalog::SetBlockDeleteBitmask(*block_meta, begin_ts, commit_ts, bitmask);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }

        ColumnMeta column_meta(search_column_id_, *block_meta);
        status = NewCatalog::GetColumnVector(column_meta, kv_instance, begin_ts, commit_ts, row_cnt, ColumnVectorMode::kReadOnly, column_vector);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }

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
        NewIndexSnapshot *index_snapshot = index_index->new_index_snapshots_vec_[0];
        auto iter = index_snapshot->segment_index_metas_.find(segment_id);
        if (iter == index_snapshot->segment_index_metas_.end()) {
            UnrecoverableError(fmt::format("Cannot find segment index with id: {}", segment_id));
        }
        SegmentIndexMeta *segment_index_meta = iter->second.get();

        bool has_some_result = false;
        Bitmask bitmask;
        bool use_bitmask = false;
        if (common_query_filter_->AlwaysTrue()) {
            has_some_result = true;
        } else {
            auto it = common_query_filter_->filter_result_.find(segment_id);
            if (it != common_query_filter_->filter_result_.end()) {
                auto segment_it = block_index->new_segment_block_index_.find(segment_id);
                if (segment_it == block_index->new_segment_block_index_.end()) {
                    UnrecoverableError(fmt::format("Cannot find segment with id: {}", segment_id));
                }
                SizeT segment_row_count = segment_it->second.segment_offset();
                bitmask = it->second;
                if (bitmask.count() != segment_row_count) {
                    UnrecoverableError(fmt::format("Invalid segment row count: {} vs {}", bitmask.count(), segment_row_count));
                }
                has_some_result = true;
                use_bitmask = !bitmask.IsAllTrue();
            }
        }
        if (!has_some_result)
            break;
        auto bmp_search = [&](BMPHandlerPtr bmp_handler, SizeT query_id, bool with_lock, const auto &filter) {
            auto query = get_ele(query_vector, query_id);
            BmpSearchOptions options = BMPUtil::ParseBmpSearchOptions(match_sparse_expr_->opt_params_);
            options.use_lock_ = with_lock;
            bmp_handler->template SearchIndex<ResultType, DistFunc>(query, topn, options, filter, query_id, segment_id, merge_heap);
        };
        auto bmp_scan = [&](const auto &filter) {
            auto [chunk_ids_ptr, status] = segment_index_meta->GetChunkIDs1(kv_instance);
            if (!status.ok()) {
                UnrecoverableError(status.message());
            }
            for (ChunkID chunk_id : *chunk_ids_ptr) {
                ChunkIndexMeta chunk_index_meta(chunk_id, *segment_index_meta);
                BufferObj *index_buffer = nullptr;
                status = chunk_index_meta.GetIndexBuffer(kv_instance, index_buffer);
                if (!status.ok()) {
                    UnrecoverableError(status.message());
                }
                BufferHandle index_handle = index_buffer->Load();
                const BMPHandlerPtr *bmp_handler = reinterpret_cast<const BMPHandlerPtr *>(index_handle.GetData());
                bmp_search(*bmp_handler, 0, false, filter);
            }
            SharedPtr<MemIndex> mem_index = segment_index_meta->GetMemIndex();
            if (mem_index) {
                SharedPtr<BMPIndexInMem> bmp_index = mem_index->GetBMPIndex();
                if (bmp_index) {
                    bmp_search(bmp_index->get(), 0, true, filter);
                }
            }
        };

        if (use_bitmask) {
            BitmaskFilter<SegmentOffset> filter(bitmask);
            bmp_scan(filter);
        } else {
            bmp_scan(nullptr);
        }

        break;
    }
    if (block_ids_idx == block_ids.size() && segment_ids_idx == segment_ids.size()) {
        LOG_DEBUG(fmt::format("MatchSparseScan: {} task finished", block_ids_idx));
        merge_heap->End();
        i64 result_n = merge_heap->GetSize();

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
