//
// Created by jinhai on 23-3-4.
//

#pragma once

#include "parsed_expr.h"

namespace infinity {

struct WhenThen {
    ~WhenThen();
    ParsedExpr* when_{nullptr};
    ParsedExpr* then_{nullptr};
};

class CaseExpr final : public ParsedExpr {
public:
    explicit
    CaseExpr() : ParsedExpr(ParsedExprType::kCase) {}

    ~CaseExpr() final;

    [[nodiscard]] String
    ToString() const override;

public:
    ParsedExpr* expr_{nullptr};
    ParsedExpr* else_expr_{nullptr};
    Vector<WhenThen*>* case_check_array_{nullptr};
};

}
