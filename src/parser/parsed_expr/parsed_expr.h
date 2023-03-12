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
    kParameter,
    kColumn,
    kFunction,
    kBetween,
    kOperator,
    kSubquery,
    kCase,
    kCast
};

class ParsedExpr {
public:
    explicit
    ParsedExpr(ParsedExprType type) : type_(type) {}

    virtual
    ~ParsedExpr() = default;

    inline String
    GetName() const {
        return ToString();
    }

    [[nodiscard]] virtual String
    ToString() const = 0;

    ParsedExprType type_;
    String alias_{};
};

}