//
// Created by jinhai on 23-2-24.
//

#pragma once

#include "common/types/alias/smart_ptr.h"

namespace infinity {

enum class CopyFileType {
    kCSV,
    kJSON,
    kFVECS,
};

inline SharedPtr<String>
copy_file_to_str(CopyFileType copy_file_type) {
    switch(copy_file_type) {
        case CopyFileType::kCSV:
            return MakeShared<String>("CSV");
        case CopyFileType::kJSON:
            return MakeShared<String>("JSON");
        case CopyFileType::kFVECS:
            return MakeShared<String>("FVECS");
    }
}

struct TableName {
    char* schema_name_ptr_{nullptr};
    char* table_name_ptr_{nullptr};
};

}


