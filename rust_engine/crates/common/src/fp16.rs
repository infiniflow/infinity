//! IEEE binary16 conversion (`float16_t`).
//!
//! C++: `src/unit_test/parser/type/fp16_ut.cpp` exercises the implicit
//! `float16_t` conversion from `float` on exact powers of two, so the
//! round-to-nearest-even rules below are observable at the bit level.

/// Converts an `f32` to IEEE binary16 bits (round-to-nearest-even).
pub fn f32_to_f16_bits(value: f32) -> u16 {
    let bits = value.to_bits();
    let sign = ((bits >> 16) & 0x8000) as u16;
    let exp = ((bits >> 23) & 0xff) as i32;
    let mantissa = bits & 0x007f_ffff;

    if exp == 0xff {
        // NaN keeps the quiet bit; infinity stays infinity.
        let payload = if mantissa != 0 {
            ((mantissa >> 13) as u16) | 0x0200
        } else {
            0
        };
        return sign | 0x7c00 | payload;
    }

    let half_exp = exp - 127 + 15;
    if half_exp >= 0x1f {
        return sign | 0x7c00;
    }

    if half_exp <= 0 {
        if half_exp < -10 {
            return sign;
        }
        // Normalise the implicit leading bit and shift into the subnormal
        // range.
        let mantissa = mantissa | 0x0080_0000;
        let shift = (14 - half_exp) as u32;
        let mut half = (mantissa >> shift) as u16;
        let remainder = mantissa & ((1u32 << shift) - 1);
        let halfway = 1u32 << (shift - 1);
        if remainder > halfway || (remainder == halfway && (half & 1) != 0) {
            half += 1;
        }
        return sign | half;
    }

    let mut half = (((half_exp as u32) << 10) as u16) | ((mantissa >> 13) as u16);
    let remainder = mantissa & 0x1fff;
    if remainder > 0x1000 || (remainder == 0x1000 && (half & 1) != 0) {
        half += 1;
    }
    sign | half
}

#[cfg(test)]
mod tests {
    use super::f32_to_f16_bits;

    /// C++ `src/unit_test/parser/type/fp16_ut.cpp:19 FP16TypeTest.test1`.
    #[test]
    fn fp16_test1() {
        let cases: [(u32, u16); 12] = [
            (0x3880_0000, 0x0400), // 2^-14 (smallest normal)
            (0x3e00_0000, 0x3000), // 0.125
            (0x3e80_0000, 0x3400), // 0.25
            (0x3f00_0000, 0x3800), // 0.5
            (0x3f80_0000, 0x3c00), // 1.0
            (0x4000_0000, 0x4000), // 2.0
            (0x4080_0000, 0x4400), // 4.0
            (0x4100_0000, 0x4800), // 8.0
            (0x4180_0000, 0x4c00), // 16.0
            (0x4200_0000, 0x5000), // 32.0
            (0x4280_0000, 0x5400), // 64.0
            (0x4700_0000, 0x7800), // 32768.0
        ];
        for (f32_bits, expected) in cases {
            let value = f32::from_bits(f32_bits);
            assert_eq!(
                f32_to_f16_bits(value),
                expected,
                "f32 bits {f32_bits:#010x} ({value})"
            );
        }
    }
}
