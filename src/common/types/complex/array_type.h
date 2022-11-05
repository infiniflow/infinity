//
// Created by JinHai on 2022/10/30.
//

#pragma once

#include "common/types/varlen_type.h"
#include "common/types/logical_type.h"


namespace infinity {

struct ArrayType {
    LogicalType elem_type;
    i16 count;
    ptr_t ptr;
};

}


