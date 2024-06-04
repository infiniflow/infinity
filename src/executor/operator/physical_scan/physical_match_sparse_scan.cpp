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

namespace infinity {

PhysicalMatchSparseScan::PhysicalMatchSparseScan(u64 id,
                                                 u64 table_index,
                                                 SharedPtr<BaseTableRef> base_table_ref,
                                                 SharedPtr<MatchSparseExpression> match_sparse_expression,
                                                 const SharedPtr<CommonQueryFilter> &common_query_filter,
                                                 SharedPtr<Vector<LoadMeta>> load_metas)
    : PhysicalScanBase(id, PhysicalOperatorType::kMatchSparseScan, nullptr, nullptr, base_table_ref, load_metas), table_index_(table_index),
      match_sparse_expr_(std::move(match_sparse_expression)), common_query_filter_(common_query_filter) {}

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

template <typename DataT, typename IdxType, typename ResultType,  template <typename, typename> typename C>
void PhysicalMatchSparseScan::ExecuteInner(QueryContext *query_context, MatchSparseScanOperatorState *match_sparse_scan_state) {
    MatchSparseScanFunctionData &function_data = match_sparse_scan_state->match_sparse_scan_function_data_;

    using MergeHeap = MergeKnn<ResultType, C>;
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
void PhysicalMatchSparseScan::ExecuteInnerT(DistFunc *dist_func, MergeHeap *merge_heap, QueryContext *query_context, MatchSparseScanOperatorState *match_sparse_scan_state) {
    SizeT query_n = match_sparse_expr_->query_n_;
    SizeT topn = match_sparse_expr_->topn_;
    MatchSparseScanFunctionData &function_data = match_sparse_scan_state->match_sparse_scan_function_data_;

    if (merge_heap == nullptr) {
        auto merge_knn_ptr = MakeUnique<MergeHeap>(query_n, topn);
        merge_heap = merge_knn_ptr.get();
        merge_knn_ptr->Begin();
        function_data.merge_knn_base_ = std::move(merge_knn_ptr);

        auto dist_func_ptr =  MakeUnique<DistFunc>(match_sparse_expr_->metric_type_);
        dist_func = dist_func_ptr.get();
        function_data.sparse_distance_ = std::move(dist_func_ptr);
    }

    BufferManager *buffer_mgr = query_context->storage()->buffer_manager();
    const Vector<GlobalBlockID> &block_ids = *function_data.global_block_ids_;
    const BlockIndex *block_index = function_data.block_index_;
    auto &block_ids_idx = function_data.current_block_ids_idx_;

    if (auto task_id = block_ids_idx; task_id < block_ids.size()) {
        ++block_ids_idx;
        const auto [segment_id, block_id] = block_ids[task_id];

        BlockOffset row_cnt = block_index->GetBlockOffset(segment_id, block_id);
        const BlockEntry *block_entry = block_index->GetBlockEntry(segment_id, block_id);
        LOG_DEBUG(fmt::format("MatchSparseScan: segment_id: {}, block_id: {}", segment_id, block_id));

        auto *block_column_entry = block_entry->GetColumnBlockEntry(search_column_id_);
        ColumnVector column_vector = block_column_entry->GetColumnVector(buffer_mgr);
        const ColumnVector &query_vector = *function_data.query_data_->column_vectors[0];

        const auto *query_data_begin = reinterpret_cast<const SparseT *>(query_vector.data());
        for (SizeT query_id = 0; query_id < query_n; ++query_id) {
            const auto *query_data = query_data_begin + query_id;
            const auto &[query_nnz, query_chunk_id, query_chunk_offset] = *query_data;
            const char *query_sparse_ptr = nullptr;
            if (query_nnz) {
                query_sparse_ptr = query_vector.buffer_->fix_heap_mgr_->GetRawPtrFromChunk(query_chunk_id, query_chunk_offset);
            }

            const auto *data_begin = reinterpret_cast<const SparseT *>(column_vector.data());
            FixHeapManager *heap_mgr = column_vector.buffer_->fix_heap_mgr_.get();
            for (BlockOffset i = 0; i < row_cnt; ++i) {
                const auto *data = data_begin + i;
                const auto &[nnz, chunk_id, chunk_offset] = *data;

                ResultType d = 0;
                const char *sparse_ptr = nullptr;
                if (nnz) {
                    sparse_ptr = heap_mgr->GetRawPtrFromChunk(chunk_id, chunk_offset);
                }
                if (query_sparse_ptr != nullptr && sparse_ptr != nullptr) {
                    d = dist_func->Calculate(query_sparse_ptr, query_nnz, sparse_ptr, nnz);
                }
                RowID row_id(segment_id, block_id * DEFAULT_BLOCK_CAPACITY + i);

                merge_heap->Search(query_id, &d, &row_id, 1);
            }
        }
    }
    if (block_ids_idx >= block_ids.size()) {
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
