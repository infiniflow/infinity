//
// Created by jinhai on 23-2-26.
//

#include "data_type_builder.h"
#include "common/types/info/decimal_info.h"
#include "common/types/info/varchar_info.h"
#include "common/types/info/embedding_info.h"
#include "common/types/info/char_info.h"

namespace infinity {

DataType
DataTypeBuilder::Make(hsql::ColumnType type) {
    switch (type.data_type) {
        case hsql::DataType::UNKNOWN: {
            TypeError("Unknown parsed data type.")
        }
        case hsql::DataType::BIGINT: {
            return DataType(LogicalType::kBigInt);
        }
        case hsql::DataType::BOOLEAN: {
            return DataType(LogicalType::kBoolean);
        }
        case hsql::DataType::CHAR: {
            SharedPtr<CharInfo> char_info_ptr = CharInfo::Make(type.length);
            return DataType(LogicalType::kChar, char_info_ptr);
        }
        case hsql::DataType::DATE: {
            return DataType(LogicalType::kDate);
        }
        case hsql::DataType::DATETIME: {
            return DataType(LogicalType::kDateTime);
        }
        case hsql::DataType::DECIMAL: {
            SharedPtr<TypeInfo> decimal_info_ptr = DecimalInfo::Make(type.precision, type.scale);
            LogicalType decimal_type = DecimalInfo::GetDecimalType(type.precision, type.scale);
            return DataType(decimal_type, decimal_info_ptr);
        }
        case hsql::DataType::DOUBLE: {
            return DataType(LogicalType::kDouble);
        }
        case hsql::DataType::FLOAT: {
            return DataType(LogicalType::kFloat);
        }
        case hsql::DataType::INT: {
            return DataType(LogicalType::kInteger);
        }
        case hsql::DataType::LONG: {
            return DataType(LogicalType::kBigInt);
        }
        case hsql::DataType::REAL: {
            return DataType(LogicalType::kFloat);
        }
        case hsql::DataType::SMALLINT: {
            return DataType(LogicalType::kSmallInt);
        }
        case hsql::DataType::TEXT: {
            SharedPtr<VarcharInfo> varchar_info_ptr = VarcharInfo::Make(type.length);
            return DataType(LogicalType::kVarchar, varchar_info_ptr);
        }
        case hsql::DataType::TIME: {
            return DataType(LogicalType::kTime);
        }
        case hsql::DataType::VARCHAR: {
            SharedPtr<VarcharInfo> varchar_info_ptr = VarcharInfo::Make(type.length);
            return DataType(LogicalType::kVarchar, varchar_info_ptr);
        }
    }

    TypeError("Unexpected data type.")
}

DataType
DataTypeBuilder::ConvertType(hsql::ColumnType type) {
    switch (type.data_type) {
        case hsql::DataType::BOOLEAN: return DataType(LogicalType::kBoolean);
            // No TinyInt
        case hsql::DataType::SMALLINT: return DataType(LogicalType::kSmallInt);
        case hsql::DataType::INT: return DataType(LogicalType::kInteger);
        case hsql::DataType::BIGINT:
        case hsql::DataType::LONG: return DataType(LogicalType::kBigInt);;

        case hsql::DataType::DECIMAL: {
            LogicalType logical_type = DecimalType::GetLogicalType(type.precision);
            SharedPtr<TypeInfo> decimal_info = DecimalInfo::Make(type.precision, type.scale);
            return DataType(logical_type, std::move(decimal_info));
        }

        case hsql::DataType::REAL:
        case hsql::DataType::FLOAT: return DataType(LogicalType::kFloat);
        case hsql::DataType::DOUBLE: return DataType(LogicalType::kDouble);

        case hsql::DataType::CHAR:
        case hsql::DataType::VARCHAR:
        case hsql::DataType::TEXT: {
            return DataType(LogicalType::kVarchar, VarcharInfo::Make(type.length));
        }

        case hsql::DataType::DATE: return DataType(LogicalType::kDate);
        case hsql::DataType::TIME: return DataType(LogicalType::kTime);
        case hsql::DataType::DATETIME: return DataType(LogicalType::kDateTime);
        case hsql::DataType::UNKNOWN: {
            TypeError("Unknown date type.");
        }
    }

    TypeError("Unexpected date type.");
}

}
