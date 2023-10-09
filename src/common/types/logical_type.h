//
// Created by JinHai on 2022/10/30.
//

#pragma once

#include "common/types/complex/bitmap_type.h"
#include "common/types/complex/blob_type.h"
#include "common/types/complex/char_type.h"
#include "common/types/complex/embedding_type.h"
#include "common/types/complex/uuid_type.h"
#include "common/types/complex/varchar_type.h"
#include "common/types/datetime/date_type.h"
#include "common/types/datetime/datetime_type.h"
#include "common/types/datetime/interval_type.h"
#include "common/types/datetime/time_type.h"
#include "common/types/datetime/timestamp_type.h"
#include "common/types/datetime/timestamp_tz_type.h"
#include "common/types/geo/box_type.h"
#include "common/types/geo/circle_type.h"
#include "common/types/geo/line_segment_type.h"
#include "common/types/geo/line_type.h"
#include "common/types/geo/path_type.h"
#include "common/types/geo/point_type.h"
#include "common/types/geo/polygon_type.h"
#include "common/types/heterogenous/mixed_type.h"
#include "common/types/number/decimal_type.h"
#include "common/types/number/huge_int.h"

namespace infinity {

// 40 types in total now.
enum LogicalType : i8 {
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

extern i64 LogicalTypeWidth(LogicalType logical_type);

} // namespace infinity
