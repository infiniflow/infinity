//
// Created by jinhai on 23-2-27.
//

#pragma once

#include "parsed_expr.h"
#include "common/types/logical_type.h"

namespace infinity {

enum class LiteralType {
    kBoolean,
    kDouble,
    kString,
    kInteger,
    kNull,
    kDate,
    kInterval,
};

class ConstantExpr : public ParsedExpr {
public:
    explicit
    ConstantExpr(LiteralType literal_type) : ParsedExpr(ParsedExprType::kConstant), literal_type_(literal_type) {}

    ~ConstantExpr() override;

    [[nodiscard]] String
    ToString() const override;

public:
    LiteralType literal_type_;

    bool bool_value_{false};
    i64 integer_value_{0};
    double double_value_{0};
    char* str_value_{nullptr};
    TimeUnit interval_type_{TimeUnit::kInvalidUnit};
    char* date_value_{nullptr};
};

}

