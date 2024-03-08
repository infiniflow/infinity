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

#include "uuid_type.h"

namespace infinity {

UuidType::UuidType(const UuidType &other) { memcpy(this->body, other.body, UuidType::LENGTH); }

UuidType::UuidType(UuidType &&other) noexcept {
    memcpy(this->body, other.body, UuidType::LENGTH);
    other.Reset();
}

UuidType &UuidType::operator=(const UuidType &other) {
    if (this == &other)
        return *this;
    memcpy(this->body, other.body, UuidType::LENGTH);
    return *this;
}

UuidType &UuidType::operator=(UuidType &&other) noexcept {
    if (this == &other)
        return *this;
    memcpy(this->body, other.body, UuidType::LENGTH);
    other.Reset();
    return *this;
}

bool UuidType::operator==(const UuidType &other) const {
    if (this == &other)
        return true;
    // TODO: Need to do benchmark of memcmp and strncmp to decide which one should be used here.
    return (memcmp(this->body, other.body, UuidType::LENGTH) == 0);
}

} // namespace infinity
