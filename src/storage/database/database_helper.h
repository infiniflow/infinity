//
// Created by jinhai on 23-5-27.
//

#pragma once

#include "common/types/alias/primitives.h"
#include "common/types/alias/strings.h"

namespace infinity {

class DatabaseHelper {
public:
    static i32 CreateLockerFile(const String &dir_name);
};

} // namespace infinity
