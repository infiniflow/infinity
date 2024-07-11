//  Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

module;
#include "binaryfusefilter.h"
export module binary_fuse_filter;
import stl;
import default_values;
import infinity_exception;
import file_system;
import third_party;
import logger;

namespace infinity {

// two visible states for query:
// 1. finished_build_filter_ is true, filter is ready, have valid build_time and filter
// 2. finished_build_filter_ is false, skip filter check
export class BinaryFuse {
private:
    // in build, lock and check timestamp to ensure that filter will only be built once
    mutable std::mutex mutex_check_task_start_;
    TxnTimeStamp build_time_{UNCOMMIT_TS};

    // in query, use atomic_flag to check if filter is ready
    atomic_flag finished_build_filter_;
    // zero-initialize the filter
    binary_fuse8_t filter = {};

private:
    inline bool HaveFilter() const { return finished_build_filter_.test(std::memory_order_acquire); }

public:
    BinaryFuse() = default;

    BinaryFuse(const BinaryFuse &o) = delete;

    BinaryFuse(BinaryFuse &&o) = delete;

    ~BinaryFuse() { binary_fuse8_free(&filter); }

    void Build(TxnTimeStamp begin_ts, uint64_t *data, const size_t size) {
        {
            std::lock_guard<std::mutex> lock(mutex_check_task_start_);
            if (build_time_ != UNCOMMIT_TS) {
                String error_message = "Already have data, cannot allocate.";
                UnrecoverableError(error_message);
            }
            build_time_ = begin_ts;
        }
        // check old data
        if (filter.Fingerprints) {
            String error_message = "Already have data, cannot allocate.";
            UnrecoverableError(error_message);
        }
        bool alloc_ret = binary_fuse8_allocate(size, &filter);
        if (!alloc_ret) {
            String error_message = "Out of memory.";
            UnrecoverableError(error_message);
        }
        // check data
        if (!filter.Fingerprints) {
            String error_message = "Need to allocate first.";
            UnrecoverableError(error_message);
        }
        bool populate_ret = binary_fuse8_populate(data, size, &filter);
        if (!populate_ret) {
            String error_message = "Failed to populate data.";
            UnrecoverableError(error_message);
        }
        // set finished_build_filter_ to true
        finished_build_filter_.test_and_set(std::memory_order_release);
    }

    inline bool Contain(TxnTimeStamp query_ts, uint64_t &item) const {
        if (!HaveFilter()) {
            return true;
        }
        if (query_ts > build_time_) [[likely]] {
            return binary_fuse8_contain(item, &filter);
        } else {
            return true;
        }
    }

    [[nodiscard]] size_t SaveBytes() const {
        size_t header_size = sizeof(u8); // if finished_build_filter_ is true
        if (HaveFilter()) {
            size_t filter_size = sizeof(filter.Seed) + sizeof(filter.SegmentLength) + sizeof(filter.SegmentCount) +
                                 sizeof(filter.SegmentCountLength) + sizeof(filter.ArrayLength) + filter.ArrayLength * sizeof(uint8_t);
            return header_size + sizeof(build_time_) + filter_size;
        } else {
            return header_size;
        }
    }

    void SaveToOStringStream(OStringStream &os) const {
        u8 finished_build_filter = HaveFilter() ? 1 : 0;
        os.write(reinterpret_cast<const char *>(&finished_build_filter), sizeof(finished_build_filter));
        if (!finished_build_filter) {
            return;
        }
        os.write(reinterpret_cast<const char *>(&build_time_), sizeof(build_time_));
        os.write(reinterpret_cast<const char *>(&filter.Seed), sizeof(filter.Seed));
        os.write(reinterpret_cast<const char *>(&filter.SegmentLength), sizeof(filter.SegmentLength));
        os.write(reinterpret_cast<const char *>(&filter.SegmentCount), sizeof(filter.SegmentCount));
        os.write(reinterpret_cast<const char *>(&filter.SegmentCountLength), sizeof(filter.SegmentCountLength));
        os.write(reinterpret_cast<const char *>(&filter.ArrayLength), sizeof(filter.ArrayLength));
        os.write(reinterpret_cast<const char *>(filter.Fingerprints), filter.ArrayLength * sizeof(uint8_t));
    }

    bool LoadFromIStringStream(IStringStream &is) {
        u8 finished_build_filter = 0;
        is.read(reinterpret_cast<char *>(&finished_build_filter), sizeof(finished_build_filter));
        if (!finished_build_filter) {
            return true;
        }
        is.read(reinterpret_cast<char *>(&build_time_), sizeof(build_time_));
        is.read(reinterpret_cast<char *>(&filter.Seed), sizeof(filter.Seed));
        is.read(reinterpret_cast<char *>(&filter.SegmentLength), sizeof(filter.SegmentLength));
        filter.SegmentLengthMask = filter.SegmentLength - 1;
        is.read(reinterpret_cast<char *>(&filter.SegmentCount), sizeof(filter.SegmentCount));
        is.read(reinterpret_cast<char *>(&filter.SegmentCountLength), sizeof(filter.SegmentCountLength));
        is.read(reinterpret_cast<char *>(&filter.ArrayLength), sizeof(filter.ArrayLength));
        filter.Fingerprints = (uint8_t *)malloc(filter.ArrayLength * sizeof(uint8_t));
        if (!filter.Fingerprints) {
            String error_message = "Out of memory.";
            UnrecoverableError(error_message);
            return false;
        }
        is.read(reinterpret_cast<char *>(filter.Fingerprints), filter.ArrayLength * sizeof(uint8_t));
        finished_build_filter_.test_and_set(std::memory_order_release);
        return true;
    }
};

} // namespace infinity