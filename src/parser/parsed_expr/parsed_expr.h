//
// Created by jinhai on 23-2-27.
//

#pragma once

#include "common/types/internal_types.h"
#include "main/logger.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

enum class ParsedExprType {
    kConstant,
    kExprParameter,
    kExprColumn,
    kExprFunction,
    kExprOperator,
    kExprSubquery,
    kExprCase,
    kExprCast
};

class ParsedExpr {
public:
    explicit
    ParsedExpr(ParsedExprType type) : type_(type) {}

    virtual
    ~ParsedExpr() = default;

    [[nodiscard]] virtual String
    ToString() const = 0;

    ParsedExprType type_;
    String alias_{};
};

}