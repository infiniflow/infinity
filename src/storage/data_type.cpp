//
// Created by JinHai on 2022/7/23.
//


#include "data_type.h"
#include "common/utility/infinity_assert.h"

#include <string>
#include <unordered_map>

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

    return LogicalType(LogicalTypeId::kInvalid);
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

static std::unordered_map<LogicalTypeId, std::unordered_map<LogicalTypeId, int64_t>> TypeCastMap {
    {LogicalTypeId::kBoolean, std::unordered_map<LogicalTypeId, int64_t>({
        {LogicalTypeId::kBoolean, 0},
        {LogicalTypeId::kTinyInt, -1},
        {LogicalTypeId::kSmallInt, -1},
        {LogicalTypeId::kInteger, -1},
        {LogicalTypeId::kBigInt, -1},
        {LogicalTypeId::kFloat, -1},
        {LogicalTypeId::kDouble, -1},
        {LogicalTypeId::kDecimal, -1},
        {LogicalTypeId::kDate, -1},
        {LogicalTypeId::kDateTime, -1},
        {LogicalTypeId::kTime, -1},
        {LogicalTypeId::kInterval, -1},
        {LogicalTypeId::kVarchar, 1000},
        {LogicalTypeId::kText, 1000},
        {LogicalTypeId::kNull, -1},
        {LogicalTypeId::kAny, 1},
    })},
    {LogicalTypeId::kTinyInt, std::unordered_map<LogicalTypeId, int64_t>({
        {LogicalTypeId::kBoolean, -1},
        {LogicalTypeId::kTinyInt, 0},
        {LogicalTypeId::kSmallInt, 1},
        {LogicalTypeId::kInteger, 2},
        {LogicalTypeId::kBigInt, 3},
        {LogicalTypeId::kFloat, 4},
        {LogicalTypeId::kDouble, 5},
        {LogicalTypeId::kDecimal, 6},
        {LogicalTypeId::kDate, -1},
        {LogicalTypeId::kDateTime, -1},
        {LogicalTypeId::kTime, -1},
        {LogicalTypeId::kInterval, -1},
        {LogicalTypeId::kVarchar, 1000},
        {LogicalTypeId::kText, 1000},
        {LogicalTypeId::kNull, -1},
        {LogicalTypeId::kAny, 1},
    })},
    {LogicalTypeId::kSmallInt, std::unordered_map<LogicalTypeId, int64_t>({
        {LogicalTypeId::kBoolean, -1},
        {LogicalTypeId::kTinyInt, -1},
        {LogicalTypeId::kSmallInt, 0},
        {LogicalTypeId::kInteger, 1},
        {LogicalTypeId::kBigInt, 2},
        {LogicalTypeId::kFloat, 3},
        {LogicalTypeId::kDouble, 4},
        {LogicalTypeId::kDecimal, 5},
        {LogicalTypeId::kDate, -1},
        {LogicalTypeId::kDateTime, -1},
        {LogicalTypeId::kTime, -1},
        {LogicalTypeId::kInterval, -1},
        {LogicalTypeId::kVarchar, 1000},
        {LogicalTypeId::kText, 1000},
        {LogicalTypeId::kNull, -1},
        {LogicalTypeId::kAny, 1},
    })},
    {LogicalTypeId::kInteger, std::unordered_map<LogicalTypeId, int64_t>({
        {LogicalTypeId::kBoolean, -1},
        {LogicalTypeId::kTinyInt, -1},
        {LogicalTypeId::kSmallInt, -1},
        {LogicalTypeId::kInteger, 0},
        {LogicalTypeId::kBigInt, 1},
        {LogicalTypeId::kFloat, 2},
        {LogicalTypeId::kDouble, 3},
        {LogicalTypeId::kDecimal, 4},
        {LogicalTypeId::kDate, -1},
        {LogicalTypeId::kDateTime, -1},
        {LogicalTypeId::kTime, -1},
        {LogicalTypeId::kInterval, -1},
        {LogicalTypeId::kVarchar, 1000},
        {LogicalTypeId::kText, 1000},
        {LogicalTypeId::kNull, -1},
        {LogicalTypeId::kAny, 1},
    })},
    {LogicalTypeId::kBigInt, std::unordered_map<LogicalTypeId, int64_t>({
        {LogicalTypeId::kBoolean, -1},
        {LogicalTypeId::kTinyInt, -1},
        {LogicalTypeId::kSmallInt, -1},
        {LogicalTypeId::kInteger, -1},
        {LogicalTypeId::kBigInt, 0},
        {LogicalTypeId::kFloat, 1},
        {LogicalTypeId::kDouble, 2},
        {LogicalTypeId::kDecimal, 10},
        {LogicalTypeId::kDate, -1},
        {LogicalTypeId::kDateTime, -1},
        {LogicalTypeId::kTime, -1},
        {LogicalTypeId::kInterval, -1},
        {LogicalTypeId::kVarchar, 1000},
        {LogicalTypeId::kText, 1000},
        {LogicalTypeId::kNull, -1},
        {LogicalTypeId::kAny, 1},
    })},
    {LogicalTypeId::kFloat, std::unordered_map<LogicalTypeId, int64_t>({
        {LogicalTypeId::kBoolean, -1},
        {LogicalTypeId::kTinyInt, -1},
        {LogicalTypeId::kSmallInt, -1},
        {LogicalTypeId::kInteger, -1},
        {LogicalTypeId::kBigInt, -1},
        {LogicalTypeId::kFloat, 0},
        {LogicalTypeId::kDouble, 1},
        {LogicalTypeId::kDecimal, -1},
        {LogicalTypeId::kDate, -1},
        {LogicalTypeId::kDateTime, -1},
        {LogicalTypeId::kTime, -1},
        {LogicalTypeId::kInterval, -1},
        {LogicalTypeId::kVarchar, 1000},
        {LogicalTypeId::kText, 1000},
        {LogicalTypeId::kNull, -1},
        {LogicalTypeId::kAny, 1},
    })},
    {LogicalTypeId::kDouble, std::unordered_map<LogicalTypeId, int64_t>({
        {LogicalTypeId::kBoolean, -1},
        {LogicalTypeId::kTinyInt, -1},
        {LogicalTypeId::kSmallInt, -1},
        {LogicalTypeId::kInteger, -1},
        {LogicalTypeId::kBigInt, -1},
        {LogicalTypeId::kFloat, -1},
        {LogicalTypeId::kDouble, 0},
        {LogicalTypeId::kDecimal, -1},
        {LogicalTypeId::kDate, -1},
        {LogicalTypeId::kDateTime, -1},
        {LogicalTypeId::kTime, -1},
        {LogicalTypeId::kInterval, -1},
        {LogicalTypeId::kVarchar, 1000},
        {LogicalTypeId::kText, 1000},
        {LogicalTypeId::kNull, -1},
        {LogicalTypeId::kAny, 1},
    })},
    {LogicalTypeId::kDecimal, std::unordered_map<LogicalTypeId, int64_t>({
        {LogicalTypeId::kBoolean, -1},
        {LogicalTypeId::kTinyInt, -1},
        {LogicalTypeId::kSmallInt, -1},
        {LogicalTypeId::kInteger, -1},
        {LogicalTypeId::kBigInt, -1},
        {LogicalTypeId::kFloat, 2},
        {LogicalTypeId::kDouble, 1},
        {LogicalTypeId::kDecimal, 0},
        {LogicalTypeId::kDate, -1},
        {LogicalTypeId::kDateTime, -1},
        {LogicalTypeId::kTime, -1},
        {LogicalTypeId::kInterval, -1},
        {LogicalTypeId::kVarchar, 1000},
        {LogicalTypeId::kText, 1000},
        {LogicalTypeId::kNull, -1},
        {LogicalTypeId::kAny, 1},
    })},
    {LogicalTypeId::kDate, std::unordered_map<LogicalTypeId, int64_t>({
        {LogicalTypeId::kBoolean, -1},
        {LogicalTypeId::kTinyInt, -1},
        {LogicalTypeId::kSmallInt, -1},
        {LogicalTypeId::kInteger, -1},
        {LogicalTypeId::kBigInt, -1},
        {LogicalTypeId::kFloat, -1},
        {LogicalTypeId::kDouble, -1},
        {LogicalTypeId::kDecimal, -1},
        {LogicalTypeId::kDate, 0},
        {LogicalTypeId::kDateTime, 1},
        {LogicalTypeId::kTime, -1},
        {LogicalTypeId::kInterval, -1},
        {LogicalTypeId::kVarchar, 1000},
        {LogicalTypeId::kText, 1000},
        {LogicalTypeId::kNull, -1},
        {LogicalTypeId::kAny, 1},
    })},
    {LogicalTypeId::kDateTime, std::unordered_map<LogicalTypeId, int64_t>({
        {LogicalTypeId::kBoolean, -1},
        {LogicalTypeId::kTinyInt, -1},
        {LogicalTypeId::kSmallInt, -1},
        {LogicalTypeId::kInteger, -1},
        {LogicalTypeId::kBigInt, -1},
        {LogicalTypeId::kFloat, -1},
        {LogicalTypeId::kDouble, -1},
        {LogicalTypeId::kDecimal, -1},
        {LogicalTypeId::kDate, -1},
        {LogicalTypeId::kDateTime, 0},
        {LogicalTypeId::kTime, -1},
        {LogicalTypeId::kInterval, -1},
        {LogicalTypeId::kVarchar, 1000},
        {LogicalTypeId::kText, 1000},
        {LogicalTypeId::kNull, -1},
        {LogicalTypeId::kAny, 1},
    })},
    {LogicalTypeId::kTime, std::unordered_map<LogicalTypeId, int64_t>({
        {LogicalTypeId::kBoolean, -1},
        {LogicalTypeId::kTinyInt, -1},
        {LogicalTypeId::kSmallInt, -1},
        {LogicalTypeId::kInteger, -1},
        {LogicalTypeId::kBigInt, -1},
        {LogicalTypeId::kFloat, -1},
        {LogicalTypeId::kDouble, -1},
        {LogicalTypeId::kDecimal, -1},
        {LogicalTypeId::kDate, -1},
        {LogicalTypeId::kDateTime, -1},
        {LogicalTypeId::kTime, 0},
        {LogicalTypeId::kInterval, -1},
        {LogicalTypeId::kVarchar, 1000},
        {LogicalTypeId::kText, 1000},
        {LogicalTypeId::kNull, -1},
        {LogicalTypeId::kAny, 1},
    })},
    {LogicalTypeId::kInterval, std::unordered_map<LogicalTypeId, int64_t>({
        {LogicalTypeId::kBoolean, -1},
        {LogicalTypeId::kTinyInt, -1},
        {LogicalTypeId::kSmallInt, -1},
        {LogicalTypeId::kInteger, -1},
        {LogicalTypeId::kBigInt, -1},
        {LogicalTypeId::kFloat, -1},
        {LogicalTypeId::kDouble, -1},
        {LogicalTypeId::kDecimal, -1},
        {LogicalTypeId::kDate, -1},
        {LogicalTypeId::kDateTime, -1},
        {LogicalTypeId::kTime, -1},
        {LogicalTypeId::kInterval, 0},
        {LogicalTypeId::kVarchar, 1000},
        {LogicalTypeId::kText, 1000},
        {LogicalTypeId::kNull, -1},
        {LogicalTypeId::kAny, 1},
    })},
    {LogicalTypeId::kVarchar, std::unordered_map<LogicalTypeId, int64_t>({
        {LogicalTypeId::kBoolean, 1000},
        {LogicalTypeId::kTinyInt, 1000},
        {LogicalTypeId::kSmallInt, 1000},
        {LogicalTypeId::kInteger, 1000},
        {LogicalTypeId::kBigInt, 1000},
        {LogicalTypeId::kFloat, 1000},
        {LogicalTypeId::kDouble, 1000},
        {LogicalTypeId::kDecimal, 1000},
        {LogicalTypeId::kDate, 1000},
        {LogicalTypeId::kDateTime, 1000},
        {LogicalTypeId::kTime, 1000},
        {LogicalTypeId::kInterval, 1000},
        {LogicalTypeId::kVarchar, 0},
        {LogicalTypeId::kText, 1},
        {LogicalTypeId::kNull, -1},
        {LogicalTypeId::kAny, 1},
    })},
    {LogicalTypeId::kText, std::unordered_map<LogicalTypeId, int64_t>({
        {LogicalTypeId::kBoolean, 1000},
        {LogicalTypeId::kTinyInt, 1000},
        {LogicalTypeId::kSmallInt, 1000},
        {LogicalTypeId::kInteger, 1000},
        {LogicalTypeId::kBigInt, 1000},
        {LogicalTypeId::kFloat, 1000},
        {LogicalTypeId::kDouble, 1000},
        {LogicalTypeId::kDecimal, 1000},
        {LogicalTypeId::kDate, 1000},
        {LogicalTypeId::kDateTime, 1000},
        {LogicalTypeId::kTime, 1000},
        {LogicalTypeId::kInterval, 1000},
        {LogicalTypeId::kVarchar, 1},
        {LogicalTypeId::kText, 0},
        {LogicalTypeId::kNull, -1},
        {LogicalTypeId::kAny, 1},
    })},
    {LogicalTypeId::kText, std::unordered_map<LogicalTypeId, int64_t>({
        {LogicalTypeId::kBoolean, -1},
        {LogicalTypeId::kTinyInt, -1},
        {LogicalTypeId::kSmallInt, -1},
        {LogicalTypeId::kInteger, -1},
        {LogicalTypeId::kBigInt, -1},
        {LogicalTypeId::kFloat, -1},
        {LogicalTypeId::kDouble, -1},
        {LogicalTypeId::kDecimal, -1},
        {LogicalTypeId::kDate, -1},
        {LogicalTypeId::kDateTime, -1},
        {LogicalTypeId::kTime, -1},
        {LogicalTypeId::kInterval, -1},
        {LogicalTypeId::kVarchar, -1},
        {LogicalTypeId::kText, -1},
        {LogicalTypeId::kNull, 0},
        {LogicalTypeId::kAny, -1},
    })},
    {LogicalTypeId::kAny, std::unordered_map<LogicalTypeId, int64_t>({
        {LogicalTypeId::kBoolean, -1},
        {LogicalTypeId::kTinyInt, -1},
        {LogicalTypeId::kSmallInt, -1},
        {LogicalTypeId::kInteger, -1},
        {LogicalTypeId::kBigInt, -1},
        {LogicalTypeId::kFloat, -1},
        {LogicalTypeId::kDouble, -1},
        {LogicalTypeId::kDecimal, -1},
        {LogicalTypeId::kDate, -1},
        {LogicalTypeId::kDateTime, -1},
        {LogicalTypeId::kTime, -1},
        {LogicalTypeId::kInterval, -1},
        {LogicalTypeId::kVarchar, -1},
        {LogicalTypeId::kText, -1},
        {LogicalTypeId::kNull, -1},
        {LogicalTypeId::kAny, 0},
    })},

};

int64_t
LogicalType::CastRule(const LogicalTypeId& from, const LogicalTypeId& to) {
    if(from == to) {
        return 0;
    }
    return TypeCastMap[from][to];
}

}
