//
// Created by JinHai on 2022/10/30.
//

#pragma once

#include "type/complex/bitmap_type.h"
#include "type/complex/blob_type.h"
#include "type/complex/char_type.h"
#include "type/complex/embedding_type.h"
#include "type/complex/uuid_type.h"
#include "type/complex/varchar_type.h"
#include "type/datetime/date_type.h"
#include "type/datetime/datetime_type.h"
#include "type/datetime/interval_type.h"
#include "type/datetime/time_type.h"
#include "type/datetime/timestamp_type.h"
#include "type/datetime/timestamp_tz_type.h"
#include "type/geo/box_type.h"
#include "type/geo/circle_type.h"
#include "type/geo/line_segment_type.h"
#include "type/geo/line_type.h"
#include "type/geo/path_type.h"
#include "type/geo/point_type.h"
#include "type/geo/polygon_type.h"
#include "type/heterogenous/mixed_type.h"
#include "type/number/decimal_type.h"
#include "type/number/huge_int.h"

namespace infinity {

// 40 types in total now.
enum LogicalType : int8_t {
    // Bool * 1
    kBoolean = 0,

    // Integer * 5
    kTinyInt,
    kSmallInt,
    kInteger,
    kBigInt,
    kHugeInt,

    // Decimal * 1
    kDecimal,

    // Float * 2
    kFloat,
    kDouble,

    // Varchar * 1
    kVarchar,

    // Date and Time * 5
    kDate,
    kTime,
    kDateTime,
    kTimestamp,
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
    kRowID,

    // Heterogeneous type * 1
    kMixed,

    kNull,
    // only used in heterogeneous type
    kMissing,

    kInvalid,
};

extern const char *LogicalType2Str(LogicalType logical_type);

extern int64_t LogicalTypeWidth(LogicalType logical_type);

} // namespace infinity
