//
// Created by JinHai on 2022/10/28.
//

#pragma once

#include "common/types/varlen_type.h"

namespace infinity {

struct VarcharType {
    ptr_t pos;   // pointer to the varchar value buffer.
    i32 length;  // 4GB will be the limitation.
    i32 _dummy;  // padding to align with 8 bytes.
};

}

