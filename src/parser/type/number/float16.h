#pragma once

#if defined(__ARM_NEON)
#include <arm_neon.h>
#endif

#if defined(__F16C__)
#include "common/simd/simd_init_h.h"
#include <x86intrin.h>
#endif

#ifndef PARESER_USE_STD_MODULE
#define PARESER_USE_STD_MODULE 1
import std.compat;
#endif

namespace infinity {
namespace detail {
template <typename T, typename U>
T bit_cast(const U &u) {
    T t;
    uint8_t *t_ptr = reinterpret_cast<uint8_t *>(&t);
    const uint8_t *u_ptr = reinterpret_cast<const uint8_t *>(&u);
    for (size_t i = 0; i < sizeof(U); i++)
        t_ptr[i] = u_ptr[i];
    return t;
}
} // namespace detail

#if defined(__ARM_NEON)
template <typename Dest, typename Src>
Dest memcpy_cast(const Src &src) {
    static_assert(sizeof(Dest) == sizeof(Src), "Sizes of types do not match");
    Dest dest;
    const char *srcPtr = reinterpret_cast<const char *>(&src);
    char *destPtr = reinterpret_cast<char *>(&dest);
    for (size_t i = 0; i < sizeof(Dest); i++) {
        destPtr[i] = srcPtr[i];
    }
    return dest;
}
#endif

struct float16_t {
    uint16_t raw;

    constexpr float16_t(uint16_t raw) : raw(raw) {}

    float16_t() = default;

    float16_t(float f) { (*this) = f; }

    float16_t operator+(float16_t h) const { return float16_t(float(*this) + float(h)); }
    float16_t operator-(float16_t h) const { return float16_t(float(*this) - float(h)); }
    float16_t operator*(float16_t h) const { return float16_t(float(*this) * float(h)); }
    float16_t operator/(float16_t h) const { return float16_t(float(*this) / float(h)); }
    float16_t operator-() const { return float16_t(-float(*this)); }

    float16_t &operator+=(float16_t h) { return operator=(*this + h); }
    float16_t &operator-=(float16_t h) { return operator=(*this - h); }
    float16_t &operator*=(float16_t h) { return operator=(*this *h); }
    float16_t &operator/=(float16_t h) { return operator=(*this / h); }

    bool operator==(float16_t h) const { return float(*this) == float(h); }
    bool operator!=(float16_t h) const { return float(*this) != float(h); }
    bool operator<(float16_t h) const { return float(*this) < float(h); }
    bool operator>(float16_t h) const { return float(*this) > float(h); }
    bool operator<=(float16_t h) const { return float(*this) <= float(h); }
    bool operator>=(float16_t h) const { return float(*this) >= float(h); }

    friend std::ostream &operator<<(std::ostream &os, const float16_t &h) {
        os << float(h);
        return os;
    }

    float16_t &operator=(float f) {
#if (defined(__x86_64__) || defined(__i386__))
#if defined __F16C__
        if (IsF16CSupported()) {
            this->raw = _cvtss_sh(f, _MM_FROUND_CUR_DIRECTION);
            // this->raw = (uint16_t)_mm_cvtsi128_si32(_mm_cvtps_ph(_mm_set_ss(f), _MM_FROUND_CUR_DIRECTION));
            return *this;
        }
#endif
        uint32_t i = detail::bit_cast<uint32_t>(f);
        uint32_t s = i >> 31;
        uint32_t e = (i >> 23) & 0xFF;
        uint32_t m = i & 0x7FFFFF;

        uint32_t ss = s;
        uint32_t mm = m >> 13;
        uint32_t r = m & 0x1FFF;
        uint32_t ee = 0;
        int32_t eee = (e - 127) + 15;

        if (e == 0) {
            // Denormal/zero floats all become zero.
            ee = 0;
            mm = 0;
        } else if (e == 0xFF) {
            // Preserve inf/nan.
            ee = 0x1F;
            if (m != 0 && mm == 0)
                mm = 1;
        } else if (eee > 0 && eee < 0x1F) {
            // Normal range. Perform round to even on mantissa.
            ee = eee;
            if (r > (0x1000 - (mm & 1))) {
                // Round up.
                mm++;
                if (mm == 0x400) {
                    // Rounds up to next dyad (or inf).
                    mm = 0;
                    ee++;
                }
            }
        } else if (eee >= 0x1F) {
            // Overflow.
            ee = 0x1F;
            mm = 0;
        } else {
            // Underflow.
            float ff = fabsf(f) + 0.5;
            uint32_t ii = detail::bit_cast<uint32_t>(ff);
            ee = 0;
            mm = ii & 0x7FF;
        }

        this->raw = (ss << 15) | (ee << 10) | mm;
        return *this;
#elif defined __ARM_NEON
        this->raw = memcpy_cast<uint16_t>((__fp16)f);
        return *this;
#else
#error "Unsupported architecture"
#endif
    }

