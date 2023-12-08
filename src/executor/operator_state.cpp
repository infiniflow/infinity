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

import data_block;
import stl;
import physical_operator_type;
import fragment_data;
import infinity_exception;

module operator_state;

namespace infinity {

void QueueSourceState::MarkCompletedTask(u64 fragment_id) {
    auto it = num_tasks_.find(fragment_id);
    if (it != num_tasks_.end()) {
        u64 &pending_tasks = it->second;
        pending_tasks--;
        if (pending_tasks == 0) {
            num_tasks_.erase(it);
        }
    }
}

// A true return value indicate the source is complete
// A false return value indicate there are more data need to read from source.
// True or false doesn't mean the source data is error or not.
bool QueueSourceState::GetData() {
    SharedPtr<FragmentData> fragment_data = nullptr;
    source_queue_.Dequeue(fragment_data);

    if(fragment_data->error_message_.get() != nullptr) {
        if(this->error_message_.get() == nullptr) {
            // Only record the first error of input data.
            this->error_message_ = Move(fragment_data->error_message_);
        }

        // Get an error message from predecessor fragment
        MarkCompletedTask(fragment_data->fragment_id_);
    } else if (fragment_data->data_idx_ + 1 == fragment_data->data_count_) {
        // Get an all data from this
        MarkCompletedTask(fragment_data->fragment_id_);
    } else {
        return false;
    }

    bool completed = num_tasks_.empty();
    OperatorState *next_op_state = this->next_op_state_;
    switch (next_op_state->operator_type_) {
        case PhysicalOperatorType::kMergeKnn: {
            MergeKnnOperatorState *merge_knn_op_state = (MergeKnnOperatorState *)next_op_state;
            merge_knn_op_state->input_data_block_ = Move(fragment_data->data_block_);
            merge_knn_op_state->input_complete_ = completed;
            break;
        }
        case PhysicalOperatorType::kFusion: {
            FusionOperatorState *fusion_op_state = (FusionOperatorState *)next_op_state;
            fusion_op_state->input_data_blocks_[fragment_data->fragment_id_].push_back(Move(fragment_data->data_block_));
            fusion_op_state->input_complete_ = completed;
            break;
        }
        default: {
            Error<ExecutorException>("Not support operator type");
            break;
        }
    }
    return completed;
}

} // namespace infinity
