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
#include <unordered_set>
#include <set>
#include <vector>
#include <unordered_map>
#include <map>

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

    "Invalid",
};

std::map<std::set<std::string>, LogicalType> name2type = {
    // Bool
    {{ "Boolean", "bool", "boolean" }, LogicalType::kBoolean }, 

    {{ "TinyInt", "tinyint" }, LogicalType::kTinyInt },
    {{ "SmallInt", "smallint" }, LogicalType::kSmallInt },
    {{ "Integer", "int", "integer" }, LogicalType::kInteger },
    {{ "BigInt", "bigint" }, LogicalType::kBigInt },
    {{ "HugeInt", "hugeint" }, LogicalType::kHugeInt },
    {{ "Decimal", "decimal" }, LogicalType::kDecimal },
    {{ "Float", "float" }, LogicalType::kFloat },
    {{ "Double", "double" }, LogicalType::kDouble },

    // std::string
    {{ "Varchar", "varchar" }, LogicalType::kVarchar }, 

    // Date and Time
    {{ "Date", "date" }, LogicalType::kDate }, 
    {{ "Time", "time" }, LogicalType::kTime },
    {{ "DateTime", "datetime" }, LogicalType::kDateTime },
    {{ "Timestamp", "timestamp" }, LogicalType::kTimestamp },
    {{ "Interval", "interval" }, LogicalType::kInterval },

    // Nested types
    {{ "Array", "array" }, LogicalType::kArray }, 
    {{ "Tuple", "tuple" }, LogicalType::kTuple },

    // Geography
    {{ "Point", "point" }, LogicalType::kPoint }, 
    {{ "Line", "line" }, LogicalType::kLine },
    {{ "LineSegment", "linesegment" }, LogicalType::kLineSeg },
    {{ "Box", "box" }, LogicalType::kBox },

    //    "Path",
    //    "Polygon",
    {{ "Circle", "circle" }, LogicalType::kCircle },

    // Other
    //    "Bitmap",
    {{ "UUID", "uuid" }, LogicalType::kUuid },

    //    "Blob",
    {{ "Embedding", "embedding" }, LogicalType::kEmbedding },
    {{ "RowID", "rowid" }, LogicalType::kRowID },

    // Heterogeneous/Mix type
    {{ "Heterogeneous", "heterogeneous" }, LogicalType::kMixed }, 
    {{ "Null", "null" }, LogicalType::kNull },
    {{ "Missing", "missing" }, LogicalType::kMissing },

    {{ "Invalid", "invalid" }, LogicalType::kInvalid },
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
    0, // Invalid
};

const char *LogicalType2Str(LogicalType logical_type) { return type2name[logical_type]; }

LogicalType Str2LogicalType(std::string str) {
    for(auto &name_map : name2type){
        if(name_map.first.find(str) != name_map.first.end()){
            return name_map.second;   
        }
    }
    return LogicalType::kInvalid; 
}

int64_t LogicalTypeWidth(LogicalType logical_type) { return type_size[logical_type]; }

} // namespace infinity
