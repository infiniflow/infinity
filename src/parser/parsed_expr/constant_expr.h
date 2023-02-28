//
// Created by jinhai on 23-2-27.
//

#pragma once

#include "parsed_expr.h"

namespace infinity {

enum class LiteralType {
    kBoolean,
    kFloat,
    kString,
    kInteger,
    kNull,
    kDate,
    kInterval,
};

enum class IntervalExprType {
    kSecond,
    kMinute,
    kHour,
    kDay,
    kMonth,
    kYear,
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
    double float_value_{0};
    char* str_value_{nullptr};
    IntervalExprType interval_type_{IntervalExprType::kSecond};
    char* date_value_{nullptr};
};

}

