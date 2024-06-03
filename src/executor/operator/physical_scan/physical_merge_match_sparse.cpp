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

module physical_merge_match_sparse;

import stl;
import txn;
import query_context;
import physical_operator_type;
import operator_state;
import logger;
import status;
import infinity_exception;
import buffer_manager;
import third_party;
import default_values;
import data_block;
import value;
import column_vector;
import physical_scan_base;
import sparse_info;
import match_sparse_expr;
import internal_types;
import knn_result_handler;
import merge_knn;
import match_sparse_scan_function_data;

namespace infinity {

PhysicalMergeMatchSparse::PhysicalMergeMatchSparse(u64 id,
                                                   UniquePtr<PhysicalOperator> left,
                                                   u64 table_index,
                                                   SharedPtr<BaseTableRef> base_table_ref,
                                                   SharedPtr<MatchSparseExpression> match_sparse_expr,
                                                   SharedPtr<Vector<LoadMeta>> load_metas)
    : PhysicalScanBase(id, PhysicalOperatorType::kMergeMatchSparse, std::move(left), nullptr, base_table_ref, load_metas), table_index_(table_index),
      match_sparse_expr_(std::move(match_sparse_expr)) {}

void PhysicalMergeMatchSparse::Init() { left_->Init(); }

SizeT PhysicalMergeMatchSparse::TaskletCount() {
    String error_message = "Not Expected: TaskletCount of PhysicalMergeMatchSparse?";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return 0;
}

bool PhysicalMergeMatchSparse::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto *merge_match_sparse_state = static_cast<MergeMatchSparseOperatorState *>(operator_state);
    LOG_DEBUG("PhysicalMergeMatchSparse::Execute");

    if (merge_match_sparse_state->data_block_array_.empty()) {
        merge_match_sparse_state->data_block_array_.emplace_back(DataBlock::MakeUniquePtr());
        merge_match_sparse_state->data_block_array_[0]->Init(*GetOutputTypes());
    }
    const auto &column_type = match_sparse_expr_->column_expr_->Type();
    const auto *sparse_info = static_cast<const SparseInfo *>(column_type.type_info().get());

    switch (sparse_info->DataType()) {
        case EmbeddingDataType::kElemFloat: {
            ExecuteInner<float>(query_context, merge_match_sparse_state, match_sparse_expr_->metric_type_);
            break;
        }
        case EmbeddingDataType::kElemDouble: {
            ExecuteInner<double>(query_context, merge_match_sparse_state, match_sparse_expr_->metric_type_);
            break;
        }
        default: {
            UnrecoverableError("Not implemented yet");
        }
    }

    return true;
}

template <typename DataType>
void PhysicalMergeMatchSparse::ExecuteInner(QueryContext *query_context,
                                            MergeMatchSparseOperatorState *operator_state,
                                            const SparseMetricType &metric_type) {
    switch (metric_type) {
        case SparseMetricType::kInnerProduct: {
            ExecuteInner<DataType, CompareMin>(query_context, operator_state);
            break;
        }
        default: {
            UnrecoverableError("Not implemented yet");
        }
    }
}

template <typename DataType, template <typename, typename> typename C>
void PhysicalMergeMatchSparse::ExecuteInner(QueryContext *query_context, MergeMatchSparseOperatorState *operator_state) {
    auto &input_data = *operator_state->input_data_block_;
    if (!input_data.Finalized()) {
        UnrecoverableError("Input data block is not finalized");
    }
    int column_n = input_data.column_count() - 2;
    if (column_n < 0) {
        UnrecoverableError(fmt::format("Invalid column count, {}", column_n));
    }

    auto &dist_column = *input_data.column_vectors[column_n];
    auto &row_id_column = *input_data.column_vectors[column_n + 1];
    auto dists = reinterpret_cast<DataType *>(dist_column.data());
    auto row_ids = reinterpret_cast<RowID *>(row_id_column.data());
    SizeT row_n = input_data.row_count();

    SizeT query_n = match_sparse_expr_->query_n_;
    SizeT topn = match_sparse_expr_->topn_;
    MergeSparseFunctionData &match_sparse_data = operator_state->merge_sparse_function_data_;
    if (match_sparse_data.merge_knn_base_.get() == nullptr) {
        auto merge_knn = MakeUnique<MergeKnn<DataType, C>>(query_n, topn);
        merge_knn->Begin();
        match_sparse_data.merge_knn_base_ = std::move(merge_knn);
    }

    auto *merge_knn = static_cast<MergeKnn<DataType, C> *>(match_sparse_data.merge_knn_base_.get());
    merge_knn->Search(dists, row_ids, row_n);

    if (operator_state->input_complete_) {
        merge_knn->End(); // reorder the heap
        i64 result_n = std::min(topn, (SizeT)merge_knn->total_count());

        Vector<char *> result_dists_list;
        Vector<RowID *> row_ids_list;
        for (SizeT query_id = 0; query_id < query_n; ++query_id) {
            result_dists_list.emplace_back(reinterpret_cast<char *>(merge_knn->GetDistancesByIdx(query_id)));
            row_ids_list.emplace_back(merge_knn->GetIDsByIdx(query_id));
        }
        this->SetOutput(result_dists_list, row_ids_list, sizeof(DataType), result_n, query_context, operator_state);
        operator_state->SetComplete();
    }
}

} // namespace infinity
