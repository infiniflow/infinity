//
// Created by jinhai on 23-10-1.
//

#include "type/logical_type.h"

namespace infinity {

static const char *type2name[] = {
    // Bool
    "Boolean",

    // Numeric
    "TinyInt",
    "SmallInt",
    "Integer",
    "BigInt",
    "HugeInt",
    "Decimal",
    "Float",
    "Double",

    // std::string
    "Varchar",

    // Date and Time
    "Date",
    "Time",
    "DateTime",
    "Timestamp",
    "Interval",

    // Nested types
    "Array",
    "Tuple",

    // Geography
    "Point",
    "Line",
    "LineSegment",
    "Box",
    "Path",
    "Polygon",
    "Circle",

    // Other
    "Bitmap",
    "UUID",
    "Blob",
    "Embedding",
    "RowID",

    // Heterogeneous/Mix type
    "Heterogeneous",

    // only used in heterogeneous type
    "Null",
    "Missing",

    "Invalid",
};

static int64_t type_size[] = {
    // Bool * 1
    1, // Boolean

    // Integer * 5
    1,  // TinyInt
    2,  // SmallInt
    4,  // Integer
    8,  // BigInt
    16, // HugeInt

    // Decimal * 1
    16, // Decimal

    // Float * 2
    4, // Float
    8, // Double

    // Varchar * 1
    16, // Varchar

    // Date and Time * 6
    4, // Date
    4, // Time
    8, // DateTime
    8, // Timestamp
    8, // Interval

    // Nested types
    8, // Array
    4, // Tuple

    // Geography
    16, // Point
    24, // Line
    32, // LineSegment
    32, // Box
    16, // Path
    48, // Polygon
    24, // Circle

    // Other
    16, // Bitmap
    16, // UUID
    16, // Blob
    8,  // Embedding
    8,  // RowID

    // Heterogeneous
    16, // Mixed

    // only used in heterogeneous type
    0, // Null
    0, // Missing
    0, // Invalid
};

const char *LogicalType2Str(LogicalType logical_type) { return type2name[logical_type]; }

int64_t LogicalTypeWidth(LogicalType logical_type) { return type_size[logical_type]; }

} // namespace infinity
