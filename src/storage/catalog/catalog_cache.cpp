// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

module catalog_cache;

import stl;
// import status;
// import meta_info;
// import extra_ddl_info;
// import default_values;
import internal_types;
import logger;
import infinity_exception;
// import buffer_handle;
// import column_def;
// import profiler;
import third_party;
// import storage;
// import catalog_delta_entry;

// uint32_t segment_offset_;
// uint32_t segment_id_;

namespace infinity {

void TableCache::UpdateCapacityPosition(SegmentID segment_id, SegmentOffset segment_offset) {
    if (capacity_position_.segment_id_ > segment_id) {
        UnrecoverableError("Update capacity position to smaller segment id");
    }

    if (capacity_position_.segment_id_ == segment_id and capacity_position_.segment_offset_ > segment_offset) {
        UnrecoverableError("Update capacity position to smaller segment offset");
    }

    if (capacity_position_.segment_id_ != segment_id) {
        if (capacity_position_.segment_id_ != data_position_.segment_id_) {
            diff_segment_ids_.push_back(capacity_position_.segment_id_);
        }
        capacity_position_.segment_id_ = segment_id;
    }

    capacity_position_.segment_offset_ = segment_offset;
    return;
}

void TableCache::UpdateDataPosition(SegmentID segment_id, SegmentOffset segment_offset) {

    if (data_position_.segment_id_ > segment_id) {
        UnrecoverableError("Update data position to smaller segment id");
    }

    if (data_position_.segment_id_ == segment_id and data_position_.segment_offset_ > segment_offset) {
        UnrecoverableError("Update data position to smaller segment offset");
    }

    if (data_position_.segment_id_ != segment_id) {
        if (capacity_position_.segment_id_ != segment_id) {
            // The save the segment_id must be same as the segment_id
            if (diff_segment_ids_.front() != segment_id) {
                LOG_CRITICAL(fmt::format("current data position: {}:{}, proposed data position: {}:{}",
                                         data_position_.segment_id_,
                                         data_position_.segment_offset_,
                                         segment_id,
                                         segment_offset));
                LOG_CRITICAL(fmt::format("current capacity position: {}:{}", capacity_position_.segment_id_, capacity_position_.segment_offset_));
                for (SegmentID segment_id : diff_segment_ids_) {
                    LOG_CRITICAL(fmt::format("diff segment id: {}", segment_id));
                }
                UnrecoverableError("Unmatched segment id");
            }
            diff_segment_ids_.pop_front();
        }
        data_position_.segment_id_ = segment_id;
    }
    data_position_.segment_offset_ = segment_offset;
    return;
}

Deque<SegmentID> TableCache::GetDiffSegments() const { return diff_segment_ids_; }

} // namespace infinity