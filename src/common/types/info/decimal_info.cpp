//
// Created by JinHai on 2022/10/28.
//

#include "decimal_info.h"
#include "common/types/number/decimal_type.h"

namespace infinity {

UniquePtr<DecimalInfo>
DecimalInfo::Make(LogicalType logical_type, i64 precision, i64 scale) {
    // Make sure the precision does not exceed the limitation of the specified decimal type.

    switch(logical_type) {
        case kDecimal16: {
            return MakeUnique<DecimalInfo>(TypeInfoType::kDecimal16, precision, scale);
        }
        case kDecimal32: {
            return MakeUnique<DecimalInfo>(TypeInfoType::kDecimal32, precision, scale);
        }
        case kDecimal64: {
            return MakeUnique<DecimalInfo>(TypeInfoType::kDecimal64, precision, scale);
        }
        case kDecimal128: {
            return MakeUnique<DecimalInfo>(TypeInfoType::kDecimal128, precision, scale);
        }
        default: {
            TypeError("Unexpected error");
        }
    }
}

UniquePtr<DecimalInfo>
DecimalInfo::Make(i64 precision, i64 scale) {

    TypeInfoType type_info(TypeInfoType::kInvalid);

    switch(DecimalType::GetLogicalType(precision)) {
        case LogicalType::kDecimal16: {
            type_info = TypeInfoType::kDecimal16;
            break;
        }
        case LogicalType::kDecimal32: {
            type_info = TypeInfoType::kDecimal32;
            break;
        }
        case LogicalType::kDecimal64: {
            type_info = TypeInfoType::kDecimal64;
            break;
        }
        case LogicalType::kDecimal128: {
            type_info = TypeInfoType::kDecimal128;
            break;
        }
        default: {
            TypeError("Unexpected error");
        }
    }

    return MakeUnique<DecimalInfo>(type_info, precision, scale);
}

}
