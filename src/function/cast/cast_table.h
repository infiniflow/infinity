//
// Created by jinhai on 23-1-5.
//

#pragma once

#include <array>

#include "common/singleton.h"
#include "common/types/logical_type.h"

namespace infinity {

class CastTable : public Singleton<CastTable> {
public:
    CastTable();

public:
    [[nodiscard]] i64 GetCastCost(LogicalType from, LogicalType to) const;

private:
    std::array<std::array<i64, LogicalType::kInvalid>, LogicalType::kInvalid> matrix_{-1};
};

} // namespace infinity
