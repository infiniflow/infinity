//
// Created by JinHai on 2022/8/5.
//

#pragma once

#include "base_expression.h"
#include "function/cast/bound_cast_func.h"

namespace infinity {

class CastExpression : public BaseExpression {
public:
    CastExpression(BoundCastFunc cast_function,
                   const SharedPtr<BaseExpression>& argument,
                   DataType target_type)
            : BaseExpression(ExpressionType::kCast, {argument}),
              target_type_(std::move(target_type)),
              func_(cast_function) {}

    inline DataType
    Type() const override {
        return target_type_;
    }

    String
    ToString() const override;

    static bool
    CanCast(const DataType& source, const DataType& target);

    static SharedPtr<BaseExpression>
    AddCastToType(const SharedPtr<BaseExpression>& expr, const DataType& target_type);

    BoundCastFunc func_;

private:
    DataType target_type_;
};

}
