//
// Created by JinHai on 2022/11/29.
//

#include "decimal16_info.h"
#include "common/types/number/decimal_type.h"

namespace infinity {

SharedPtr<Decimal16Info>
Decimal16Info::Make(i64 precision, i64 scale) {
    TypeAssert(scale <= precision, "Scale should not more than precision.")
    TypeAssert(precision <= 4, "Decimal16 max precision should not more than 4.")

    return MakeShared<Decimal16Info>(precision, scale);
}

bool
Decimal16Info::operator==(const TypeInfo& other) const {
    if(other.type() != TypeInfoType::kDecimal16) return false;

    auto* decimal16_info_ptr = (Decimal16Info*)(&other);

    return this->precision_ == decimal16_info_ptr->precision_
           && this->scale_ == decimal16_info_ptr->scale_;
}


}