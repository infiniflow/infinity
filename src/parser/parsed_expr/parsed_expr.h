//
// Created by jinhai on 23-2-27.
//

#pragma once

#include "common/types/internal_types.h"
namespace infinity {

enum class ParsedExprType {
    kConstant,
    kExprStar,
    kExprParameter,
    kExprColumn,
    kExprFunction,
    kExprOperator,
    kExprSubquery,
    kExprCase,
    kExprCast
};

class ParsedExpr {
public:
    explicit
    ParsedExpr(ParsedExprType type) : type_(type) {}

    virtual
    ~ParsedExpr() = default;

    ParsedExprType type_;
    char* alias_{nullptr};
};

}