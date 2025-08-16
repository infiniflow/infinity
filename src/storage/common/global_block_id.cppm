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

export module infinity_core:global_block_id;

import :infinity_type;

import std;

namespace infinity {

export struct GlobalBlockID {
    SegmentID segment_id_;
    BlockID block_id_;

    GlobalBlockID(SegmentID segment_id, BlockID block_id) : segment_id_(segment_id), block_id_(block_id) {}

    bool operator==(const GlobalBlockID &other) const { return segment_id_ == other.segment_id_ && block_id_ == other.block_id_; }
};

export class GlobalBlockIDHash {
public:
    size_t operator()(const GlobalBlockID &global_block_id) const {
        return std::hash<SegmentID>()(global_block_id.segment_id_) ^ std::hash<BlockID>()(global_block_id.block_id_);
    }
};

} // namespace infinity
