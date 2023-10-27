//
// Created by jinhai on 23-3-4.
//

#pragma once

#include "expr.h"
#include "statement/select_statement.h"

namespace infinity {

enum class SubqueryType {
    kExists,
    kNotExists,
    kIn,
    kNotIn,
    kScalar,
    kAny,
};

class SubqueryExpr final : public ParsedExpr {
public:
    explicit SubqueryExpr() : ParsedExpr(ParsedExprType::kSubquery) {}

    ~SubqueryExpr() final;

    [[nodiscard]] std::string ToString() const override;

public:
    SubqueryType subquery_type_{SubqueryType::kScalar};

    ParsedExpr *left_{nullptr};
    SelectStatement *select_{nullptr};
};

} // namespace infinity
