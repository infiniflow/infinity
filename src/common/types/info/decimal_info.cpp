//
// Created by JinHai on 2022/11/29.
//

#include "decimal_info.h"
#include "common/types/number/decimal_type.h"

namespace infinity {

SharedPtr<DecimalInfo>
DecimalInfo::Make(i64 precision, i64 scale) {
    TypeAssert(scale <= precision, "Scale should not more than precision.")
    TypeAssert(precision <= 38, "Decimal128 max precision should not more than 38.")

    return MakeShared<DecimalInfo>(precision, scale);
}

bool
DecimalInfo::operator==(const TypeInfo& other) const {
    if(other.type() != TypeInfoType::kDecimal) return false;

    auto* decimal128_info_ptr = (DecimalInfo*)(&other);

    return this->precision_ == decimal128_info_ptr->precision_
           && this->scale_ == decimal128_info_ptr->scale_;
}

}