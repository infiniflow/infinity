//
// Created by JinHai on 2022/10/28.
//

#pragma once

#include "common/types/varlen_type.h"

namespace infinity {

struct VarcharType {
    ptr_t ptr;   // pointer to the varchar value buffer.
    i16 length;  // 65535 will be the limitation.
    i16 _dummy1; // padding to align with 8 bytes.
    i32 _dummy2;  // padding to align with 8 bytes.
};

}

