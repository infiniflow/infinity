//
// Created by jinhai on 23-1-22.
//

#pragma once

#include <utility>

#include "common/types/internal_types.h"
#include "parser/parsed_expression.h"

namespace infinity {
class ParsedColumnExpression : public ParsedExpression {
public:
    explicit
    ParsedColumnExpression(String db_name, String schema_name, String table_name, String column_name)
        : ParsedExpression(ExpressionType::kColumn),
        db_name_(std::move(db_name)),
        schema_name_(std::move(schema_name)),
        table_name_(std::move(table_name)),
        column_name_(std::move(column_name))
    {}

    [[nodiscard]] String
    ToString() const override;

public:
    String db_name_;
    String schema_name_;
    String table_name_;
    String column_name_;
};
}
