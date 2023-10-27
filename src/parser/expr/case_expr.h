//
// Created by jinhai on 23-3-4.
//

#pragma once

#include "expr.h"
#include <string>
#include <vector>

namespace infinity {

struct WhenThen {
    ~WhenThen();

    ParsedExpr *when_{nullptr};
    ParsedExpr *then_{nullptr};
};

class CaseExpr final : public ParsedExpr {
public:
    explicit CaseExpr() : ParsedExpr(ParsedExprType::kCase) {}

    ~CaseExpr() final;

    [[nodiscard]] std::string ToString() const override;

public:
    ParsedExpr *expr_{nullptr};
    ParsedExpr *else_expr_{nullptr};
    std::vector<WhenThen *> *case_check_array_{nullptr};
};

} // namespace infinity
