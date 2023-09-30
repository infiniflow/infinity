//
// Created by jinhai on 23-2-23.
//

#pragma once

#include "common/utility/infinity_assert.h"
#include "common/types/alias/strings.h"

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

String
ConflictTypeToStr(ConflictType type);

class ExtraDDLInfo {
public:
    explicit
    ExtraDDLInfo(DDLType type) : type_(type) {}

    virtual
    ~ExtraDDLInfo() = default;

    [[nodiscard]] virtual String
    ToString() const = 0;

    DDLType type_{DDLType::kInvalid};

    ConflictType conflict_type_{ConflictType::kInvalid};
};

}
