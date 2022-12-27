//
// Created by JinHai on 2022/10/28.
//

#include "decimal_info.h"
#include "common/types/number/decimal_type.h"
#include "common/types/info/decimal16_info.h"
#include "common/types/info/decimal32_info.h"
#include "common/types/info/decimal64_info.h"
#include "common/types/info/decimal128_info.h"

namespace infinity {

SharedPtr<TypeInfo>
DecimalInfo::Make(i64 precision, i64 scale) {
    switch(precision) {
        case 0 ... 4: {
            return Decimal16Info::Make(precision, scale);
        }
        case 5 ... 9: {
            return Decimal32Info::Make(precision, scale);
        }
        case 10 ... 18: {
            return Decimal64Info::Make(precision, scale);
        }
        case 19 ... 38: {
            return Decimal128Info::Make(precision, scale);
        }
        default: {
            TypeError("Unsupported decimal precision: " + std::to_string(precision));
        }
    }
}

LogicalType
DecimalInfo::GetDecimalType(i64 precision, i64 scale) {
    switch(precision) {
        case 0 ... 4: {
            return LogicalType::kDecimal16;
        }
        case 5 ... 9: {
            return LogicalType::kDecimal32;
        }
        case 10 ... 18: {
            return LogicalType::kDecimal64;
        }
        case 19 ... 38: {
            return LogicalType::kDecimal128;
        }
        default: {
            TypeError("Unsupported decimal precision: " + std::to_string(precision));
        }
    }
}

}
