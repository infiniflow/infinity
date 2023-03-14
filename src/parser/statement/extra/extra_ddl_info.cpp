//
// Created by jinhai on 23-3-14.
//

#include "extra_ddl_info.h"

namespace infinity {

String
ConflictTypeToStr(ConflictType type) {
    switch(type) {
        case ConflictType::kInvalid: {
            PlannerError("Invalid conflict type")
        }
        case ConflictType::kIgnore: {
            return "Ignore";
        }
        case ConflictType::kError: {
            return "Issue error";
        }
        case ConflictType::kReplace: {
            return "Replace";
        }
    }
    PlannerError("Unreachable")
}

}