    operator float() const {
#if (defined(__x86_64__) || defined(__i386__))
#if defined __F16C__
        if (IsF16CSupported()) {
            return _cvtsh_ss(raw);
            // return _mm_cvtss_f32(_mm_cvtph_ps(_mm_cvtsi32_si128((int32_t)raw)));
        }
#endif
        uint32_t ss = raw >> 15;
        uint32_t ee = (raw >> 10) & 0x1F;
        uint32_t mm = raw & 0x3FF;

        uint32_t s = ss;
        uint32_t eee = ee - 15 + 127;
        uint32_t m = mm << 13;
        uint32_t e;

        if (ee == 0) {
            if (mm == 0)
                e = 0;
            else {
                // Half denormal -> float normal
                return (ss ? -1 : 1) * std::scalbn((float)mm, -24);
            }
        } else if (ee == 0x1F) {
            // inf/nan
            e = 0xFF;
        } else
            e = eee;

        uint32_t f = (s << 31) | (e << 23) | m;

        return detail::bit_cast<float>(f);
#elif defined __ARM_NEON
        return (float)memcpy_cast<__fp16>(raw);
#else
#error "Unsupported architecture"
#endif
    }

    float f() const { return (float)(*this); }
};

} // namespace infinity

namespace std {
template <>
struct numeric_limits<infinity::float16_t> {
    using T = infinity::float16_t;

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
    static constexpr int digits = 11;  // mantissa bits (including implicit leading bit)
    static constexpr int digits10 = 3; // decimal digits of precision
    static constexpr int max_digits10 = 5;
    static constexpr int radix = 2;
    static constexpr int min_exponent = -14;
    static constexpr int min_exponent10 = -4;
    static constexpr int max_exponent = 16;
    static constexpr int max_exponent10 = 4;
    static constexpr bool traps = false;
    static constexpr bool tinyness_before = false;

    static constexpr T min() { return T(static_cast<uint16_t>(0x0400u)); }         // smallest positive normalized value (approx 6.10351563e-05)
    static constexpr T max() { return T(static_cast<uint16_t>(0x7BFFu)); }         // largest finite value (approx 65504.0)
    static constexpr T lowest() { return T(static_cast<uint16_t>(0xFBFFu)); }      // most negative finite value (approx -65504.0)
    static constexpr T epsilon() { return T(static_cast<uint16_t>(0x1400u)); }     // difference between 1.0 and next representable value
    static constexpr T round_error() { return T(static_cast<uint16_t>(0x3800u)); } // 0.5
    static constexpr T infinity() { return T(static_cast<uint16_t>(0x7C00u)); }
    static constexpr T quiet_NaN() { return T(static_cast<uint16_t>(0x7E00u)); }
    static constexpr T signaling_NaN() { return T(static_cast<uint16_t>(0x7D00u)); }
    static constexpr T denorm_min() { return T(static_cast<uint16_t>(0x0001u)); } // smallest positive denormalized value
};
} // namespace std
