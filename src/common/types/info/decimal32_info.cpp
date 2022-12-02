//
// Created by JinHai on 2022/11/29.
//

#include "decimal32_info.h"
#include "common/types/number/decimal_type.h"

namespace infinity {

SharedPtr<Decimal32Info>
Decimal32Info::Make(i64 precision, i64 scale) {
    TypeAssert(scale <= precision, "Scale should not more than precision.")
    TypeAssert(precision <= 9, "Decimal32 max precision should not more than 9.")

    return MakeShared<Decimal32Info>(precision, scale);
}

bool
Decimal32Info::operator==(const TypeInfo& other) const {
    if(other.type() != TypeInfoType::kDecimal32) return false;

    auto* decimal32_info_ptr = (Decimal32Info*)(&other);

    return this->precision_ == decimal32_info_ptr->precision_
           && this->scale_ == decimal32_info_ptr->scale_;
}

}