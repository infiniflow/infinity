// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

export module infinity_core:smallfloat;

import :infinity_type;

namespace infinity {

export class SmallFloat {
public:
    SmallFloat() {}
    ~SmallFloat() {}

private:
    union ifvalue {
        i32 i;
        float f;
    };

public:
    static i32 FloatToIntBits(float value) {
        ifvalue u;
        u.f = value;
        return u.i;
    }

    static float IntBitsToFloat(i32 bits) {
        ifvalue u;
        u.i = bits;
        return u.f;
    }

    // Converts a 32-bit float to a 16-bit unsigned integer (u16).
    // Values less than zero are mapped to zero.
    // Values are truncated (rounded down) to the nearest 16-bit value.
    // Values between zero and the smallest representable value are rounded up.
    // num_mantissa_bits the number of mantissa bits to use in the u16
    // zero_exp the zero-point in the range of exponent values
    static u16 FloatToUInt16(float f, int num_mantissa_bits, int zero_exp) {
        // Adjustment from a float zero exponent to our zero exponent,
        // shifted over to our exponent position.
        int fzero = (127 - zero_exp) << num_mantissa_bits;
        int bits = FloatToIntBits(f);
        int smallfloat = bits >> (23 - num_mantissa_bits);

        if (smallfloat < fzero) {
            return (bits <= 0) ? 0  // negative numbers and zero both map to 0
                               : 1; // underflow is mapped to smallest non-zero number.
        } else if (smallfloat >= fzero + 0x10000) {
            return 0xFFFF; // overflow maps to largest number
        } else {
            return static_cast<u16>(smallfloat - fzero);
        }
    }

    static float UInt16ToFloat(u16 b, int num_mantissa_bits, int zero_exp) {
        if (b == 0)
            return 0.0f;
        int bits = (b & 0xFFFF) << (23 - num_mantissa_bits);
        bits += (127 - zero_exp) << 23;
        return IntBitsToFloat(bits);
    }

    //
    // Specializations of the generic functions follow.
    //

    // FloatToUInt16(f, mantissa_bits=10, zero_exponent=15)
    // smallest non-zero value = 5.820766E-10
    // largest value = 7.5161928E9
    // epsilon = 0.125
    static u16 Float1015ToUInt16(float f) {
        int bits = FloatToIntBits(f);
        int smallfloat = bits >> (23 - 10);
        if (smallfloat < (127 - 15) << 10) {
            return (bits <= 0) ? 0 : 1;
        }
        if (smallfloat >= ((127 - 15) << 10) + 0x10000) {
            return 0xFFFF;
        }
        return static_cast<u16>(smallfloat - ((127 - 15) << 10));
    }

    // UInt16ToFloat(b, mantissa_bits=10, zero_exponent=15)
    static float UInt16ToFloat1015(u16 b) {
        if (b == 0)
            return 0.0f;
        int bits = (b & 0xFFFF) << (23 - 10);
        bits += (127 - 15) << 23;
        return IntBitsToFloat(bits);
    }

    // FloatToUInt16(f, mantissa_bits=12, zero_exponent=2)
    // smallest nonzero value = 0.033203125
    // largest value = 1984.0
    // epsilon = 0.03125
    static u16 Float122ToUInt16(float f) {
        int bits = FloatToIntBits(f);
        int smallfloat = bits >> (23 - 12);
        if (smallfloat < (127 - 2) << 12) {
            return (bits <= 0) ? 0 : 1;
        }
        if (smallfloat >= ((127 - 2) << 12) + 0x10000) {
            return 0xFFFF;
        }
        return static_cast<u16>(smallfloat - ((127 - 2) << 12));
    }

    // UInt16ToFloat(b, mantissa_bits=12, zero_exponent=2)
    static float UInt16ToFloat122(u16 b) {
        if (b == 0)
            return 0.0f;
        int bits = (b & 0xFFFF) << (23 - 12);
        bits += (127 - 2) << 23;
        return IntBitsToFloat(bits);
    }
};
} // namespace infinity
