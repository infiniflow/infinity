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

#include "datetime_type.h"
#include "parser_assert.h"
#include <string>

namespace infinity {
struct TimestampType : public DateTimeType {
    TimestampType() = default;
    explicit constexpr TimestampType(int32_t date_value, int32_t time_value) : DateTimeType(date_value, time_value) {};
    explicit constexpr TimestampType(int64_t epoch_time) : DateTimeType(epoch_time) {};
};
/*
struct TimestampType {
    TimestampType() = default;

    explicit TimestampType(int32_t date_value, int32_t time_value) : date(date_value), time(time_value) {}

    inline void Reset() {
        date = 0;
        time = 0;
    }

    [[nodiscard]] inline std::string ToString() const {
        // Convert timestamp to readable format
        // This is a simplified implementation
        return std::to_string(date) + " " + std::to_string(time);
    }

    int32_t date{};
    int32_t time{};
};
*/
} // namespace infinity
