//
// Created by jinhai on 23-2-23.
//

#pragma once

#include "parser/statement/statement_common.h"

namespace infinity {

enum class DDLType {
    kInvalid,
    kSchema,
    kTable,
    kCollection,
    kView,
    kIndex,
};

enum class ConflictType {
    kInvalid,
    kIgnore,
    kError,
    kReplace
};

class ExtraDDLInfo {
public:
    explicit
    ExtraDDLInfo(DDLType type) : type_(type) {}

    [[nodiscard]] virtual String
    ToString() const = 0;

    DDLType type_{DDLType::kInvalid};

    ConflictType conflict_type_{ConflictType::kInvalid};
};

}
