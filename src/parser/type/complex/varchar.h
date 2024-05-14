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

#pragma once

#include <cstdint>
#include <string>

namespace infinity {

class FixHeapManager;

// varchar related constants
constexpr uint64_t VARCHAR_PREFIX_LENGTH = 5;
constexpr uint64_t VARCHAR_INLINE_LENGTH = 13;
constexpr uint64_t VARCHAR_LENGTH_LIMIT = 8 * 1024 * 1024; // 23 bit or 8MB

#pragma pack(1)

struct InlineVarchar {
    char data_[VARCHAR_INLINE_LENGTH]{};
};

struct VectorVarchar {
    char prefix_[VARCHAR_PREFIX_LENGTH]{};
    uint32_t chunk_id_{0};
    uint32_t chunk_offset_{0};
};

struct Varchar {
    explicit Varchar(const uint64_t length = 0) : length_(length) { this->short_ = {}; }

    bool operator==(const Varchar &other) const = delete;

    bool operator>=(const Varchar &other) const = delete;

    bool operator>(const Varchar &other) const = delete;

    bool operator<=(const Varchar &other) const = delete;

    bool operator<(const Varchar &other) const = delete;

    [[nodiscard]] inline bool IsInlined() const { return length_ <= VARCHAR_INLINE_LENGTH; }

    uint64_t is_value_ : 1 = 0; // deprecated, keep it for compatibility
    uint64_t length_ : 23 = 0;
    union {
        InlineVarchar short_;
        VectorVarchar vector_;
    };

    // only for unit test
    [[nodiscard]] std::string ToString() const {
        if (IsInlined()) {
            return std::string{short_.data_, static_cast<size_t>(length_)};
        }
        return std::string{vector_.prefix_, static_cast<size_t>(VARCHAR_PREFIX_LENGTH)};
    }
};

#pragma pack()

} // namespace infinity
