//
// Created by jinhai on 23-2-27.
//

#pragma once

#include "common/utility/infinity_assert.h"
#include "main/logger.h"

#include <sstream>

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
    kCast,
    kIn,
    kKnn,
};

class ParsedExpr {
public:
    explicit ParsedExpr(ParsedExprType type) : type_(type) {}

    virtual ~ParsedExpr() = default;

    [[nodiscard]] inline String GetName() const {
        if (HasAlias())
            return GetAlias();
        return ToString();
    }

    inline String GetAlias() const { return alias_; }

    [[nodiscard]] inline bool HasAlias() const { return !alias_.empty(); }

    [[nodiscard]] virtual String ToString() const = 0;

    ParsedExprType type_;
    String alias_{};
};

} // namespace infinity