//
// Created by JinHai on 2022/11/29.
//

#include "decimal16_info.h"
#include "common/types/number/decimal_type.h"

namespace infinity {

UniquePtr<Decimal16Info>
Decimal16Info::Make(i64 precision, i64 scale) {
    TypeAssert(scale <= precision, "Scale should not more than precision.")
    TypeAssert(precision <= 4, "Decimal16 max precision should not more than 4.")

    return MakeUnique<Decimal16Info>(precision, scale);
}

}