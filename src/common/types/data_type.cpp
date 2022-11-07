//
// Created by JinHai on 2022/10/27.
//

#include "data_type.h"
#include "common/utility/infinity_assert.h"

#include "common/types/info/decimal_info.h"
#include "common/types/info/varchar_info.h"

namespace infinity {

static std::string type2name[] = {
    // Bool
    "Boolean",

    // Numeric
    "TinyInt",
    "SmallInt",
    "Integer",
    "BigInt",
    "HugeInt",
    "Float",
    "Double",
    "Decimal",

    // String
    "Varchar",

    // Date and Time
    "Date",
    "Time",
    "DateTime",
    "Timestamp",
    "TimestampTZ",
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

    // Heterogeneous type
    "Heterogeneous",

    // only used in heterogeneous type
    "Null",
    "Missing",

    "Invalid",
};

static i64 type_size[] = {
        // Bool
        1, // Boolean

        // Numeric
        1, // TinyInt
        2, // SmallInt
        4, // Integer
        8, // BigInt
        16, // HugeInt
        4, // Float
        8, // Double
        16, // Decimal

        // String
        16, // Varchar

        // Date and Time
        4, // Date
        4, // Time
        8, // DateTime
        8, // Timestamp
        8, // TimestampTZ
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
        46, // Polygon
        24, // Circle

        // Other
        16, // Bitmap
        16, // UUID
        16, // Blob
        8, // Embedding

        // Heterogeneous
        16, // Mixed

        // only used in heterogeneous type
        0, // Null
        0, // Missing
        0, // Invalid
};

std::string
DataType::ToString() const {
    if(type_ > kInvalid) {
        StorageError("Invalid logical data type.");
    }
    return type2name[type_];
}

bool
DataType::operator==(const DataType &other) const {
    if(type_ != other.type_) return false;
    if(*this->type_info_ != *other.type_info_) return false;
    return true;
}

bool
DataType::operator!=(const DataType& other) const {
    return !operator==(other);
}

size_t
DataType::Size() const {
    if(type_ > kInvalid) {
        StorageError("Invalid logical data type.");
    }
    return type_size[type_];
}

int64_t
DataType::CastRule(const DataType &from, const DataType &to) {
    return 0;
}

DataType
DataType::ConvertType(hsql::ColumnType type) {
    switch (type.data_type) {
        case hsql::DataType::BOOLEAN: return DataType(LogicalType::kBoolean);
        // No TinyInt
        case hsql::DataType::SMALLINT: return DataType(LogicalType::kSmallInt);
        case hsql::DataType::INT: return DataType(LogicalType::kInteger);
        case hsql::DataType::BIGINT:
        case hsql::DataType::LONG: return DataType(LogicalType::kBigInt);;

        case hsql::DataType::DECIMAL: {
            UniquePtr<DecimalInfo> decimal_info = MakeUnique<DecimalInfo>();
            return DataType(LogicalType::kDecimal, std::move(decimal_info));
        }

        case hsql::DataType::REAL:
        case hsql::DataType::FLOAT: return DataType(LogicalType::kFloat);
        case hsql::DataType::DOUBLE: return DataType(LogicalType::kDouble);

        case hsql::DataType::CHAR:
        case hsql::DataType::VARCHAR:
        case hsql::DataType::TEXT: {
            UniquePtr<VarcharInfo> varchar_info = MakeUnique<VarcharInfo>();
            return DataType(LogicalType::kVarchar, std::move(varchar_info));
        }

        case hsql::DataType::DATE: return DataType(LogicalType::kDate);
        case hsql::DataType::TIME: return DataType(LogicalType::kTime);
        case hsql::DataType::DATETIME: return DataType(LogicalType::kDateTime);
        case hsql::DataType::UNKNOWN: {
            TypeError("Unknown date type.");
        }
    }
}

i64
DataType::TypeSize(LogicalType logical_type) {
    return type_size[logical_type];
}

}


