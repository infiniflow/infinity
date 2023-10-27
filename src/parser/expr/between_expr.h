//
// Created by jinhai on 23-3-12.
//

#pragma once

#include "expr.h"

namespace infinity {

class BetweenExpr : public ParsedExpr {
public:
    explicit BetweenExpr() : ParsedExpr(ParsedExprType::kBetween) {}

    ~BetweenExpr() override;

    [[nodiscard]] std::string ToString() const override;

public:
    ParsedExpr *value_{nullptr};
    ParsedExpr *upper_bound_{nullptr};
    ParsedExpr *lower_bound_{nullptr};
};

} // namespace infinity
