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

#include "interval_type.h"
#include "parser_assert.h"

#ifndef PARESER_USE_THIRD_PARTY_MODULE
#define PARESER_USE_THIRD_PARTY_MODULE 1
import third_party;
#endif

namespace infinity {

std::string IntervalType::ToString() const {
    switch (unit) {
        case kYear: {
            return fmt::format("{} YEAR", value);
        }
        case kMonth: {
            return fmt::format("{} MONTH", value);
        }
        case kDay: {
            return fmt::format("{} DAY", value);
        }
        case kHour: {
            return fmt::format("{} HOUR", value);
        }
        case kMinute: {
            return fmt::format("{} MINUTE", value);
        }
        case kSecond: {
            return fmt::format("{} SECOND", value);
        }
        default: {
            return fmt::format("{} INVALID", value);
        }
    }
}

} // namespace infinity
