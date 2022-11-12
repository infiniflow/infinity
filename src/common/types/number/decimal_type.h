//
// Created by JinHai on 2022/10/28.
//

#pragma once

#include "common/types/logical_type.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

struct Decimal16Type {
    i16 value;
};

struct Decimal32Type {
    i32 value;
};

struct Decimal64Type {
    i64 value;
};

struct Decimal128Type {
    i64 upper;
    i64 lower;
};

struct DecimalType {
public:
    static LogicalType
    GetLogicalType(i64 precision) {
        if(precision <= GetDecimal16Precision()) {
            return LogicalType::kDecimal16;
        }
        if(precision > GetDecimal16Precision() && precision <= GetDecimal32Precision()) {
            return LogicalType::kDecimal32;
        }
        if(precision > GetDecimal32Precision() && precision <= GetDecimal64Precision()) {
            return LogicalType::kDecimal64;
        }
        if(precision <= GetDecimal128Precision()) {
            return LogicalType::kDecimal128;
        }
        TypeError("Decimal precision is larger than " + std::to_string(GetDecimal128Precision()));
    }

    static inline constexpr i64
    GetDecimal16Precision() {
        return 4;
    }

    static inline constexpr i64
    GetDecimal32Precision() {
        return 9;
    }

    static inline constexpr i64
    GetDecimal64Precision() {
        return 18;
    }

    static inline constexpr i64
    GetDecimal128Precision() {
        return 38;
    }
private:
};

}
