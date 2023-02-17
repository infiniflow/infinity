//
// Created by JinHai on 2022/8/5.
//

#pragma once

#include "base_expression.h"

namespace infinity {

class CastExpression: public BaseExpression {
public:
    CastExpression(const SharedPtr<BaseExpression>& argument, DataType target_type);

    inline DataType
    Type() const override {
        return target_type_;
    }

    String
    ToString() const override;

    static bool
    CanCast(const DataType& source, const DataType& target);

    static SharedPtr<BaseExpression>
    AddCastToType(const SharedPtr<BaseExpression>& expr, const DataType &target_type);

private:
    DataType target_type_;

    // TODO: Cast function should be scalar function
};

}
