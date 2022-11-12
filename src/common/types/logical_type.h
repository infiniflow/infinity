//
// Created by JinHai on 2022/10/30.
//

#pragma once

#include "internal_types.h"

namespace infinity {

// 35 types in total now.
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

    kDecimal16,
    kDecimal32,
    kDecimal64,
    kDecimal128,

    // String
    kVarchar,
    kChar1,
    kChar2,
    kChar4,
    kChar8,
    kChar16,
    kChar32,
    kChar64,

    // Date and Time
    kDate,
    kTime,
    kDateTime,
    kTimestamp,
    kTimestampTZ,
    kInterval,

    // Nested types
    kArray,
    kTuple,

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

    // Heterogeneous type
    kMixed,

    // only used in heterogeneous type
    kNull,
    kMissing,

    kInvalid,
};

enum class TypeFamily {

};
}
