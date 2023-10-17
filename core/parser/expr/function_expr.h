//
// Created by jinhai on 23-2-28.
//

#pragma once

#include "expr.h"
#include <string>
#include <vector>

namespace infinity {

class FunctionExpr : public ParsedExpr {
public:
    explicit FunctionExpr() : ParsedExpr(ParsedExprType::kFunction) {}

    ~FunctionExpr() override;

    [[nodiscard]] std::string ToString() const override;

public:
    std::string func_name_{};
    std::vector<ParsedExpr *> *arguments_{nullptr};
    bool distinct_{false};
};

} // namespace infinity
