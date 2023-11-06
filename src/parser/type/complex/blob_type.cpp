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

#include "blob_type.h"
#include <cstring>

namespace infinity {

BlobType::BlobType(const BlobType &other) { this->Copy(other.ptr, other.size); }

BlobType::BlobType(BlobType &&other) noexcept {
    Move(other.ptr, other.size);
    other.ptr = nullptr;
    other.size = 0;
}

BlobType &BlobType::operator=(const BlobType &other) {
    if (this == &other)
        return *this;
    this->Copy(other.ptr, other.size);
    return *this;
}

BlobType &BlobType::operator=(BlobType &&other) noexcept {
    if (this == &other)
        return *this;
    this->Move(other.ptr, other.size);
    other.ptr = nullptr;
    other.size = 0;
    return *this;
}

bool BlobType::operator==(const BlobType &other) const {
    if (this == &other)
        return true;
    if (this->size != other.size)
        return false;
    // TODO: Need to do benchmark of memcmp and strncmp to decide which one should be used here.
    return memcmp(this->ptr, other.ptr, other.size) == 0;
}

void BlobType::Copy(char *blob_ptr, uint64_t blob_size) {
    // If input blob size is same as current blob size, don't need to allocate memory again
    if (blob_size != this->size) {
        Reset();

        this->ptr = new char[blob_size]{0};
    }

    memcpy(this->ptr, blob_ptr, blob_size);
    this->size = blob_size;
}

void BlobType::Move(char *blob_ptr, uint64_t blob_size) {
    Reset();
    this->ptr = blob_ptr;
    this->size = blob_size;
}

} // namespace infinity
