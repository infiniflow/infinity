//
// Created by jinhai on 23-3-19.
//

#pragma once

#include "common/types/alias/containers.h"
#include "parsed_expr.h"

namespace infinity {

class InExpr : public ParsedExpr {
public:
    explicit InExpr(bool not_in = false) : ParsedExpr(ParsedExprType::kIn), not_in_(not_in) {}

    ~InExpr() override;

    [[nodiscard]] String ToString() const override;

public:
    ParsedExpr *left_{nullptr};
    Vector<ParsedExpr *> *arguments_{nullptr};
    bool not_in_{false};
};

} // namespace infinity
