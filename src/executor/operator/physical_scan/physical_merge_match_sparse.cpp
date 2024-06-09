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

    switch (match_sparse_expr_->metric_type_) {
        case SparseMetricType::kInnerProduct: {
            ExecuteInner<CompareMin>(query_context, merge_match_sparse_state);
            break;
        }
        default: {
            UnrecoverableError("Not implemented yet");
        }
    }

    return true;
}

template <template <typename, typename> typename C>
void PhysicalMergeMatchSparse::ExecuteInner(QueryContext *query_context, MergeMatchSparseOperatorState *operator_state) {
    DataType result_type = match_sparse_expr_->Type();
    switch (result_type.type()) {
        case kTinyInt: {
            ExecuteInner<i8, C>(query_context, operator_state);
            break;
        }
        case kSmallInt: {
            ExecuteInner<i16, C>(query_context, operator_state);
            break;
        }
        case kInteger: {
            ExecuteInner<i32, C>(query_context, operator_state);
            break;
        }
        case kBigInt: {
            ExecuteInner<i64, C>(query_context, operator_state);
            break;
        }
        case kFloat: {
            ExecuteInner<float, C>(query_context, operator_state);
            break;
        }
        case kDouble: {
            ExecuteInner<double, C>(query_context, operator_state);
            break;
        }
        default: {
            UnrecoverableError("Invalid result type.");
        }
    }
}

template <typename ResultType, template <typename, typename> typename C>
void PhysicalMergeMatchSparse::ExecuteInner(QueryContext *query_context, MergeMatchSparseOperatorState *operator_state) {
    SizeT query_n = match_sparse_expr_->query_n_;
    SizeT topn = match_sparse_expr_->topn_;
    MergeSparseFunctionData &match_sparse_data = operator_state->merge_sparse_function_data_;

    using MergeHeap = MergeKnn<ResultType, C>;
    if (match_sparse_data.merge_knn_base_.get() == nullptr) {
        auto merge_knn = MakeUnique<MergeHeap>(query_n, topn);
        merge_knn->Begin();
        match_sparse_data.merge_knn_base_ = std::move(merge_knn);
    }

    auto &input_data = *operator_state->input_data_block_;
    if (!input_data.Finalized()) {
        UnrecoverableError("Input data block is not finalized");
    }
    int column_n = input_data.column_count() - 2;
    if (column_n < 0) {
        UnrecoverableError(fmt::format("Invalid column count, {}", column_n));
    }
    auto *merge_knn = static_cast<MergeHeap *>(match_sparse_data.merge_knn_base_.get());
    auto &dist_column = *input_data.column_vectors[column_n];
    auto &row_id_column = *input_data.column_vectors[column_n + 1];
    auto dists = reinterpret_cast<ResultType *>(dist_column.data());
    auto row_ids = reinterpret_cast<RowID *>(row_id_column.data());
    SizeT row_n = input_data.row_count();
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
        this->SetOutput(result_dists_list, row_ids_list, sizeof(ResultType), result_n, query_context, operator_state);
        operator_state->SetComplete();
    }
}

} // namespace infinity
