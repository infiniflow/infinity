//
// Created by JinHai on 2022/11/29.
//

#include "decimal128_info.h"
#include "common/types/number/decimal_type.h"

namespace infinity {

UniquePtr<Decimal128Info>
Decimal128Info::Make(i64 precision, i64 scale) {
    TypeAssert(scale <= precision, "Scale should not more than precision.")
    TypeAssert(precision <= 38, "Decimal128 max precision should not more than 38.")

    return MakeUnique<Decimal128Info>(precision, scale);
}

}