//
// Created by JinHai on 2022/10/31.
//

#pragma once

#include "common/types/varlen_type.h"

namespace infinity {

// bitmap is a fix length array, even look like vector with bit
struct BitmapType {
    i16 count; // bit count of the bitmap
    ptr_t ptr;
};

}
