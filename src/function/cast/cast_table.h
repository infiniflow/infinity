//
// Created by jinhai on 23-1-5.
//

#pragma once

#include "common/types/internal_types.h"
#include "common/types/logical_type.h"
#include "common/singleton.h"

namespace infinity {

class CastTable : public Singleton<CastTable> {
public:

    CastTable();

public:
    [[nodiscard]] i64
    GetCastCost(LogicalType from, LogicalType to) const;

private:

    std::array<std::array<i64, LogicalType::kInvalid>, LogicalType::kInvalid> matrix_{-1};
};

}
