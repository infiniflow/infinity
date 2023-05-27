//
// Created by jinhai on 23-5-27.
//

#pragma once

#include "common/types/internal_types.h"

namespace infinity {

class DatabaseHelper {
public:
    static i32
    CreateLockerFile(const String& dir_name);
};

}
