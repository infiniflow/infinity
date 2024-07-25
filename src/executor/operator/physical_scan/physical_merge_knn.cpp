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

module physical_merge_knn;

import stl;
import txn;
import query_context;

import physical_operator_type;
import operator_state;
import logger;
import status;
import infinity_exception;
import merge_knn_data;
import knn_result_handler;
import merge_knn;
import block_index;
import buffer_manager;
import third_party;
import block_entry;
import default_values;
import data_block;
import knn_expression;
import value;
import column_vector;

namespace infinity {

void PhysicalMergeKnn::Init() {}

bool PhysicalMergeKnn::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto merge_knn_op_state = static_cast<MergeKnnOperatorState *>(operator_state);
    if (merge_knn_op_state->input_complete_) {
        LOG_TRACE("PhysicalMergeKnn::Input is complete");
    }

    auto &merge_knn_data = *merge_knn_op_state->merge_knn_function_data_;
    switch (merge_knn_data.elem_type_) {
        case kElemInvalid: {
            String error_message = "Invalid elem type";
            UnrecoverableError(error_message);
            break;
        }
        case kElemUInt8:
        case kElemInt8:
        case kElemFloat: {
            switch (merge_knn_data.heap_type_) {
                case MergeKnnHeapType::kInvalid: {
                    String error_message = "Invalid heap type";
                    UnrecoverableError(error_message);
                    break;
                }
                case MergeKnnHeapType::kMaxHeap: {
                    ExecuteInner<f32, CompareMax>(query_context, merge_knn_op_state);
                    break;
                }
                case MergeKnnHeapType::kMinHeap: {
                    ExecuteInner<f32, CompareMin>(query_context, merge_knn_op_state);
                    break;
                }
            }
            break;
        }
        default: {
            Status status = Status::NotSupport("Not implemented");
            RecoverableError(status);
        }
    }
    return true;
}

template <typename DataType, template <typename, typename> typename C>
void PhysicalMergeKnn::ExecuteInner(QueryContext *query_context, MergeKnnOperatorState *merge_knn_state) {
    auto &merge_knn_data = *merge_knn_state->merge_knn_function_data_;

    auto &input_data = *merge_knn_state->input_data_block_;
    if (!input_data.Finalized()) {
        String error_message = "Input data block is not finalized";
        UnrecoverableError(error_message);
    }

    auto merge_knn = dynamic_cast<MergeKnn<DataType, C, DataType> *>(merge_knn_data.merge_knn_base_.get());
    if (merge_knn == nullptr) {
        String error_message = "Invalid merge knn data type";
        UnrecoverableError(error_message);
    }
    int column_n = input_data.column_count() - 2;
    if (column_n < 0) {
        String error_message = "Input data block is invalid";
        UnrecoverableError(error_message);
    }

    auto &dist_column = *input_data.column_vectors[column_n];
    auto &row_id_column = *input_data.column_vectors[column_n + 1];

    auto dists = reinterpret_cast<DataType *>(dist_column.data());
    auto row_ids = reinterpret_cast<RowID *>(row_id_column.data());
    SizeT row_n = input_data.row_count();
    merge_knn->Search(dists, row_ids, row_n);

    if (merge_knn_state->input_complete_) {
        merge_knn->End(); // reorder the heap
        i64 result_n = std::min(merge_knn_data.topk_, merge_knn->total_count());

        if (merge_knn_state->data_block_array_.empty()) {
            merge_knn_state->data_block_array_.emplace_back(DataBlock::MakeUniquePtr());
            merge_knn_state->data_block_array_[0]->Init(*GetOutputTypes());
        }

        SizeT query_n = merge_knn_data.query_count_;
        Vector<char *> result_dists_list;
        Vector<RowID *> row_ids_list;
        for (SizeT query_id = 0; query_id < query_n; ++query_id) {
            result_dists_list.emplace_back(reinterpret_cast<char *>(merge_knn->GetDistancesByIdx(query_id)));
            row_ids_list.emplace_back(merge_knn->GetIDsByIdx(query_id));
        }
        this->SetOutput(result_dists_list, row_ids_list, sizeof(DataType), result_n, query_context, merge_knn_state);
        merge_knn_state->SetComplete();
    }
}

} // namespace infinity
