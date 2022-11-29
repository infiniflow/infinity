//
// Created by JinHai on 2022/11/29.
//

#include "decimal64_info.h"
#include "common/types/number/decimal_type.h"

namespace infinity {

UniquePtr<Decimal64Info>
Decimal64Info::Make(i64 precision, i64 scale) {
    TypeAssert(scale <= precision, "Scale should not more than precision.")
    TypeAssert(precision <= 18, "Decimal64 max precision should not more than 18.")

    return MakeUnique<Decimal64Info>(precision, scale);
}

}