//
// Created by JinHai on 2022/10/28.
//

#pragma once

#include "common/types/internal_types.h"

namespace infinity {

struct HugeInt {
public:
    HugeInt() = default;

    explicit HugeInt(i64 upper, i64 lower) :
        upper(upper), lower(lower) {}

    inline bool
    operator==(const HugeInt& other) const {
        if(this == &other) return true;
        return other.upper == upper && other.lower == lower;
    }

    inline bool
    operator!=(const HugeInt& other) const {
        return !(operator==(other));
    }

    inline bool
    operator>(const HugeInt& other) const {
        if(this->upper > other.upper) return true;
        if(this->upper == other.upper && this->lower > other.lower) return true;
        return false;
    }

    inline bool
    operator<(const HugeInt& other) const {
        if(this->upper < other.upper) return true;
        if(this->upper == other.upper && this->lower < other.lower) return true;
        return false;
    }

    inline bool
    operator<=(const HugeInt& other) const {
        return !(operator>(other));
    }

    inline bool
    operator>=(const HugeInt& other) const {
        return !(operator<(other));
    }

    i64 upper{};
    i64 lower{};

public:
    [[nodiscard]] inline String
    ToString() const;

    inline void
    Reset() {
        upper = 0;
        lower = 0;
    }
};

String
HugeInt::ToString() const {
    TypeError("Not implemented");
}

}


