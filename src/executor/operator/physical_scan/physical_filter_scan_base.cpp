
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

#include <vector>

module physical_filter_scan_base;

import stl;
import common_query_filter;
import base_table_ref;
import bitmask;
import default_values;

namespace infinity {

bool PhysicalFilterScanBase::CalculateFilterBitmask(SegmentID segment_id, BlockID block_id, BlockOffset row_count, Bitmask &bitmask) const {
    auto it_filter = common_query_filter_->filter_result_.find(segment_id);
    if (it_filter == common_query_filter_->filter_result_.end()) {
        return false;
    }
    // not skipped after common_query_filter
    const std::variant<Vector<u32>, Bitmask> &filter_result = it_filter->second;
    bitmask.Initialize(std::bit_ceil(row_count));
    const u32 block_start_offset = block_id * DEFAULT_BLOCK_CAPACITY;
    const u32 block_end_offset = block_start_offset + row_count;
    if (std::holds_alternative<Vector<u32>>(filter_result)) {
        const Vector<u32> &filter_result_vector = std::get<Vector<u32>>(filter_result);
        const auto it1 = std::lower_bound(filter_result_vector.begin(), filter_result_vector.end(), block_start_offset);
        const auto it2 = std::lower_bound(filter_result_vector.begin(), filter_result_vector.end(), block_end_offset);
        bitmask.SetAllFalse();
        for (auto it = it1; it < it2; ++it) {
            bitmask.SetTrue(*it - block_start_offset);
        }
    } else {
        u32 u64_start_offset = block_start_offset / 64;
        u32 u64_end_offset = (block_end_offset - 1) / 64;
        if (const u64 *filter_data = std::get<Bitmask>(filter_result).GetData(); filter_data) {
            bitmask.SetAllFalse();
            u64 *data = bitmask.GetData();
            for (u32 i = u64_start_offset; i <= u64_end_offset; ++i) {
                data[i - u64_start_offset] = filter_data[i];
            }
        }
    }
    return true;
}

} // namespace infinity
