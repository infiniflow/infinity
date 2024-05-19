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

#include "type/logical_type.h"
#include <memory>
#include <string>
#include <unordered_map>
 
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
//    "Path",
//    "Polygon",
    "Circle",

    // Other
//    "Bitmap",
    "UUID",
//    "Blob",
    "Embedding",
    "RowID",

    // Heterogeneous/Mix type
    "Heterogeneous",

    // only used in heterogeneous type
    "Null",
    "Missing",

    // tensor type
    "Tensor",

    "Invalid",
};

std::unordered_map<std::string, LogicalType> name2type = {
    // Bool
    {"boolean", LogicalType::kBoolean }, 

    {"tinyint", LogicalType::kTinyInt },
    {"smallint" , LogicalType::kSmallInt },
    {"integer" , LogicalType::kInteger },
    {"bigint" , LogicalType::kBigInt },
    {"hugeint" , LogicalType::kHugeInt },
    {"decimal" , LogicalType::kDecimal },
    {"float" , LogicalType::kFloat },
    {"double" , LogicalType::kDouble },

    // std::string
    {"varchar" , LogicalType::kVarchar }, 

    // Date and Time
    {"date" , LogicalType::kDate }, 
    {"time" , LogicalType::kTime },
    { "datetime" , LogicalType::kDateTime },
    { "timestamp" , LogicalType::kTimestamp },
    { "interval" , LogicalType::kInterval },

    // Nested types
    {"array" , LogicalType::kArray }, 
    {"tuple" , LogicalType::kTuple },

    // Geography
    { "point" , LogicalType::kPoint }, 
    {"line" , LogicalType::kLine },
    { "linesegment" , LogicalType::kLineSeg },
    {"box" , LogicalType::kBox },

    //    "Path",
    //    "Polygon",
    {"circle" , LogicalType::kCircle },

    // Other
    //    "Bitmap",
    { "uuid" , LogicalType::kUuid },

    //    "Blob",
    { "embedding" , LogicalType::kEmbedding },
    {"rowid" , LogicalType::kRowID },

    // Heterogeneous/Mix type
    { "heterogeneous" , LogicalType::kMixed }, 
    { "null" , LogicalType::kNull },
    { "missing" , LogicalType::kMissing },

    // tensor
    { "tensor" , LogicalType::kTensor },

    { "invalid" , LogicalType::kInvalid },
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
//    16, // Path
//    48, // Polygon
    24, // Circle

    // Other
//    16, // Bitmap
    16, // UUID
//    16, // Blob
    8,  // Embedding
    8,  // RowID

    // Heterogeneous
    16, // Mixed

    // only used in heterogeneous type
    0, // Null
    0, // Missing

    // tensor type
    8, // Tensor

    0, // Invalid
};

const char *LogicalType2Str(LogicalType logical_type) { return type2name[logical_type]; }

LogicalType Str2LogicalType(const std::string &str) {
    auto iter = name2type.find(str);
    if(iter != name2type.end()){
        return iter->second;
    } else {
        return LogicalType::kInvalid;
    }
}

int64_t LogicalTypeWidth(LogicalType logical_type) { return type_size[logical_type]; }

} // namespace infinity
