//
// Created by JinHai on 2022/11/29.
//

#include "decimal32_info.h"
#include "common/types/number/decimal_type.h"

namespace infinity {

UniquePtr<Decimal32Info>
Decimal32Info::Make(i64 precision, i64 scale) {
    TypeAssert(scale <= precision, "Scale should not more than precision.")
    TypeAssert(precision <= 9, "Decimal32 max precision should not more than 9.")

    return MakeUnique<Decimal32Info>(precision, scale);
}

}