// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
