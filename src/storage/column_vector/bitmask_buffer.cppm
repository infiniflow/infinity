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

import stl;

export module deprecated_bitmask_buffer;

namespace infinity {

export struct BitmaskBuffer {
public:
    static SharedPtr<BitmaskBuffer> Make(SizeT count);

    static SharedPtr<BitmaskBuffer> Make(const u64 *input_ptr, SizeT count);

    static constexpr const u64 UNIT_BITS = 64; // i64 bits count
    static constexpr const u64 BYTE_BITS = 8;  // one byte bit count
    static constexpr const u64 UNIT_MAX = std::numeric_limits<u64>::max();
    static constexpr const u64 UNIT_MIN = 0;

public:
    BitmaskBuffer();

    ~BitmaskBuffer();

    void Initialize(SizeT count);

    void Initialize(const u64 *ptr, SizeT count);

    static inline SizeT UnitCount(SizeT capacity) { return (capacity + (UNIT_BITS - 1)) / UNIT_BITS; }

public:
    UniquePtr<u64[]> data_ptr_{nullptr};

    SizeT count_{0};
};

} // namespace infinity
