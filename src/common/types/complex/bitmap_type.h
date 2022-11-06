//
// Created by JinHai on 2022/10/31.
//

#pragma once

#include "common/types/varlen_type.h"

namespace infinity {

struct BitmapType {
    i64 count; // bit count of the bitmap
    ptr_t ptr;
};

}
