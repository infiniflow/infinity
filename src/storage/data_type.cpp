//
// Created by JinHai on 2022/7/23.
//


#include "data_type.h"
#include "common/utility/infinity_assert.h"

#include <string>

namespace infinity {


LogicalType
LogicalType::TypeConversion(hsql::ColumnType type) {
    switch (type.data_type) {
        case hsql::DataType::BOOLEAN: return LogicalType(LogicalTypeId::kBoolean, type.length, type.precision, type.scale);
        case hsql::DataType::SMALLINT: return LogicalType(LogicalTypeId::kSmallInt, type.length, type.precision, type.scale);
        case hsql::DataType::INT: return LogicalType(LogicalTypeId::kInteger, type.length, type.precision, type.scale);
        case hsql::DataType::BIGINT: return LogicalType(LogicalTypeId::kBigInt, type.length, type.precision, type.scale);
        case hsql::DataType::LONG: return LogicalType(LogicalTypeId::kBigInt, type.length, type.precision, type.scale);

        case hsql::DataType::DECIMAL: return LogicalType(LogicalTypeId::kDecimal, type.length, type.precision, type.scale);

        case hsql::DataType::REAL: return LogicalType(LogicalTypeId::kFloat, type.length, type.precision, type.scale);
        case hsql::DataType::FLOAT: return LogicalType(LogicalTypeId::kFloat, type.length, type.precision, type.scale);
        case hsql::DataType::DOUBLE: return LogicalType(LogicalTypeId::kDouble, type.length, type.precision, type.scale);

        case hsql::DataType::CHAR: return LogicalType(LogicalTypeId::kVarchar, type.length, type.precision, type.scale);
        case hsql::DataType::VARCHAR: return LogicalType(LogicalTypeId::kVarchar, type.length, type.precision, type.scale);
        case hsql::DataType::TEXT: return LogicalType(LogicalTypeId::kVarchar, type.length, type.precision, type.scale);

        case hsql::DataType::DATE: return LogicalType(LogicalTypeId::kDate, type.length, type.precision, type.scale);
        case hsql::DataType::TIME: return LogicalType(LogicalTypeId::kTime, type.length, type.precision, type.scale);
        case hsql::DataType::DATETIME: return LogicalType(LogicalTypeId::kDateTime, type.length, type.precision, type.scale);
        case hsql::DataType::UNKNOWN: return LogicalType(LogicalTypeId::kInvalid, type.length, type.precision, type.scale);
    }
}

LogicalType::LogicalType(LogicalTypeId logical_type_id)
    : LogicalType(logical_type_id, 0, 0, 0) {}


LogicalType::LogicalType(infinity::LogicalTypeId logical_type_id, int64_t length, int64_t precision, int64_t scale)
    : logical_type_(logical_type_id) {
    switch(logical_type_) {
        case LogicalTypeId::kBoolean: physical_type_ = PhysicalType::kBool; break;
        case LogicalTypeId::kTinyInt: physical_type_ = PhysicalType::kInt8; break;
        case LogicalTypeId::kSmallInt: physical_type_ = PhysicalType::kInt16; break;
        case LogicalTypeId::kInteger: physical_type_ = PhysicalType::kInt32; break;
        case LogicalTypeId::kBigInt: physical_type_ = PhysicalType::kInt64; break;
        case LogicalTypeId::kFloat: physical_type_ = PhysicalType::kFloat; break;
        case LogicalTypeId::kDouble: physical_type_ = PhysicalType::kDouble; break;
        case LogicalTypeId::kDecimal: physical_type_ = PhysicalType::kFloat; break;
        case LogicalTypeId::kDate: physical_type_ = PhysicalType::kInt64; break;
        case LogicalTypeId::kTime: physical_type_ = PhysicalType::kInt64; break;
        case LogicalTypeId::kDateTime: physical_type_ = PhysicalType::kInt64; break;
        case LogicalTypeId::kInterval: physical_type_ = PhysicalType::kInt64; break;
        case LogicalTypeId::kVarchar: physical_type_ = PhysicalType::kString; break;
        case LogicalTypeId::kText: physical_type_ = PhysicalType::kString; break;
        default: break;
    }
}

std::string
LogicalType::ToString() const {
    switch(logical_type_) {
        case LogicalTypeId::kBoolean: return "Boolean";
        case LogicalTypeId::kTinyInt: return "TinyInt";
        case LogicalTypeId::kSmallInt: return "SmallInt";
        case LogicalTypeId::kInteger: return "Integer";
        case LogicalTypeId::kBigInt: return "BigInt";
        case LogicalTypeId::kFloat: return "Real";
        case LogicalTypeId::kDouble: return "Float";
        case LogicalTypeId::kDecimal: return "Decimal";
        case LogicalTypeId::kDate: return "Date";
        case LogicalTypeId::kTime: return "Time";
        case LogicalTypeId::kDateTime: return "DateTime";
        case LogicalTypeId::kInterval: return "Interval";
        case LogicalTypeId::kVarchar: return "Varchar";
        case LogicalTypeId::kText: return "Text";
        default:
            StorageError("Invalid logical data type.");
    }
}

size_t
LogicalType::Size() const {
    switch(logical_type_) {
        case LogicalTypeId::kBoolean:
            return sizeof(BooleanT);
        case LogicalTypeId::kTinyInt:
            return sizeof(TinyIntT);
        case LogicalTypeId::kSmallInt:
            return sizeof(SmallIntT);
        case LogicalTypeId::kInteger:
            return sizeof(IntegerT);
        case LogicalTypeId::kBigInt:
            return sizeof(BigIntT);
        case LogicalTypeId::kFloat:
            return sizeof(FloatT);
        case LogicalTypeId::kDouble:
            return sizeof(DoubleT);
        case LogicalTypeId::kDecimal:
            return sizeof(DecimalT);
        case LogicalTypeId::kDate:
            return sizeof(DateT);
        case LogicalTypeId::kTime:
            return sizeof(TimeT);
        case LogicalTypeId::kDateTime:
            return sizeof(DateTimeT);
        case LogicalTypeId::kInterval:
            return sizeof(IntervalT);
        case LogicalTypeId::kVarchar:
            return sizeof(VarcharT);
        case LogicalTypeId::kText:
            return sizeof(TextT);
        default:
            ;
    }
    TypeError("Invalid type of LogicalType.")
}

}
