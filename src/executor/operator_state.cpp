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

module operator_state;

namespace infinity {

void QueueSourceState::PushData(DataBlock *input_data_block) {
    switch(next_op_state_->operator_type_) {
        case PhysicalOperatorType::kMergeKnn: {
            MergeKnnOperatorState* merge_knn_op_state = (MergeKnnOperatorState*)next_op_state_;
            merge_knn_op_state->input_data_block_ = input_data_block;
            ++merge_knn_op_state->received_data_count_;
            if (merge_knn_op_state->received_data_count_ >= merge_knn_op_state->total_data_count_) {
                merge_knn_op_state->input_complete_ = true;
            }
            break;
        }
        default: {
            break;
        }
    }

}

} // namespace infinity
