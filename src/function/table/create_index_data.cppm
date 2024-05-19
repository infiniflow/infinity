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

export module create_index_data;

import stl;
import segment_entry;
import block_index;
import third_party;
import infinity_exception;

namespace infinity {

export struct CreateIndexSharedData {
    CreateIndexSharedData() = default;

    explicit CreateIndexSharedData(BlockIndex *block_index) { Init(block_index); }

    void Init(BlockIndex *block_index) {
        for (const auto &[segment_id, segment_info] : block_index->segment_block_index_) {
            auto [iter, insert_ok] = create_index_idxes_.emplace(segment_id, 0);
            if (!insert_ok) {
                UnrecoverableError(fmt::format("Duplicate segment id: %u", segment_id));
            }
        }
    }

    HashMap<u32, atomic_u64> create_index_idxes_{};
};

}; // namespace infinity