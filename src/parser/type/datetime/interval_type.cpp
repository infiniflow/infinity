//
// Created by JinHai on 2022/10/28.
//

#include "interval_type.h"
#include "parser_assert.h"
#include "spdlog/fmt/fmt.h"

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
        }
    }
    ParserError("ToString() isn't implemented");
}

} // namespace infinity
