//
// Created by jinhai on 23-2-27.
//

#pragma once

#include "expr.h"
#include "type/datetime/interval_type.h"
#include <vector>

namespace infinity {

enum class LiteralType {
    kBoolean,
    kDouble,
    kString,
    kInteger,
    kNull,
    kDate,
    kIntegerArray,
    kDoubleArray,
    kInterval,
};

class ConstantExpr : public ParsedExpr {
public:
    explicit ConstantExpr(LiteralType literal_type) : ParsedExpr(ParsedExprType::kConstant), literal_type_(literal_type) {}

    ~ConstantExpr() override;

    [[nodiscard]] std::string ToString() const override;

public:
    LiteralType literal_type_;

    bool bool_value_{false};
    int64_t integer_value_{0};
    double double_value_{0};
    char *str_value_{nullptr};
    TimeUnit interval_type_{TimeUnit::kInvalidUnit};
    char *date_value_{nullptr};
    std::vector<int64_t> long_array_{};
    std::vector<double> double_array_{};
};

} // namespace infinity
