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

export module meta_state;

import stl;

namespace infinity {

struct SegmentEntry;
struct BlockEntry;
class BlockColumnEntry;
class DataBlock;
class ColumnVector;

export struct MetaColumnVectorState {
    ColumnVector *column_vector_{};
};

export struct MetaLocalDataState {
    DataBlock *data_block_{};
    HashMap<u64, MetaColumnVectorState> column_vector_map_{};
};

export struct MetaBlockColumnState {
    BlockColumnEntry *block_column_{};
};

export struct MetaBlockState {
    BlockEntry *block_entry_{};
    HashMap<u64, MetaBlockColumnState> column_data_map_{};
};

export struct MetaSegmentState {
    SegmentEntry *segment_entry_{};

    HashMap<u16, MetaBlockState> block_map_{};
};

export struct MetaTableState {
    Vector<MetaLocalDataState> local_blocks_{};

    // segment id->meta segment state
    HashMap<u32, MetaSegmentState> segment_map_{};
};

} // namespace infinity
