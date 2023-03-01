//
// Created by jinhai on 23-2-28.
//

#pragma once

#include "parsed_expr.h"

namespace infinity {

class ColumnExpr : public ParsedExpr {
public:
    explicit
    ColumnExpr() : ParsedExpr(ParsedExprType::kColumn) {}

    ~ColumnExpr() override;

    [[nodiscard]] String
    ToString() const override;

public:
    Vector<char*> names_{};
    bool star_{false};
};

}
