//
// Created by JinHai on 2022/10/28.
//

#pragma once

#include "parser_assert.h"

namespace infinity {

struct HugeInt {
public:
    HugeInt() = default;

    explicit HugeInt(int64_t upper, int64_t lower) : upper(upper), lower(lower) {}

    inline bool operator==(const HugeInt &other) const {
        if (this == &other)
            return true;
        return other.upper == upper && other.lower == lower;
    }

    inline bool operator!=(const HugeInt &other) const { return !(operator==(other)); }

    inline bool operator>(const HugeInt &other) const {
        if (this->upper > other.upper)
            return true;
        if (this->upper == other.upper && this->lower > other.lower)
            return true;
        return false;
    }

    inline bool operator<(const HugeInt &other) const {
        if (this->upper < other.upper)
            return true;
        if (this->upper == other.upper && this->lower < other.lower)
            return true;
        return false;
    }

    inline bool operator<=(const HugeInt &other) const { return !(operator>(other)); }

    inline bool operator>=(const HugeInt &other) const { return !(operator<(other)); }

    int64_t upper{};
    int64_t lower{};

public:
    [[nodiscard]] inline std::string ToString() const;

    inline void Reset() {
        upper = 0;
        lower = 0;
    }
};

std::string HugeInt::ToString() const { ParserError("ToString() isn't implemented"); }

} // namespace infinity
