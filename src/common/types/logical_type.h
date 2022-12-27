//
// Created by JinHai on 2022/10/30.
//

#pragma once

#include "internal_types.h"

namespace infinity {

// 40 types in total now.
enum LogicalType: i8 {
    // Bool * 1
    kBoolean = 0,

    // Integer * 5
    kTinyInt,
    kSmallInt,
    kInteger,
    kBigInt,
    kHugeInt,

    // Float * 2
    kFloat,
    kDouble,

    // Decimal * 4
    kDecimal16,
    kDecimal32,
    kDecimal64,
    kDecimal128,

    // Varchar * 1
    kVarchar,

    // Char * 7
    kChar1,
    kChar2,
    kChar4,
    kChar8,
    kChar16,
    kChar32,
    kChar64,

    // Date and Time * 6
    kDate,
    kTime,
    kDateTime,
    kTimestamp,
    kTimestampTZ,
    kInterval,

    // Nested types * 2
    kArray,
    kTuple,

    // Geography * 7
    kPoint,
    kLine,
    kLineSeg,
    kBox,
    kPath,
    kPolygon,
    kCircle,

    // Other * 4
    kBitmap,
    kUuid,
    kBlob,
    kEmbedding,

    // Heterogeneous type * 1
    kMixed,

    kNull,
    // only used in heterogeneous type
    kMissing,

    kInvalid,
};

enum class TypeFamily {

};
}
