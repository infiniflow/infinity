#pragma once

#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <limits>

namespace infinity {

struct bfloat16_t {
    uint16_t raw;

    constexpr bfloat16_t(uint16_t raw) : raw(raw) {}

    bfloat16_t() = default;

    bfloat16_t(float f) { (*this) = f; }

    bfloat16_t operator+(bfloat16_t h) const { return bfloat16_t(float(*this) + float(h)); }
    bfloat16_t operator-(bfloat16_t h) const { return bfloat16_t(float(*this) - float(h)); }
    bfloat16_t operator*(bfloat16_t h) const { return bfloat16_t(float(*this) * float(h)); }
    bfloat16_t operator/(bfloat16_t h) const { return bfloat16_t(float(*this) / float(h)); }
    bfloat16_t operator-() const { return bfloat16_t(-float(*this)); }

    bfloat16_t &operator+=(bfloat16_t h) { return operator=(*this + h); }
    bfloat16_t &operator-=(bfloat16_t h) { return operator=(*this - h); }
    bfloat16_t &operator*=(bfloat16_t h) { return operator=(*this *h); }
    bfloat16_t &operator/=(bfloat16_t h) { return operator=(*this / h); }

    bool operator==(bfloat16_t h) const { return float(*this) == float(h); }
    bool operator!=(bfloat16_t h) const { return float(*this) != float(h); }
    bool operator<(bfloat16_t h) const { return float(*this) < float(h); }
    bool operator>(bfloat16_t h) const { return float(*this) > float(h); }
    bool operator<=(bfloat16_t h) const { return float(*this) <= float(h); }
    bool operator>=(bfloat16_t h) const { return float(*this) >= float(h); }

    friend std::ostream &operator<<(std::ostream &os, const bfloat16_t &h) {
        os << float(h);
        return os;
    }

    bfloat16_t &operator=(float f) {
        std::memcpy(&raw, reinterpret_cast<char *>(&f) + 2, 2);
        return *this;
    }

    operator float() const {
        float val = 0.f;
        uint16_t temp = raw;
        std::memcpy(reinterpret_cast<char *>(&val) + 2, reinterpret_cast<char *>(&temp), 2);
        return val;
    }

    float f() const { return (float)(*this); }
};

} // namespace infinity
