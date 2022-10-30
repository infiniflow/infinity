//
// Created by JinHai on 2022/10/30.
//

#pragma once

#include "varlen_type.h"
#include "logical_type.h"


namespace infinity {

struct ArrayType : public VarlenType {
    LogicalType elem_type;
    i16 count;
    ptr_t ptr;
};

}


