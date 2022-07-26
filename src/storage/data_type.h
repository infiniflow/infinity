//
// Created by JinHai on 2022/7/23.
//

#pragma once

#include <cstdint>
#include <string>

namespace infinity {

enum class PhysicalType : uint8_t {
    // Null type
    kNA = 0,
    // Boolean as 8 bit "bool" value
    kBool,
    // 8-bit little-endian integer
    kInt8,
    // 16-bit little-endian integer
    kInt16,
    // 32-bit little-endian integer
    kInt32,
    // 64-bit little-endian integer
    kInt64 = 9,
    // 4-byte floating point value
    kFloat = 11,
    // 8-byte floating point value
    kDouble = 12,
    // UTF8 variable-length string
    kString = 13,

    kInvalid = 255
};

enum class LogicalTypeId : uint8_t {
    kInvalid = 0,

    kBoolean,

    kTinyInt,
    kSmallInt,
    kInteger,
    kBigInt,

    kFloat,
    kDouble,

    kDecimal,

    kDate,
    kTime,
    kDateTime,
    kInterval,

    kVarchar,
    kText,

    kNull,
    kAny,
};

class LogicalType {
public:
    explicit LogicalType(LogicalTypeId logical_type_id, int64_t length, int64_t precision, int64_t scale);

    std::string to_string() const;
private:
    int64_t length_;
    int64_t precision_;
    int64_t scale_;

    LogicalTypeId logical_type_;
    PhysicalType physical_type_;
};

}


