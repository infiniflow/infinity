//
// Created by jinhai on 23-2-27.
//

#pragma once

#include <string>

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

    [[nodiscard]] inline std::string GetName() const {
        if (HasAlias())
            return GetAlias();
        return ToString();
    }

    inline std::string GetAlias() const { return alias_; }

    [[nodiscard]] inline bool HasAlias() const { return !alias_.empty(); }

    [[nodiscard]] virtual std::string ToString() const = 0;

    ParsedExprType type_;
    std::string alias_{};
};

} // namespace infinity