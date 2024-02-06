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

module segment_iter;

import stl;
import catalog;

namespace infinity {

BlockEntry *BlockEntryIter::Next() {
    std::shared_lock<std::shared_mutex> lock;
    if (segment_->sealed()) {
        lock = std::shared_lock(segment_->rw_locker_);
    }
    if (cur_block_idx_ >= segment_->block_entries_.size()) {
        return nullptr;
    }
    return segment_->block_entries_[cur_block_idx_++].get();
}

} // namespace infinity