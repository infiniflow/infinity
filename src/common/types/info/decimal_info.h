//
// Created by JinHai on 2022/10/28.
//

#pragma once

#include "common/types/type_info.h"
#include "common/types/logical_type.h"
#include "common/types/internal_types.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

struct DecimalInfo {

static SharedPtr<TypeInfo>
Make(i64 precision, i64 scale);

static LogicalType
GetDecimalType(i64 precision, i64 scale);

};

}

