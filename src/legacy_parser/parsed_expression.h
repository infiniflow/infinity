//
// Created by jinhai on 23-1-22.
//

#pragma once

#include <utility>

#include "common/types/expression_type.h"
#include "common/types/internal_types.h"

namespace infinity {

class ParsedExpression {
public:
    explicit
    ParsedExpression(ExpressionType type) : type_(type) {}

    explicit
    ParsedExpression(ExpressionType type, String alias) : type_(type), alias_(std::move(alias)) {}

    [[nodiscard]] inline String
    GetName() const {
        return alias_.empty() ? ToString() : alias_;
    }

    [[nodiscard]] virtual String
    ToString() const = 0;

public:
    ExpressionType type_;
    String alias_{};
};

}

