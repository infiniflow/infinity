//
// Created by jinhai on 23-1-22.
//

#pragma once

#include <utility>

#include "sql/Expr.h"
#include "common/types/internal_types.h"
#include "legacy_parser/parsed_expression.h"

namespace infinity {

class ParsedRawExpression : public ParsedExpression {
public:
    explicit
    ParsedRawExpression(const hsql::Expr* raw_expr)
            : ParsedExpression(ExpressionType::kRaw),
              raw_expr_(raw_expr)
    {
        if(raw_expr_->alias != nullptr) {
            alias_ = raw_expr->alias;
        }
    }

    [[nodiscard]] String
    ToString() const override;

public:
    const hsql::Expr* raw_expr_ {};
};

}
