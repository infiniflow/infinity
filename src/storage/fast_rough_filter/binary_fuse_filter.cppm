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
import infinity_exception;
import file_system;
import third_party;

namespace infinity {

export class BinaryFuse {
private:
    binary_fuse8_t filter = {.Fingerprints = nullptr};

public:
    BinaryFuse() = default;

    BinaryFuse(const BinaryFuse &o) = delete;

    BinaryFuse(BinaryFuse &&o) noexcept : filter(o.filter) {
        o.filter.Fingerprints = nullptr; // we take ownership for the data
    }

    ~BinaryFuse() { binary_fuse8_free(&filter); }

    bool Allocate(const size_t size) {
        // check old data
        if (filter.Fingerprints) {
            UnrecoverableError("Already have data, cannot allocate.");
        }
        return binary_fuse8_allocate(size, &filter);
    }

    // may change input data if input data is not array of unique members
    // return true: success
    // return false: run out of memory
    bool AddAll(uint64_t *data, const size_t cnt) {
        // check data
        if (!filter.Fingerprints) {
            UnrecoverableError("Need to allocate first.");
        }
        return binary_fuse8_populate(data, cnt, &filter);
    }

    inline bool Contain(uint64_t &item) const { return binary_fuse8_contain(item, &filter); }

    [[nodiscard]] size_t SaveBytes() const {
        return sizeof(filter.Seed) + sizeof(filter.SegmentLength) + sizeof(filter.SegmentCount) + sizeof(filter.SegmentCountLength) +
               sizeof(filter.ArrayLength) + filter.ArrayLength * sizeof(uint8_t);
    }

    void SaveToOStringStream(OStringStream &os) const {
        os.write(reinterpret_cast<const char *>(&filter.Seed), sizeof(filter.Seed));
        os.write(reinterpret_cast<const char *>(&filter.SegmentLength), sizeof(filter.SegmentLength));
        os.write(reinterpret_cast<const char *>(&filter.SegmentCount), sizeof(filter.SegmentCount));
        os.write(reinterpret_cast<const char *>(&filter.SegmentCountLength), sizeof(filter.SegmentCountLength));
        os.write(reinterpret_cast<const char *>(&filter.ArrayLength), sizeof(filter.ArrayLength));
        os.write(reinterpret_cast<const char *>(filter.Fingerprints), filter.ArrayLength * sizeof(uint8_t));
    }

    bool LoadFromIStringStream(IStringStream &is) {
        is.read(reinterpret_cast<char *>(&filter.Seed), sizeof(filter.Seed));
        is.read(reinterpret_cast<char *>(&filter.SegmentLength), sizeof(filter.SegmentLength));
        filter.SegmentLengthMask = filter.SegmentLength - 1;
        is.read(reinterpret_cast<char *>(&filter.SegmentCount), sizeof(filter.SegmentCount));
        is.read(reinterpret_cast<char *>(&filter.SegmentCountLength), sizeof(filter.SegmentCountLength));
        is.read(reinterpret_cast<char *>(&filter.ArrayLength), sizeof(filter.ArrayLength));
        filter.Fingerprints = (uint8_t *)malloc(filter.ArrayLength * sizeof(uint8_t));
        if (!filter.Fingerprints) {
            UnrecoverableError("Out of memory.");
            return false;
        }
        is.read(reinterpret_cast<char *>(filter.Fingerprints), filter.ArrayLength * sizeof(uint8_t));
        return true;
    }
};

} // namespace infinity