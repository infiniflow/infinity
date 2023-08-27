//
// Created by jinhai on 23-2-24.
//

#pragma once

#include "common/types/internal_types.h"

namespace infinity {

enum class CopyFileType {
    kCSV,
    kJSON,
};

struct TableName {
    char* schema_name_ptr_{nullptr};
    char* table_name_ptr_{nullptr};
};

}


