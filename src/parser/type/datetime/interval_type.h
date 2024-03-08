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

#include <string>

namespace infinity {

enum TimeUnit : int32_t {
    kYear = 0,
    kMonth,
    kDay,
    kHour,
    kMinute,
    kSecond,
    kInvalidUnit,
};

struct IntervalType {
    IntervalType() = default;

    inline explicit IntervalType(int32_t v) : value(v) {}

    [[nodiscard]] std::string ToString() const;

    inline void Reset() {
        unit = TimeUnit::kInvalidUnit;
        value = 0;
    }

    TimeUnit unit{TimeUnit::kInvalidUnit};
    int32_t value{};
};

} // namespace infinity
