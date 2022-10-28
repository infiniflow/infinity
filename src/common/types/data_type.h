//
// Created by JinHai on 2022/10/27.
//

#pragma once

#include "internal_types.h"
#include "type_info.h"

#include <string>

namespace infinity {

enum class LogicalType {
    kInvalid = 0,

    // Bool
    kBoolean,

    // Numeric
    kTinyInt,
    kSmallInt,
    kInteger,
    kBigInt,
    kHugeInt,

    kFloat,
    kDouble,

    kDecimal,

    // String
    kVarchar,

    // Date and Time
    kDate,
    kTime,
    kDateTime,
    kTimestamp,
    kTimestampTZ,
    kInterval,

    // Nested types
    kArray,
    kObject,

    // Geography
    kPoint,
    kLine,
    kLineSeg,
    kBox,
    kPath,
    kPolygon,
    kCircle,

    // Other
    kBitmap,
    kUUID,
    kBlob,
    kVector,
    kNull,

    // Used for function?
    kAny,
};

class DataType {
public:
    explicit
    DataType(LogicalType logical_type) : type_(logical_type) {};

    explicit
    DataType(LogicalType logical_type, UniquePtr<TypeInfo> type_info_ptr) :
        type_(logical_type), type_info_(std::move(type_info_ptr)) {}

    bool
    operator==(const DataType& other) const;

    bool
    operator!=(const DataType& other) const;

    [[nodiscard]] std::string
    ToString() const;

    [[nodiscard]] size_t
    Size() const;

    [[nodiscard]] LogicalType
    type() const { return type_; }

private:

    LogicalType type_;
    UniquePtr<TypeInfo> type_info_;

};

}
