//
// Created by JinHai on 2022/10/30.
//

#pragma once

#include "common/types/varlen_type.h"

namespace infinity {

struct BlobType {
    ptr_t pos;
    i64 length;  // 4GB will be the limitation.
};

}
