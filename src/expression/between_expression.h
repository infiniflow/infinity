//
// Created by JinHai on 2022/8/5.
//

#pragma once

#include "base_expression.h"

namespace infinity {

class BetweenExpression: public BaseExpression {
public:
    BetweenExpression(const std::shared_ptr<BaseExpression>& value,
                      const std::shared_ptr<BaseExpression>& upper_bound,
                      const std::shared_ptr<BaseExpression>& lower_bound,
                      bool upper_inclusive,
                      bool lower_inclusive);

    std::string ToString() const override;
    LogicalType DataType() override;

private:
    bool lower_inclusive_{false};
    bool upper_inclusive_{false};
};

}
