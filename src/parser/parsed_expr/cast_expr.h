//
// Created by jinhai on 23-3-4.
//

#pragma once

#include "common/types/data_type.h"
#include "parsed_expr.h"

namespace infinity {

class CastExpr final : public ParsedExpr {
public:
    explicit CastExpr(LogicalType logical_type, SharedPtr<TypeInfo> type_info_ptr)
        : ParsedExpr(ParsedExprType::kCast), data_type_(logical_type, std::move(type_info_ptr)) {}

    ~CastExpr() final;

    [[nodiscard]] String ToString() const override;

public:
    DataType data_type_;
    ParsedExpr *expr_{nullptr};
};

} // namespace infinity
