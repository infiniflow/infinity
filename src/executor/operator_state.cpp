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

module operator_state;

import data_block;
import stl;
import physical_operator_type;
import fragment_data;
import infinity_exception;
import logger;
import third_party;

namespace infinity {

void QueueSourceState::MarkCompletedTask(u64 fragment_id) {
    auto it = num_tasks_.find(fragment_id);
    if (it != num_tasks_.end()) {
        u64 &pending_tasks = it->second;
        pending_tasks--;
        if (pending_tasks == 0) {
            num_tasks_.erase(it);
        }
    } else {
        String error_message = "Get unexpected data from child fragment";
        UnrecoverableError(error_message);
    }
}

// A true return value indicate the source is complete
// A false return value indicate there are more data need to read from source.
// True or false doesn't mean the source data is error or not.
bool QueueSourceState::GetData() {
    SharedPtr<FragmentDataBase> fragment_data_base = nullptr;
    if (!source_queue_.TryDequeue(fragment_data_base)) {
        String error_message = "This task should not be scheduled if the source queue is empty";
        UnrecoverableError(error_message);
    }

    switch (fragment_data_base->type_) {
        case FragmentDataType::kData: {
            auto *fragment_data = static_cast<FragmentData *>(fragment_data_base.get());
            if (fragment_data->is_last_ &&
                (!fragment_data->data_idx_.has_value() || fragment_data->data_idx_.value() + 1 == fragment_data->data_count_)) {
                // fragment completed
                MarkCompletedTask(fragment_data->fragment_id_);
            }
            break;
        }
        case FragmentDataType::kError: {
            auto *fragment_error = static_cast<FragmentError *>(fragment_data_base.get());
            if (this->status_.ok()) {
                // Only record the first error of input data.
                this->status_ = std::move(fragment_error->status_);
            }
            // Get an error message from predecessor fragment
            MarkCompletedTask(fragment_error->fragment_id_);
            break;
        }
        case FragmentDataType::kNone: {
            auto *fragment_none = static_cast<FragmentNone *>(fragment_data_base.get());
            MarkCompletedTask(fragment_none->fragment_id_);
            break;
        }
        default: {
            String error_message = "Not support fragment data type";
            UnrecoverableError(error_message);
            break;
        }
    }

    bool completed = num_tasks_.empty();
    OperatorState *next_op_state = this->next_op_state_;
    switch (next_op_state->operator_type_) {
        case PhysicalOperatorType::kMergeKnn: {
            auto *fragment_data = static_cast<FragmentData *>(fragment_data_base.get());
            MergeKnnOperatorState *merge_knn_op_state = (MergeKnnOperatorState *)next_op_state;
            merge_knn_op_state->input_data_block_ = std::move(fragment_data->data_block_);
            merge_knn_op_state->input_complete_ = completed;
            break;
        }
        case PhysicalOperatorType::kMergeMatchSparse: {
            auto *fragment_data = static_cast<FragmentData *>(fragment_data_base.get());
            auto *merge_match_sparse_op_state = static_cast<MergeMatchSparseOperatorState *>(next_op_state);
            merge_match_sparse_op_state->input_data_block_ = std::move(fragment_data->data_block_);
            merge_match_sparse_op_state->input_complete_ = completed;
            break;
        }
        case PhysicalOperatorType::kMergeMatchTensor: {
            auto *fragment_data = static_cast<FragmentData *>(fragment_data_base.get());
            MergeMatchTensorOperatorState *merge_match_tensor_op_state = (MergeMatchTensorOperatorState *)next_op_state;
            merge_match_tensor_op_state->input_data_blocks_.push_back(std::move(fragment_data->data_block_));
            merge_match_tensor_op_state->input_complete_ = completed;
            break;
        }
        case PhysicalOperatorType::kFusion: {
            auto *fragment_data = static_cast<FragmentData *>(fragment_data_base.get());
            FusionOperatorState *fusion_op_state = (FusionOperatorState *)next_op_state;
            fusion_op_state->input_data_blocks_[fragment_data->fragment_id_].push_back(std::move(fragment_data->data_block_));
            fusion_op_state->input_complete_ = completed;
            break;
        }
        case PhysicalOperatorType::kMergeLimit: {
            auto *fragment_data = static_cast<FragmentData *>(fragment_data_base.get());
            MergeLimitOperatorState *limit_op_state = (MergeLimitOperatorState *)next_op_state;
            limit_op_state->input_data_blocks_.push_back(std::move(fragment_data->data_block_));
            if (!limit_op_state->input_complete_) {
                limit_op_state->input_complete_ = completed;
            }
            if (limit_op_state->input_complete_) {
                source_queue_.NotAllowEnqueue();
            }
            break;
        }
        case PhysicalOperatorType::kMergeTop: {
            auto *fragment_data = static_cast<FragmentData *>(fragment_data_base.get());
            auto top_op_state = (MergeTopOperatorState *)next_op_state;
            top_op_state->input_data_blocks_.push_back(std::move(fragment_data->data_block_));
            if (!top_op_state->input_complete_) {
                top_op_state->input_complete_ = completed;
            }
            if (top_op_state->input_complete_) {
                source_queue_.NotAllowEnqueue();
            }
            break;
        }
        case PhysicalOperatorType::kMergeAggregate: {
            auto *fragment_data = static_cast<FragmentData *>(fragment_data_base.get());
            MergeAggregateOperatorState *merge_aggregate_op_state = (MergeAggregateOperatorState *)next_op_state;
            // merge_aggregate_op_state->input_data_blocks_.push_back(std::move(fragment_data->data_block_));
            merge_aggregate_op_state->input_data_block_ = std::move(fragment_data->data_block_);

            // {
            //     auto row = merge_aggregate_op_state->input_data_block_->row_count();
            //     if (row == 0) {
            //         // LOG_WARN("FFF");
            //     } else {
            //         auto v = merge_aggregate_op_state->input_data_block_->GetValue(0, 0);
            //         auto ti = v.value_.tiny_int;
            //         auto si = v.value_.small_int;
            //         auto i = v.value_.integer;
            //         LOG_WARN(fmt::format("Merge Agg: task id {}, fragment id {}, completed {}, {} {} {}",
            //                              fragment_data->task_id_,
            //                              fragment_data->fragment_id_,
            //                              completed,
            //                              ti,
            //                              si,
            //                              i));
            //     }
            // }
            merge_aggregate_op_state->input_complete_ = completed;
            break;
        }
        default: {
            String error_message = "Not support operator type";
            UnrecoverableError(error_message);
            break;
        }
    }
    return completed;
}

} // namespace infinity
