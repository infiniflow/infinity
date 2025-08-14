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

// #include <cstring>
// #include <string>

#ifndef PARESER_USE_STD_MODULE
#define PARESER_USE_STD_MODULE 1
import std;
import std.compat;
#endif

namespace infinity {

struct UuidType {
public:
    static constexpr int64_t LENGTH = 16;

public:
    inline UuidType() = default;

    explicit UuidType(const char *input) { Set(input); }

    UuidType(const UuidType &other);

    UuidType(UuidType &&other) noexcept;

    UuidType &operator=(const UuidType &other);

    UuidType &operator=(UuidType &&other) noexcept;

    bool operator==(const UuidType &other) const;

    inline bool operator!=(const UuidType &other) const { return !operator==(other); }

    inline void Set(const char *input) { memcpy(body, input, UuidType::LENGTH); }

    [[nodiscard]] inline std::string ToString() const { return std::string(body, UuidType::LENGTH); }

    inline void Reset() { memset(body, 0, UuidType::LENGTH); }

public:
    char body[UuidType::LENGTH];
};

} // namespace infinity
