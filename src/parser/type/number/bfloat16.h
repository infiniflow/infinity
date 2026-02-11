#pragma once

#ifndef PARESER_USE_STD_MODULE
#define PARESER_USE_STD_MODULE 1
import std.compat;
#endif

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

namespace std {
template <>
struct numeric_limits<infinity::bfloat16_t> {
    using T = infinity::bfloat16_t;

    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = false;
    static constexpr bool is_exact = false;
    static constexpr bool has_infinity = true;
    static constexpr bool has_quiet_NaN = true;
    static constexpr bool has_signaling_NaN = true;
    static constexpr std::float_denorm_style has_denorm = std::denorm_present;
    static constexpr bool has_denorm_loss = false;
    static constexpr std::float_round_style round_style = std::round_to_nearest;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = false;
    static constexpr int digits = 8;   // mantissa bits (excluding implicit leading bit)
    static constexpr int digits10 = 2; // decimal digits of precision
    static constexpr int max_digits10 = 4;
    static constexpr int radix = 2;
    static constexpr int min_exponent = -125;
    static constexpr int min_exponent10 = -37;
    static constexpr int max_exponent = 128;
    static constexpr int max_exponent10 = 38;
    static constexpr bool traps = false;
    static constexpr bool tinyness_before = false;

    static constexpr T min() { return T(static_cast<uint16_t>(0x0080u)); }         // smallest positive normalized value (approx 1.17549435e-38)
    static constexpr T max() { return T(static_cast<uint16_t>(0x7F7Fu)); }         // largest finite value (approx 3.38953139e38)
    static constexpr T lowest() { return T(static_cast<uint16_t>(0xFF7Fu)); }      // most negative finite value (approx -3.38953139e38)
    static constexpr T epsilon() { return T(static_cast<uint16_t>(0x3C00u)); }     // difference between 1.0 and next representable value
    static constexpr T round_error() { return T(static_cast<uint16_t>(0x3F00u)); } // 0.5
    static constexpr T infinity() { return T(static_cast<uint16_t>(0x7F80u)); }
    static constexpr T quiet_NaN() { return T(static_cast<uint16_t>(0x7FC0u)); }
    static constexpr T signaling_NaN() { return T(static_cast<uint16_t>(0x7FA0u)); }
    static constexpr T denorm_min() { return T(static_cast<uint16_t>(0x0001u)); } // smallest positive denormalized value
};
} // namespace std
