//
// Created by JinHai on 2022/8/5.
//

#pragma once

#include "base_expression.h"

namespace infinity {

enum class ConjunctionType {
    kInvalid,
    kAnd,
    kOr,
};

class ConjunctionExpression: public BaseExpression {
public:
    ConjunctionExpression(ConjunctionType conjunction_type,
                          const std::shared_ptr<BaseExpression>& left_operand,
                          const std::shared_ptr<BaseExpression>& right_operand);

    LogicalType DataType() override;
    std::string ToString() const override;
    ConjunctionType conjunction_type() const { return conjunction_type_; }

    LogicalType data_type_{LogicalTypeId::kBoolean};
    ConjunctionType conjunction_type_{ConjunctionType::kInvalid};


};


}