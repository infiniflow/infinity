//
// Created by JinHai on 2022/10/28.
//

#pragma once

#include "common/types/internal_types.h"

namespace infinity {

struct DateTimeType {
    i32 date;
    i32 time;
};

using TimestampType = DateTimeType;
using TimestampTZType = DateTimeType;

}
