//
// Created by JinHai on 2022/10/30.
//

#pragma once

#include "internal_types.h"

namespace infinity {

// 31 types in total now.
enum LogicalType: i8 {
    // Bool
    kBoolean = 0,

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
    kUuid,
    kBlob,
    kEmbedding,

    kNull,
    kMissing,
    kMixed,
    kInvalid,
    // kAny, // Used for function?
};

enum class TypeFamily {

};
}
