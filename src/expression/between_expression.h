//
// Created by JinHai on 2022/8/5.
//

#pragma once

#include "base_expression.h"

namespace infinity {

class BetweenExpression : public BaseExpression {
public:
    BetweenExpression(const SharedPtr<BaseExpression>& value,
                      const SharedPtr<BaseExpression>& upper_bound,
                      const SharedPtr<BaseExpression>& lower_bound,
                      bool upper_inclusive,
                      bool lower_inclusive);

    String
    ToString() const override;

    DataType
    Type() const override;

private:
    bool lower_inclusive_{false};
    bool upper_inclusive_{false};
};

}
