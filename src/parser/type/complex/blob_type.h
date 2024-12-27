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

#if 0

#pragma once

#include <cstdint>
#include <string>

namespace infinity {

struct BlobType {
public:
    char *ptr{nullptr};
    uint64_t size{0}; // 4GB will be the limitation.

public:
    inline BlobType() = default;

    // The blob_ptr will also be freed by BlobType's destructor.
    inline BlobType(char *blob_ptr, uint64_t blob_size) : ptr(blob_ptr), size(blob_size) {}

    inline ~BlobType() { Reset(); }

    BlobType(const BlobType &other);

    BlobType(BlobType &&other) noexcept;

    BlobType &operator=(const BlobType &other);

    BlobType &operator=(BlobType &&other) noexcept;

    bool operator==(const BlobType &other) const;

    inline bool operator!=(const BlobType &other) const { return !operator==(other); }

public:
    void Copy(char *blob_ptr, uint64_t blob_size);

    void std::move(char *blob_ptr, uint64_t blob_size);

    inline void Reset() {
        if (size != 0) {
            size = 0;

            delete[] ptr;

            ptr = nullptr;
        }
    }

    [[nodiscard]] inline std::string ToString() const { return {this->ptr, this->size}; }
};

} // namespace infinity

#endif
