//
// Created by JinHai on 2022/11/7.
//

#pragma once

#include "common/types/internal_types.h"

namespace infinity {

struct DateType {
    DateType() = default;

    explicit
    DateType(i32 date_value): value(date_value) {};

    i32 value{0};
};

}