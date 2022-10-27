//
// Created by JinHai on 2022/10/27.
//

#pragma once

#include "internal_types.h"

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
    DataType(LogicalType logical_type);

private:
};

}
