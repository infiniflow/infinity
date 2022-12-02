//
// Created by JinHai on 2022/11/29.
//

#include "decimal64_info.h"
#include "common/types/number/decimal_type.h"

namespace infinity {

SharedPtr<Decimal64Info>
Decimal64Info::Make(i64 precision, i64 scale) {
    TypeAssert(scale <= precision, "Scale should not more than precision.")
    TypeAssert(precision <= 18, "Decimal64 max precision should not more than 18.")

    return MakeShared<Decimal64Info>(precision, scale);
}

bool
Decimal64Info::operator==(const TypeInfo& other) const {
    if(other.type() != TypeInfoType::kDecimal64) return false;

    auto* decimal64_info_ptr = (Decimal64Info*)(&other);

    return this->precision_ == decimal64_info_ptr->precision_
           && this->scale_ == decimal64_info_ptr->scale_;
}

}