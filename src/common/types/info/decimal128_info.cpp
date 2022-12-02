//
// Created by JinHai on 2022/11/29.
//

#include "decimal128_info.h"
#include "common/types/number/decimal_type.h"

namespace infinity {

SharedPtr<Decimal128Info>
Decimal128Info::Make(i64 precision, i64 scale) {
    TypeAssert(scale <= precision, "Scale should not more than precision.")
    TypeAssert(precision <= 38, "Decimal128 max precision should not more than 38.")

    return MakeShared<Decimal128Info>(precision, scale);
}

bool
Decimal128Info::operator==(const TypeInfo& other) const {
    if(other.type() != TypeInfoType::kDecimal128) return false;

    auto* decimal128_info_ptr = (Decimal128Info*)(&other);

    return this->precision_ == decimal128_info_ptr->precision_
           && this->scale_ == decimal128_info_ptr->scale_;
}

}