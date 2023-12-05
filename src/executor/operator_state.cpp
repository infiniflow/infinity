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

module operator_state;

namespace infinity {

bool QueueSourceState::GetData(UniquePtr<DataBlock> &output) {
    SharedPtr<FragmentData> fragment_data = nullptr;
    source_queue_.Dequeue(fragment_data);
//    DataBlock* input_data_block = fragment_data->data_block_.get();
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
    output = Move(fragment_data->data_block_);
    return num_tasks_.empty();
}


} // namespace infinity
