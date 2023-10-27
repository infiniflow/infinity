//
// Created by jinhai on 23-2-23.
//

#pragma once

#include <string>

namespace infinity {

enum class DDLType {
    kInvalid,
    kSchema,
    kTable,
    kCollection,
    kView,
    kIndex,
};

enum class ConflictType { kInvalid, kIgnore, kError, kReplace };

std::string ConflictTypeToStr(ConflictType type);

class ExtraDDLInfo {
public:
    explicit ExtraDDLInfo(DDLType type) : type_(type) {}

    virtual ~ExtraDDLInfo() = default;

    [[nodiscard]] virtual std::string ToString() const = 0;

    DDLType type_{DDLType::kInvalid};

    ConflictType conflict_type_{ConflictType::kInvalid};
};

} // namespace infinity
