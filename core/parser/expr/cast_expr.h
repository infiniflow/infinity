//
// Created by jinhai on 23-3-4.
//

#pragma once

#include "expr.h"
#include "type/logical_type.h"
#include "type/type_info.h"
#include "type/data_type.h"
#include <memory>

namespace infinity {

class CastExpr final : public ParsedExpr {
public:
    explicit CastExpr
    (LogicalType logical_type, std::shared_ptr<TypeInfo> type_info_ptr)
        : ParsedExpr(ParsedExprType::kCast), data_type_(logical_type, std::move(type_info_ptr)) {}

    ~CastExpr() final;

    [[nodiscard]] std::string ToString() const override;

public:
    DataType data_type_;
    ParsedExpr *expr_{nullptr};
};

} // namespace infinity
