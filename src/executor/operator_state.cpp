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

bool QueueSourceState::GetData() {
    SharedPtr<FragmentData> fragment_data = nullptr;
    source_queue_.Dequeue(fragment_data);
    if (fragment_data->data_idx_ + 1 == fragment_data->data_count_) {
        auto it = num_tasks_.find(fragment_data->fragment_id_);
        if (it != num_tasks_.end()) {
            u64 &pending_tasks = it->second;
            pending_tasks--;
            if (pending_tasks == 0) {
                num_tasks_.erase(it);
            }
        }
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
