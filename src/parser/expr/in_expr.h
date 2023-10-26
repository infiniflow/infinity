//
// Created by jinhai on 23-3-19.
//

#pragma once

#include "expr.h"
#include <string>
#include <vector>

namespace infinity {

class InExpr : public ParsedExpr {
public:
    explicit InExpr(bool not_in = false) : ParsedExpr(ParsedExprType::kIn), not_in_(not_in) {}

    ~InExpr() override;

    [[nodiscard]] std::string ToString() const override;

public:
    ParsedExpr *left_{nullptr};
    std::vector<ParsedExpr *> *arguments_{nullptr};
    bool not_in_{false};
};

} // namespace infinity
