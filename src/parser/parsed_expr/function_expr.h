//
// Created by jinhai on 23-2-28.
//

#pragma once

#include "common/types/alias/containers.h"
#include "parsed_expr.h"

namespace infinity {

class FunctionExpr : public ParsedExpr {
public:
    explicit FunctionExpr() : ParsedExpr(ParsedExprType::kFunction) {}

    ~FunctionExpr() override;

    [[nodiscard]] String ToString() const override;

public:
    String func_name_{};
    Vector<ParsedExpr *> *arguments_{nullptr};
    bool distinct_{false};
};

} // namespace infinity
