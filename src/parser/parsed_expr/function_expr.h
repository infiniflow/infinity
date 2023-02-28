//
// Created by jinhai on 23-2-28.
//

#pragma once

#include "parsed_expr.h"

namespace infinity {

class FunctionExpr : public ParsedExpr {
public:
    explicit
    FunctionExpr() : ParsedExpr(ParsedExprType::kExprFunction) {}

    ~FunctionExpr() override;

    [[nodiscard]] String
    ToString() const override;

public:
    char* func_name_{nullptr};
    Vector<ParsedExpr*> *arguments_{nullptr};
    bool distinct_{false};
};

}
