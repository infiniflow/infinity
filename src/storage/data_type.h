//
// Created by JinHai on 2022/7/23.
//

#pragma once

#include <cstdint>
#include <string>
#include <memory>

namespace infinity {

enum class TableType { kInvalid, kFixedRowCount, kFixedChunkSize };

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

struct DecimalT {
    int8_t flags;
    int16_t precision;
    int16_t scale;
    int64_t number;
};

using BooleanT = bool;
using TinyIntT = int8_t;
using SmallIntT = int16_t;
using IntegerT = int32_t;
using BigIntT = int64_t;
using FloatT = float;
using DoubleT = double;
using DateT = int32_t;
using TimeT = int32_t;
using DateTimeT = int64_t;
using IntervalT = int64_t;
using VarcharT = std::string;
using TextT = std::string;


class LogicalType {
public:
    explicit LogicalType(LogicalTypeId logical_type_id);
    LogicalType(LogicalTypeId logical_type_id, int64_t length, int64_t precision, int64_t scale);

    [[nodiscard]] std::string ToString() const;
    [[nodiscard]] LogicalTypeId GetTypeId() const { return logical_type_; }
private:
    int64_t length_;
    int64_t precision_;
    int64_t scale_;

    LogicalTypeId logical_type_;
    PhysicalType physical_type_;
};

}


